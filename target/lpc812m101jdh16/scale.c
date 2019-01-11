/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

/* Throughout the following, note that all Chapter, Section, Page references 
 * are to the lpc81x user manual at
 *
 * http://www.nxp.com/docs/en/user-guide/UM10601.pdf
 */

#include "scale.h"

scale_conf_t SCALE_CONF = {
  .clock_type        = SCALE_CLOCK_TYPE_INT,
  .clock_freq_source = SCALE_CLOCK_FREQ_12MHZ,
  .clock_freq_target = SCALE_CLOCK_FREQ_12MHZ,

  .tsc               = true
};

bool scale_init( scale_conf_t* conf ) {
  memcpy( &scale_conf, conf, sizeof( scale_conf_t ) );

  /* To ensure some peripherals function at all, we need to explicitly enable
   * the clock supplied to them; it's important this is done first, since the
   * update of any control register for said peripheral would fail otherwise.
   * Then we force a reset of any peripheral we can.
   */

    LPC81X_SYSCON->SYSAHBCLKCTRL  |=  ( 0x1 <<  5 ) ;        // Table  30:  enable I2C   clock
    LPC81X_SYSCON->SYSAHBCLKCTRL  |=  ( 0x1 <<  6 ) ;        // Table  30:  enable GPIO  clock
    LPC81X_SYSCON->SYSAHBCLKCTRL  |=  ( 0x1 <<  7 ) ;        // Table  21:  enable SWM   clock
    LPC81X_SYSCON->SYSAHBCLKCTRL  |=  ( 0x1 << 14 ) ;        // Table  30:  enable UART  clock
    LPC81X_SYSCON->SYSAHBCLKCTRL  |=  ( 0x1 << 18 ) ;        // Table  21:  enable IOCON clock

    LPC81X_SYSCON->PRESETCTRL     &= ~( 0x1 <<  2 ) ;        // Table  19:  enable reset UART FGR
    LPC81X_SYSCON->PRESETCTRL     |=  ( 0x1 <<  2 ) ;        // Table  19: disable reset UART FGR
    LPC81X_SYSCON->PRESETCTRL     &= ~( 0x1 <<  3 ) ;        // Table  19:  enable reset UART
    LPC81X_SYSCON->PRESETCTRL     |=  ( 0x1 <<  3 ) ;        // Table  19: disable reset UART
    LPC81X_SYSCON->PRESETCTRL     &= ~( 0x1 <<  6 ) ;        // Table  19:  enable reset I2C
    LPC81X_SYSCON->PRESETCTRL     |=  ( 0x1 <<  6 ) ;        // Table  19: disable reset I2C
    LPC81X_SYSCON->PRESETCTRL     &= ~( 0x1 << 10 ) ;        // Table  19:  enable reset GPIO
    LPC81X_SYSCON->PRESETCTRL     |=  ( 0x1 << 10 ) ;        // Table  19: disable reset GPIO

  /* The majority of I/O pins have a configurable role; the configuration is
   * outlined in Chapters 6 and 9 (the latter relating to the switch matrix),
   * which are used to match all board-specific pins.  It's important this is 
   * done next, since that configuration potentially dictates whether or not 
   * the clock will subsequently work.
   */

    LPC81X_SWM->PINASSIGN0         = 0xFFFF0004;             // Table 109: map RxD => PIO0_0, TxD => PIO0_4
    LPC81X_SWM->PINASSIGN7         = 0x0AFFFFFF;             // Table 116: map SDA => PIO0_10
    LPC81X_SWM->PINASSIGN8         = 0xFFFFFF0B;             // Table 117: map SCL => PIO0_11
  
    LPC81X_SWM->PINENABLE0         = 0xFFFFF183;             // Table 118: enable pins (e.g., RxD => PIO0_0 vs. ACMP_I1)

    LPC81X_IOCON->PIO0_8          &= ~( 0x3 <<  3 ) ;        // Table  75: PIO0_8  => disable pull-up/down
    LPC81X_IOCON->PIO0_9          &= ~( 0x3 <<  3 ) ;        // Table  74: PIO0_9  => disable pull-up/down
    LPC81X_IOCON->PIO0_12          =  ( 0x2 <<  3 ) ;        // Table  64: PIO0_12 =>  enable pull-up
  
    LPC81X_IOCON->PIO0_10          =  ( 0x2 <<  8 ) ;        // Table  70: PIO0_10 => configure in fast mode plus (@ 1MHz)
    LPC81X_IOCON->PIO0_11          =  ( 0x2 <<  8 ) ;        // Table  69: PIO0_11 => configure in fast mode plus (@ 1MHz)
  
  /* Section 4.5 gives a complete overview of clock generation; it highlights
   * the existence of three oscillators, of which two are pertinent:
   *
   * a) the system oscillator is used to stimulate an external crystal, or to
   *    accept a generated clock signal, and
   * b) the Internal RC oscillator (IRC) is used as an alternative if/when no
   *    such crystal exists: this is the default on reset.
   *
   * As such, there are two main configuration challenges:
   *
   * 1. Section 4.3.1 outlines the system PLL, which generates a clock signal
   *    from one of the oscillator:
   *
   *    IRC      oscillator --->|\                                 +------------+
   *                            | |-- system PLL input (FCLKIN) -->| system PLL |-- system PLL output (FCLKOUT) --> ...
   *    system   oscillator --->|/                                 +------------+
   *
   *    FCLKIN is selected via SYSPLLCLKSEL, with the PLL then configured via
   *    two parameters denoted M and P to generate FCLKOUT:
   *
   *    FCLKOUT = M * FCLKIN = FCCO / ( 2 * P )
   * 
   *    where FCCO denotes the frequency of the Current Controlled Oscillator 
   *    (CCO).  Per Table 46 we must ensure
   *
   *     10 MHz <= FCLKIN  <=  25 MHz
   *    156 MHz <= FCCO    <= 320 MHz
   *               FCLKOUT <  100 MHz
   *
   *    for everything to work.
   *
   * 2. Figure 3 illustrates how clock generation operates as a whole, with
   *    the following section capturing the main idea:
   *
   *    IRC      oscillator --->|\                     +------------------------+
   *                            | |                +-->| system   clock divider | --> to core (AHB clock #0) and/or AHB peripherals (AHB clocks #1 to #18) based on SYSAHBCLKCTRL
   *    watchdog oscillator --->| |                |   +------------------------+
   *                            | |-- main clock --+-->| UART     clock divider | --> to UART (as UART_PCLK)
   *    system PLL  input   --->| |                |   +------------------------+
   *                            | |                +-->| ...      clock divider | 
   *    system PLL output   --->|/                     +------------------------+
   *
   *    Basically, the main clock can be derived from either 
   *
   *    a) the IRC oscillator, 
   *    b) the system PLL  input (e.g., raw        system oscillator), or 
   *    c) the system PLL output (i.e., multiplied system oscillator),
   *
   *    based on MAINCLKSEL, then divided via SYSAHBCLKDIV.
   */

  uint32_t MAINCLKSEL, SYSPLLCLKSEL, SYSPLLCTRL, SYSPLLCTRL_MSEL, SYSPLLCTRL_PSEL, SYSAHBCLKDIV;

  switch( scale_conf.clock_type ) {
    case SCALE_CLOCK_TYPE_INT : {
      MAINCLKSEL   = 0x3;
      SYSPLLCLKSEL = 0x0;
      break;
    }
    case SCALE_CLOCK_TYPE_EXT : {
      MAINCLKSEL   = 0x3;
      SYSPLLCLKSEL = 0x1;
      break;
    }
    case SCALE_CLOCK_TYPE_PIN : {
      MAINCLKSEL   = 0x1;
      SYSPLLCLKSEL = 0x1;
      break;
    }
    default                   : {
      return false;
    }
  }

  for( SYSPLLCTRL_MSEL = 0; SYSPLLCTRL_MSEL <= 31; SYSPLLCTRL_MSEL++ ) {
    for( SYSAHBCLKDIV = 1; SYSAHBCLKDIV <= 255; SYSAHBCLKDIV++ ) {
      uint32_t freq_q = ( scale_conf.clock_freq_source * ( SYSPLLCTRL_MSEL + 1 ) ) / ( SYSAHBCLKDIV );
      uint32_t freq_r = ( scale_conf.clock_freq_source * ( SYSPLLCTRL_MSEL + 1 ) ) % ( SYSAHBCLKDIV );

      if( ( freq_r == 0 ) && ( freq_q == scale_conf.clock_freq_target ) ) {
        goto done_msel;
      }
    }
  }

  return false;

  done_msel:

  for( SYSPLLCTRL_PSEL = 0; SYSPLLCTRL_PSEL <=  3; SYSPLLCTRL_PSEL++ ) {  
     uint32_t FCCO = ( ( SYSPLLCTRL_MSEL + 1 ) * scale_conf.clock_freq_source ) * ( 2 * ( 1 << SYSPLLCTRL_PSEL ) );

     if( ( FCCO >= 156 ) && ( FCCO <= 320 ) ) {
       goto done_psel;
     }
  }

  return false;

  done_psel:

  SYSPLLCTRL = SYSPLLCTRL_MSEL | ( SYSPLLCTRL_PSEL << 5 );

  if     ( ( scale_conf.clock_freq_source >=  1 ) && 
           ( scale_conf.clock_freq_source <= 20 ) ) {
    LPC81X_SYSCON->SYSOSCCTRL      =  ( 0x0 <<  0 ) |        // Table 12: configure  system oscillator => not bypassed
                                      ( 0x0 <<  1 ) ;        // Table 12: configure  system oscillator =>  1-20 MHz
  }
  else if( ( scale_conf.clock_freq_source >= 15 ) && 
           ( scale_conf.clock_freq_source <= 25 ) ) {
    LPC81X_SYSCON->SYSOSCCTRL      =  ( 0x0 <<  0 ) |        // Table 12: configure  system oscillator => not bypassed
                                      ( 0x1 <<  1 ) ;        // Table 12: configure  system oscillator => 15-25 MHz
  }

  if( scale_conf.clock_type == SCALE_CLOCK_TYPE_INT ) {
    LPC81X_SYSCON->PDRUNCFG       |=  ( 0x1 <<  5 ) ;        // Table 43: power-down system oscillator
  }
  else {
    LPC81X_SYSCON->PDRUNCFG       &= ~( 0x1 <<  5 ) ;        // Table 43: power-up   system oscillator
  }
  
  for( int i = 0; i < 200; i++ ) {
    __asm__ __volatile__( "nop" );
  }

    LPC81X_SYSCON->SYSPLLCLKSEL   = SYSPLLCLKSEL;            // Table 16: set system PLL input
  
    LPC81X_SYSCON->SYSPLLCLKUEN   |=  ( 0x1 <<  0 ) ;        // Section 3.5.10: update 
    LPC81X_SYSCON->SYSPLLCLKUEN   &= ~( 0x1 <<  0 ) ;        // Section 3.5.10: toggle
    LPC81X_SYSCON->SYSPLLCLKUEN   |=  ( 0x1 <<  0 ) ;        // Section 3.5.10: update 
  
  while( !( LPC81X_SYSCON->SYSPLLCLKUEN & ( 0x1 << 0 ) ) ) { // Section 3.5.10: wait for update
    /* skip */
  }

    LPC81X_SYSCON->SYSPLLCTRL      = SYSPLLCTRL;             // Table 10: configure system PLL => MSEL | ( PSEL << 5 )
    LPC81X_SYSCON->PDRUNCFG       &= ~( 0x1 <<  7 ) ;        // Table 43: power-up  system PLL
  
  while( !( LPC81X_SYSCON->SYSPLLSTAT   & ( 0x1 << 0 ) ) ) { // Section 3.5.10: wait for update
    /* skip */
  }

    LPC81X_SYSCON->MAINCLKSEL      = MAINCLKSEL;             // Table 18: set main system clock
  
    LPC81X_SYSCON->MAINCLKUEN     |=  ( 0x1 <<  0 ) ;        // Section 3.5.11: update 
    LPC81X_SYSCON->MAINCLKUEN     &= ~( 0x1 <<  0 ) ;        // Section 3.5.11: toggle
    LPC81X_SYSCON->MAINCLKUEN     |=  ( 0x1 <<  0 ) ;        // Section 3.5.11: update 
  
  while( !( LPC81X_SYSCON->MAINCLKUEN   & ( 0x1 << 0 ) ) ) { // Section 3.5.11: wait for update
    /* skip */
  }

    LPC81X_SYSCON->SYSAHBCLKDIV    = SYSAHBCLKDIV;           // Table 20: set AHB clock divider

  if     ( scale_conf.clock_freq_target <= 20 ) {
    LPC81X_FMC->FLASHCFG           =  ( 0x0 <<  0 ) ;        // Table 48: configure flash memory => 1 system clock
  }
  else if( scale_conf.clock_freq_target <= 40 ) {
    LPC81X_FMC->FLASHCFG           =  ( 0x1 <<  0 ) ;        // Table 48: configure flash memory => 2 system clock
  }

  /* Chapter 7 details the (GP)I/O ports.
   *
   * For each board-specific pin, we need to set the direction and also an
   * initial value.
   */
  
    LPC81X_GPIO->DIR0             |=  ( 0x1 <<  6 ) ;        // Table  84: PIO0_6  => configure as SCALE_GPIO_PIN_TRG (output)
    LPC81X_GPIO->DIR0             |=  ( 0x1 <<  7 ) ;        // Table  84: PIO0_7  => configure as SCALE_GPIO_PIN_GPO (output)
    LPC81X_GPIO->DIR0             &= ~( 0x1 << 12 ) ;        // Table  64: PIO0_12 => configure as SCALE_GPIO_PIN_GPI  (input)
  
    LPC81X_GPIO->B0[  6 ]          = 0x0;                    // initialise SCALE_GPIO_PIN_TRG = 0
    LPC81X_GPIO->B0[  7 ]          = 0x0;                    // initialise SCALE_GPIO_PIN_GPO = 0
       
  /* Chapter 15 details the UART.
   * 
   * We need to 
   * 
   * 1. configure the baud rate,
   * 2. configure the frame format, then
   * 3. enable the USART.
   *
   * Section 4.6.18 and 4.6.19 state that the FRG produces
   *
   * U_PCLK = ( main clock freq. / UARTCLKDIV ) / ( 1 + ( UARTFRGMULT / ( UARTFRGDIV + 1 ) ) ) .
   *
   * Section 15.7.1 explains the BRG further divides U_PCLK, and uses 16-fold 
   * "overclocking" for the baud rate, meaning
   *
   * UART baud rate = U_PCLK / ( 16 * ( BRGVAL + 1 ) ) .
   * 
   * However, we can simplify selection of the parameters by pre-dividing the
   * UART clock is *always* 1 MHz, and U_PCLK is *always* 6 MHz, using
   *
   * UARTFRGDIV  = 255
   * UARTFRGMULT =  77
   * BRG         =   4
   *
   * to get the approximation 9609 baud ~= 9600 baud.
   */

  uint32_t UARTCLKDIV = scale_conf.clock_freq_target, UARTFRGDIV = 255, UARTFRGMULT = 77, BRG = 4;

    LPC81X_SYSCON->UARTCLKDIV      = UARTCLKDIV;             // Table  31: set UART                 divider
    LPC81X_SYSCON->UARTFRGDIV      = UARTFRGDIV;             // Table  35: set fractional generator divider 
    LPC81X_SYSCON->UARTFRGMULT     = UARTFRGMULT;            // Table  36: set fractional generator divider 
  
    LPC81X_UART0->BRG              = BRG;                    // Table 181: set baud       generator divider

    LPC81X_UART0->CFG             |=  ( 0x1 <<  2 ) ;        // Table 173: 8 data bits
    LPC81X_UART0->CFG             &= ~( 0x3 <<  4 ) ;        // Table 173: no parity
    LPC81X_UART0->CFG             &= ~( 0x1 <<  6 ) ;        // Table 173: 0 stop bits
    LPC81X_UART0->CFG             |=  ( 0x1 <<  0 ) ;        // Table 173: enable

  while(  scale_uart_rd_avail() ) { // flush, ready to read 
    scale_uart_rd( SCALE_UART_MODE_NONBLOCKING );
  }
  while( !scale_uart_wr_avail() ) { // flush, ready to write
    /* skip */
  }

  /* Chapter 16 details the I2C interface.
   *
   * We need to configure the clock generator so it matches the required I2C
   * clock frequency of 100 kHz (standard mode).  Section 16.3 says
   *
   * SCL freq. = f( I2C_PCLK / CLKDIV )
   *
   * where f sets the duty cycle via MSTSCLLOW and MSTSCLHIGH.
   */

  uint32_t CLKDIV, MSTTIME;

  for( CLKDIV = 0; CLKDIV <= 65535; CLKDIV++ ) {
    for( MSTTIME = 0; MSTTIME <= 7; MSTTIME++ ) {
      if( 100000 == ( ( ( scale_conf.clock_freq_target * 1000000 ) / ( CLKDIV + 1 ) ) / ( 2 * ( MSTTIME + 2 ) ) ) ) {
        MSTTIME = ( MSTTIME << 0 ) |
                  ( MSTTIME << 3 ) ;

        goto done_i2c;
      }
    }
  }

  return false;

  done_i2c:  

    LPC81X_I2C->CLKDIV             = CLKDIV;                 // Table 192: set clock divider  
    LPC81X_I2C->MSTTIME            = MSTTIME;                // Table 195: set -ve, low  duty cycle
   
    LPC81X_I2C->CFG               |=  ( 0x1 <<  0 ) ;        // Table 185:  enable interface


  /* Chapter 14 details the system tick (or systick) timer.
   * 
   * Figure 28 outlines the structure of the systick timer, which we used as
   * a cycle-accurate Time Stamp Counter (TSC) by driving it with the system 
   * clock.  Note that the timer uses a 24-bit counter, so, at 16 MHz for 
   * example, offers at most a
   *
   * 2^24 cycles / ( 16 * 10^6 ) ~= 1 second
   *
   * useful duration.
   */

    LPC81X_SYSTICK->SYST_RVR       =  0x00FFFFFF;             // Table 286: set  reload value to maximum
    LPC81X_SYSTICK->SYST_CVR       =  0x00FFFFFF;             // Table 287: set current value to maximum

  if( conf->tsc ) {
    #if defined( LPC81X_SYSTICK_56BIT )
    LPC81X_SYSTICK->SYST_CSR       =  ( 0x1 <<  0 ) |         // Table 285:  enable systick
                                      ( 0x1 <<  1 ) |         // Table 285:  enable systick interrupt
                                      ( 0x1 <<  2 ) ;         // Table 285:     set systick clock = system clock
    #else
    LPC81X_SYSTICK->SYST_CSR       =  ( 0x1 <<  0 ) |         // Table 285:  enable systick
                                      ( 0x0 <<  1 ) |         // Table 285: disable systick interrupt
                                      ( 0x1 <<  2 ) ;         // Table 285:     set systick clock = system clock
    #endif
    #if defined( LPC81X_SYSTICK_56BIT )
    lpc81x_tsc                     =  0xFFFFFFFF;
    #endif
  }
  else {
    LPC81X_SYSTICK->SYST_CSR       =  ( 0x0 <<  0 ) |         // Table 285: disable systick
                                      ( 0x0 <<  1 ) |         // Table 285: disable systick interrupt
                                      ( 0x1 <<  2 ) ;         // Table 285:     set systick clock = system clock
  }

  lpc81x_irq_enable();

  return  true;
}

void scale_delay_us( int us ) {
  /* If we use an n MHz main system clock, there are n cycles per us; the 
   * documentation at
   * 
   * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0484c/CHDCICDF.html
   * 
   * states (in Table 3.1) that
   * 
   * sub  -> 1 cycle
   * bne  -> 1 cycle if not taken, or 2 cycles otherwise
   *
   * and so 3 cycles per loop (bar the last).  Note that per
   *
   * http://infocenter.arm.com/help/topic/com.arm.doc.dui0662b/BABFFEJF.html
   *
   * sub is equivalent to subs, in that both update flags: there's no need
   * to *specifically* indicate an update is needed.
   */

  int l = ( scale_conf.clock_freq_target * us ) / 3;

  if( l > 0 ) {
    __asm__ __volatile__( "0: sub %0, #1 ; bne 0b ;" : : "r" (l) : "cc" );
  }

  return;
}

void scale_delay_ms( int ms ) {
  for( int i = 0; i < ms; i++ ) {
    scale_delay_us( 1000 );
  }

  return;
}

scale_tsc_t scale_tsc() {
  #if defined( LPC81X_SYSTICK_56BIT )
  return ( ( scale_tsc_t )( lpc81x_tsc ) << 24 ) | ( scale_tsc_t )( LPC81X_SYSTICK->SYST_CVR );
  #else
  return                                           ( scale_tsc_t )( LPC81X_SYSTICK->SYST_CVR );
  #endif
}

bool scale_gpio_rd( scale_gpio_pin_t id         ) {
  switch( id ) {
    case SCALE_GPIO_PIN_TRG : {
      return LPC81X_GPIO->B0[  6 ] & 0x1;
    }
    case SCALE_GPIO_PIN_GPO : {
      return LPC81X_GPIO->B0[  7 ] & 0x1;
    }
    case SCALE_GPIO_PIN_GPI : {
      return LPC81X_GPIO->B0[ 12 ] & 0x1;
    }
  }

  return false;
}

void scale_gpio_wr( scale_gpio_pin_t id, bool x ) {
  switch( id ) {
    case SCALE_GPIO_PIN_TRG : {
      LPC81X_GPIO->B0[  6 ] = x; break;
    }
    case SCALE_GPIO_PIN_GPO : {
      LPC81X_GPIO->B0[  7 ] = x; break;
    }
    case SCALE_GPIO_PIN_GPI : {
      LPC81X_GPIO->B0[ 12 ] = x; break;
    }
  }

  return;
}

bool scale_uart_rd_avail() {
  return LPC81X_UART0->STAT & ( 0x1 << 0 );
}

bool scale_uart_wr_avail() {
  return LPC81X_UART0->STAT & ( 0x1 << 2 );
}

uint8_t scale_uart_rd( scale_uart_mode_t mode            ) {
  while( ( mode == SCALE_UART_MODE_BLOCKING ) && !scale_uart_rd_avail() ) {
    /* skip */
  }
  return ( LPC81X_UART0->RXDATA     );
}

void    scale_uart_wr( scale_uart_mode_t mode, uint8_t x ) {
  while( ( mode == SCALE_UART_MODE_BLOCKING ) && !scale_uart_wr_avail() ) {
    /* skip */
  }
         ( LPC81X_UART0->TXDATA = x );
  return;
}

scale_i2c_status_t scale_i2c_rd( scale_i2c_addr_t addr, uint8_t* x, size_t n ) {
      LPC81X_I2C->MSTDAT = ( addr << 1 ) | SCALE_I2C_MODE_RD;
    
      while( !( ( LPC81X_I2C->STAT >> 0 ) & 0x1 ) );
    
      LPC81X_I2C->MSTCTL = (  0x0 << 0 ) | // clear CONTINUE
                           (  0x1 << 1 ) | // set   START 
                           (  0x0 << 2 ) ; // clear STOP
    
      while( !( ( LPC81X_I2C->STAT >> 0 ) & 0x1 ) );

    if( ( ( LPC81X_I2C->STAT >> 1 ) & 0x7 ) == 0x3 ) {
      return SCALE_I2C_STATUS_FAILURE_NACK_ADDR;
    }
  
  while( n-- != 0 ) {
      *x++ = LPC81X_I2C->MSTDAT;

    if( n > 0 ) {
      while( !( ( LPC81X_I2C->STAT >> 0 ) & 0x1 ) );

      LPC81X_I2C->MSTCTL = (  0x1 << 0 ) | // set   CONTINUE
                           (  0x0 << 1 ) | // clear START 
                           (  0x0 << 2 ) ; // clear STOP
    }

      while( !( ( LPC81X_I2C->STAT >> 0 ) & 0x1 ) );
  }

      LPC81X_I2C->MSTCTL = (  0x0 << 0 ) | // clear continue
                           (  0x0 << 1 ) | // clear start 
                           (  0x1 << 2 ) ; // set   stop
    
      return SCALE_I2C_STATUS_SUCCESS;
}

scale_i2c_status_t scale_i2c_wr( scale_i2c_addr_t addr, uint8_t* x, size_t n ) {
      LPC81X_I2C->MSTDAT = ( addr << 1 ) | SCALE_I2C_MODE_WR;
    
      while( !( ( LPC81X_I2C->STAT >> 0 ) & 0x1 ) );
    
      LPC81X_I2C->MSTCTL = (  0x0 << 0 ) | // clear CONTINUE
                           (  0x1 << 1 ) | // set   START 
                           (  0x0 << 2 ) ; // clear STOP
    
      while( !( ( LPC81X_I2C->STAT >> 0 ) & 0x1 ) );

    if( ( ( LPC81X_I2C->STAT >> 1 ) & 0x7 ) == 0x3 ) {
      return SCALE_I2C_STATUS_FAILURE_NACK_ADDR;
    }

  while( n-- != 0 ) {
      LPC81X_I2C->MSTDAT = *x++;
  
      while( !( ( LPC81X_I2C->STAT >> 0 ) & 0x1 ) );
  
      LPC81X_I2C->MSTCTL = (  0x1 << 0 ) | // set   CONTINUE
                           (  0x0 << 1 ) | // clear START 
                           (  0x0 << 2 ) ; // clear STOP
  
      while( !( ( LPC81X_I2C->STAT >> 0 ) & 0x1 ) );

    if( ( ( LPC81X_I2C->STAT >> 1 ) & 0x7 ) == 0x4 ) {
      return SCALE_I2C_STATUS_FAILURE_NACK_DATA;
    }
  }

      LPC81X_I2C->MSTCTL = (  0x0 << 0 ) | // clear CONTINUE
                           (  0x0 << 1 ) | // clear START 
                           (  0x1 << 2 ) ; // set   STOP
  
      return SCALE_I2C_STATUS_SUCCESS;
}
