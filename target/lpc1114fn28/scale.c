/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

/* Throughout the following, note that all Chapter, Section, Page references 
 * are to the lpc111x user manual at
 *
 * http://www.nxp.com/docs/en/user-guide/UM10398.pdf
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

  /* The majority of I/O pins have a configurable role; the configuration is
   * outlined in Chapter 8, which is used to match all board-specific pins;
   * it's important this is done next, since that configuration potentially
   * dictates whether or not the clock will subsequently work.
   *
   * To ensure some peripherals function at all, we need to explicitly enable
   * the clock supplied to them; it's important this is done in a pre-defined
   * order, since the update of any control register for said peripheral will
   * would fail otherwise.
   */

    LPC111X_SYSCON->SYSAHBCLKCTRL |=  ( 0x1 << 16 ) ;         // Table  21:  enable IOCON clock
  
    LPC111X_IOCON->PIO0_8          =  ( 0x0 <<  0 ) |         // Table 128: PIO0_8 => configure as GPIO
                                      ( 0x0 <<  3 ) ;         //          : PIO0_8 => disable pull-up/down
    LPC111X_IOCON->PIO0_9          =  ( 0x0 <<  0 ) |         // Table 129: PIO0_9 => configure as GPIO
                                      ( 0x0 <<  3 ) ;         //          : PIO0_9 => disable pull-up/down
    LPC111X_IOCON->PIO0_1          =  ( 0x0 <<  0 ) |         // Table 109: PIO0_1 => configure as GPIO
                                      ( 0x2 <<  3 ) ;         //          : PIO0_1 =>  enable pull-up
  
    LPC111X_IOCON->PIO1_6          =  ( 0x1 <<  0 ) |         // Table 145: PIO1_6 => configure as UART RxD
                                      ( 0x0 <<  3 ) ;         //          : PIO1_6 => disable pull-up/down
    LPC111X_IOCON->PIO1_7          =  ( 0x1 <<  0 ) |         // Table 146: PIO1_7 => configure as UART TxD
                                      ( 0x0 <<  3 ) ;         //          : PIO1_7 => disable pull-up/down

    LPC111X_IOCON->PIO0_4          =  ( 0x1 <<  0 ) |         // Table 116: PIO0_4 => configure as I2C (SCL)
                                      ( 0x0 <<  8 ) ;         //          : PIO0_4 => configure in standard mode (@ 100 kHz)
    LPC111X_IOCON->PIO0_5          =  ( 0x1 <<  0 ) |         // Table 117: PIO0_5 => configure as I2C (SDA)
                                      ( 0x0 <<  8 ) ;         //          : PIO0_5 => configure in standard mode (@ 100 kHz)

    LPC111X_SYSCON->SYSAHBCLKCTRL |=  ( 0x1 <<  5 ) ;         // Table  21:  enable I2C   clock
    LPC111X_SYSCON->SYSAHBCLKCTRL |=  ( 0x1 <<  6 ) ;         // Table  21:  enable GPIO  clock
    LPC111X_SYSCON->SYSAHBCLKCTRL |=  ( 0x1 << 12 ) ;         // Table  21:  enable UART  clock

    LPC111X_SYSCON->PRESETCTRL    &= ~( 0x1 <<  1 ) ;         // Table  19:  enable reset I2C
    LPC111X_SYSCON->PRESETCTRL    |=  ( 0x1 <<  1 ) ;         // Table  19: disable reset I2C

  /* Section 3.4 gives a complete overview of clock generation; it highlights
   * the existence of three oscillators, of which two are pertinent:
   *
   * a) the system oscillator is used to stimulate an external crystal, or to
   *    accept a generated clock signal, and
   * b) the Internal RC oscillator (IRC) is used as an alternative if/when no
   *    such crystal exists: this is the default on reset.
   *
   * As such, there are two main configuration challenges:
   *
   * 1. Section 3.11 outlines the system PLL, which generates a clock signal
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
   * 2. Figure 8 illustrates how clock generation operates as a whole, with
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
    LPC111X_SYSCON->SYSOSCCTRL     =  ( 0x0 <<  0 ) |         // Table 12: configure  system oscillator => not bypassed
                                      ( 0x0 <<  1 ) ;         // Table 12: configure  system oscillator =>  1-20 MHz
  }
  else if( ( scale_conf.clock_freq_source >= 15 ) && 
           ( scale_conf.clock_freq_source <= 25 ) ) {
    LPC111X_SYSCON->SYSOSCCTRL     =  ( 0x0 <<  0 ) |         // Table 12: configure  system oscillator => not bypassed
                                      ( 0x1 <<  1 ) ;         // Table 12: configure  system oscillator => 15-25 MHz
  }

  if( scale_conf.clock_type == SCALE_CLOCK_TYPE_INT ) {
    LPC111X_SYSCON->PDRUNCFG      |=  ( 0x1 <<  5 ) ;         // Table 43: power-down system oscillator
  }
  else {
    LPC111X_SYSCON->PDRUNCFG      &= ~( 0x1 <<  5 ) ;         // Table 43: power-up   system oscillator
  }
  
  for( int i = 0; i < 500; i++ ) {                         
    __asm__ __volatile__( "nop" );
  }

    LPC111X_SYSCON->SYSPLLCLKSEL   = SYSPLLCLKSEL;            // Table 16: set system PLL input
  
    LPC111X_SYSCON->SYSPLLCLKUEN  |=  ( 0x1 <<  0 ) ;         // Section 3.5.10: update 
    LPC111X_SYSCON->SYSPLLCLKUEN  &= ~( 0x1 <<  0 ) ;         // Section 3.5.10: toggle
    LPC111X_SYSCON->SYSPLLCLKUEN  |=  ( 0x1 <<  0 ) ;         // Section 3.5.10: update 
  
  while( !( LPC111X_SYSCON->SYSPLLCLKUEN & ( 0x1 << 0 ) ) ) { // Section 3.5.10: wait for update
    /* skip */
  }

    LPC111X_SYSCON->SYSPLLCTRL     = SYSPLLCTRL;              // Table 10: configure system PLL => MSEL | ( PSEL << 5 )
    LPC111X_SYSCON->PDRUNCFG      &= ~( 0x1 <<  7 ) ;         // Table 43: power-up  system PLL
  
  while( !( LPC111X_SYSCON->SYSPLLSTAT   & ( 0x1 << 0 ) ) ) { // Section 3.5.10: wait for update
    /* skip */
  }

    LPC111X_SYSCON->MAINCLKSEL     = MAINCLKSEL;              // Table 18: set main system clock
  
    LPC111X_SYSCON->MAINCLKUEN    |=  ( 0x1 <<  0 ) ;         // Section 3.5.11: update 
    LPC111X_SYSCON->MAINCLKUEN    &= ~( 0x1 <<  0 ) ;         // Section 3.5.11: toggle
    LPC111X_SYSCON->MAINCLKUEN    |=  ( 0x1 <<  0 ) ;         // Section 3.5.11: update 
  
  while( !( LPC111X_SYSCON->MAINCLKUEN   & ( 0x1 << 0 ) ) ) { // Section 3.5.11: wait for update
    /* skip */
  }

    LPC111X_SYSCON->SYSAHBCLKDIV   = SYSAHBCLKDIV;            // Table 20: set AHB clock divider

  if     ( scale_conf.clock_freq_target <= 20 ) {
    LPC111X_FMC->FLASHCFG          =  ( 0x0 <<  0 ) ;         // Table 48: configure flash memory => upto 20 MHz main system clock
  }
  else if( scale_conf.clock_freq_target <= 40 ) {
    LPC111X_FMC->FLASHCFG          =  ( 0x1 <<  0 ) ;         // Table 48: configure flash memory => upto 40 MHz main system clock
  }
  else if( scale_conf.clock_freq_target <= 50 ) {
    LPC111X_FMC->FLASHCFG          =  ( 0x2 <<  0 ) ;         // Table 48: configure flash memory => upto 50 MHz main system clock
  }

  /* Chapter 12 details the (GP)I/O ports.
   *
   * For each board-specific pin, we need to set the direction and also an
   * initial value.
   */

    LPC111X_GPIO0->GPIOnDIR       |=  ( 0x1 <<  8 ) ;         // Table 175: PIO0_8 => configure as SCALE_GPIO_PIN_TRG (output)
    LPC111X_GPIO0->GPIOnDIR       |=  ( 0x1 <<  9 ) ;         // Table 175: PIO0_9 => configure as SCALE_GPIO_PIN_GPO (output)
    LPC111X_GPIO0->GPIOnDIR       &= ~( 0x1 <<  1 ) ;         // Table 175: PIO0_1 => configure as SCALE_GPIO_PIN_GPI  (input)

    LPC111X_GPIO0->GPIOnDATA      &= ~( 0x1 <<  8 ) ;         // initialise SCALE_GPIO_PIN_TRG = 0
    LPC111X_GPIO0->GPIOnDATA      &= ~( 0x1 <<  9 ) ;         // initialise SCALE_GPIO_PIN_GPO = 0

  /* Chapter 13 details the UART.
   * 
   * We need to 
   * 
   * 1. configure the baud rate,
   * 2. configure the frame format, then
   * 3. enable the USART.
   *
   * Section 13.5.15 expands upon the first task, stating that
   *
   * UART baud rate = UART_PCLK / ( 16 * ( 256 * U0DLM + U0DLL ) * ( 1 + ( DivAddVal / MulVal ) ) ) .
   *
   * However, we can simplify selection of the parameters by pre-dividing the
   * UART clock st. UART_PCLK is *always* 1 MHz: per Figure 35 and Table 201, 
   * we then fix
   *
   * DivAddVal = 5
   *    MulVal = 8
   *     U0DLM = 0
   *     U0DLL = 4
   */
 
    LPC111X_SYSCON->UARTCLKDIV     = scale_conf.clock_freq_target;
  
    LPC111X_UART->U0LCR           |=  ( 0x1 <<  7 ) ;         // Table 193:  enable divisor latch access
    LPC111X_UART->U0FDR            =  ( 0x5 <<  0 ) ;         // Table 200: set DivAddVal
    LPC111X_UART->U0FDR           |=  ( 0x8 <<  4 ) ;         // Table 200: set    MulVal
    LPC111X_UART->U0DLM            =  ( 0x0 <<  0 ) ;         // Table 187: set divisor latch MSBs
    LPC111X_UART->U0DLL            =  ( 0x4 <<  0 ) ;         // Table 187: set divisor latch LSBs
    LPC111X_UART->U0LCR           &= ~( 0x1 <<  7 ) ;         // Table 193: disable divisor latch access 
  
    LPC111X_UART->U0LCR           |=  ( 0x3 <<  0 ) ;         // Table 193: 8 data bits
    LPC111X_UART->U0LCR           &= ~( 0x1 <<  3 ) ;         // Table 193: no parity
    LPC111X_UART->U0LCR           &= ~( 0x1 <<  2 ) ;         // Table 193: 1 stop bits

  /* Chapter 15 details the I2C interface.
   *
   * We need to configure the clock generator so it matches the required I2C
   * clock frequency of 100 kHz (standard mode).  Section 15.7.5.1 says
   *
   * SCL freq. = I2C_PCLK (= core clock freq.) / ( SCLH + SCLL ) ,
   *
   * st. SCLH, SCLL > 4; per Table 226, for a n MHz core clock freq. we set
   * SCLH = SCLL = ( 10 * n ) / 2.
   */

  uint32_t SCL = ( 10 * scale_conf.clock_freq_target ) / 2;
  
    LPC111X_I2C->SCLL              = SCL;                     // Table 225: set -ve, low  duty cycle
    LPC111X_I2C->SCLH              = SCL;                     // Table 224: set +ve, high duty cycle
  
    LPC111X_I2C->CONSET            =  ( 0x1 <<  6 ) ;         // Table 220:  enable interface

  /* Chapter 24 details the system tick (or systick) timer.
   * 
   * Figure 90 outlines the structure of the systick timer, which we used as
   * a cycle-accurate Time Stamp Counter (TSC) by driving it with the system 
   * clock.  Note that the timer uses a 24-bit counter, so, at 16 MHz for 
   * example, offers at most a
   *
   * 2^24 cycles / ( 16 * 10^6 ) ~= 1 second
   *
   * useful duration.
   */

    LPC111X_SYSTICK->LOAD          =  0x00FFFFFF;             // Table 286: set  reload value to maximum
    LPC111X_SYSTICK->VAL           =  0x00FFFFFF;             // Table 287: set current value to maximum

  if( conf->tsc ) {
    #if defined( LPC111X_SYSTICK_56BIT )
    LPC111X_SYSTICK->CTRL          =  ( 0x1 <<  0 ) |         // Table 285:  enable systick
                                      ( 0x1 <<  1 ) |         // Table 285:  enable systick interrupt
                                      ( 0x1 <<  2 ) ;         // Table 285:     set systick clock = system clock
    #else
    LPC111X_SYSTICK->CTRL          =  ( 0x1 <<  0 ) |         // Table 285:  enable systick
                                      ( 0x0 <<  1 ) |         // Table 285: disable systick interrupt
                                      ( 0x1 <<  2 ) ;         // Table 285:     set systick clock = system clock
    #endif
    #if defined( LPC111X_SYSTICK_56BIT )
    lpc111x_tsc                    =  0xFFFFFFFF;
    #endif
  }
  else {
    LPC111X_SYSTICK->CTRL          =  ( 0x0 <<  0 ) |         // Table 285: disable systick
                                      ( 0x0 <<  1 ) |         // Table 285: disable systick interrupt
                                      ( 0x1 <<  2 ) ;         // Table 285:     set systick clock = system clock
  }

  lpc111x_irq_enable();

  return true;
}

void scale_delay_us( int us ) {
  /* If we use an n MHz main system clock, there are n cycles per us; the 
   * documentation at
   * 
   * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0432c/CHDCICDF.html
   * 
   * states (in Table 3.1) that
   * 
   * sub  -> 1 cycle
   * bne  -> 1 cycle if not taken, or 3 cycles otherwise
   *
   * and so 4 cycles per loop (bar the last).  Note that per
   *
   * http://infocenter.arm.com/help/topic/com.arm.doc.dui0497a/BABFFEJF.html
   *
   * sub is equivalent to subs, in that both update flags: there's no need
   * to *specifically* indicate an update is needed.
   */

  int l = ( scale_conf.clock_freq_target * us ) / 4;

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
  #if defined( LPC111X_SYSTICK_56BIT )
  return ( ( scale_tsc_t )( lpc111x_tsc ) << 24 ) | ( scale_tsc_t )( LPC111X_SYSTICK->VAL );
  #else
  return                                            ( scale_tsc_t )( LPC111X_SYSTICK->VAL );
  #endif
}

bool scale_gpio_rd( scale_gpio_pin_t id         ) {
  switch( id ) {
    case SCALE_GPIO_PIN_TRG : {
      return ( LPC111X_GPIO0->GPIOnDATA >> 8 ) & 0x1;
    }
    case SCALE_GPIO_PIN_GPO : {
      return ( LPC111X_GPIO0->GPIOnDATA >> 9 ) & 0x1;
    }
    case SCALE_GPIO_PIN_GPI : {
      return ( LPC111X_GPIO0->GPIOnDATA >> 1 ) & 0x1;
    }
  }

  return false;
}

void scale_gpio_wr( scale_gpio_pin_t id, bool x ) {
  switch( id ) {
    case SCALE_GPIO_PIN_TRG : {
      if( x ) {
        LPC111X_GPIO0->GPIOnDATA |=  ( 0x1 << 8 );
      }
      else {
        LPC111X_GPIO0->GPIOnDATA &= ~( 0x1 << 8 );
      }
      break;
    }
    case SCALE_GPIO_PIN_GPO : {
      if( x ) {
        LPC111X_GPIO0->GPIOnDATA |=  ( 0x1 << 9 );
      }
      else {
        LPC111X_GPIO0->GPIOnDATA &= ~( 0x1 << 9 );
      }
      break;
    }
    case SCALE_GPIO_PIN_GPI : {
      if( x ) {
        LPC111X_GPIO0->GPIOnDATA |=  ( 0x1 << 1 );
      }
      else {
        LPC111X_GPIO0->GPIOnDATA &= ~( 0x1 << 1 );
      }
      break;
    }
  }

  return;
}

bool scale_uart_rd_avail() {
  return LPC111X_UART->U0LSR & ( 0x1 << 0 );
}

bool scale_uart_wr_avail() {
  return LPC111X_UART->U0LSR & ( 0x1 << 5 );
}

uint8_t scale_uart_rd( scale_uart_mode_t mode            ) {
  while( ( mode == SCALE_UART_MODE_BLOCKING ) && !scale_uart_rd_avail() ) {
    /* skip */
  }
  return ( LPC111X_UART->U0RBR     );
}

void    scale_uart_wr( scale_uart_mode_t mode, uint8_t x ) {
  while( ( mode == SCALE_UART_MODE_BLOCKING ) && !scale_uart_wr_avail() ) {
    /* skip */
  }
         ( LPC111X_UART->U0THR = x );
  return;
}

scale_i2c_status_t scale_i2c_fsm( scale_i2c_addr_t addr, scale_i2c_mode_t mode, uint8_t* x, size_t n ) {
          LPC111X_I2C->CONSET = (  0x1 << 5 ) ;       // set   STA

  while( true ) {
    switch( LPC111X_I2C->STAT & 0xF8 ) {
      case 0xF8 : { // Table 242, shared   : no information
          break;
      }
      case 0x00 : { // Table 242, shared   : bus error
          LPC111X_I2C->CONSET = (  0x1 << 2 ) |       // set   AA 
                                (  0x1 << 4 ) ;       // set   STO
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 5 ) ;       // clear STA

          return SCALE_I2C_STATUS_FAILURE;
      }
      case 0x08 : { // Section 15.11.3, shared   :        start => addr
          LPC111X_I2C->DAT    = ( addr << 1 ) | mode; // set   addr
          LPC111X_I2C->CONSET = (  0x1 << 2 ) ;       // set   AA 
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 4 ) ;       // clear STO

          break;
      }
      case 0x10 : { // Section 15.11.3, shared   : repeat start => addr
          LPC111X_I2C->DAT    = ( addr << 1 ) | mode; // set   addr
          LPC111X_I2C->CONSET = (  0x1 << 2 ) ;       // set   AA 
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 4 ) ;       // clear STO

          break;
      }
      case 0x18 : { // Section 15.11.3, transmit : addr         =>  ACK
        if( n > 0 ) {
          LPC111X_I2C->DAT    = *x++;                 // set   data
          LPC111X_I2C->CONSET = (  0x1 << 2 ) ;       // set   AA 
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 4 ) |       // clear STO
                                (  0x1 << 5 ) ;       // clear STA

          break;
        }
        else {
          LPC111X_I2C->CONSET = (  0x1 << 2 ) |       // set   AA 
                                (  0x1 << 4 ) ;       // set   STO
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI 
                                (  0x1 << 5 ) ;       // clear STA

          return SCALE_I2C_STATUS_SUCCESS;
        }
      }
      case 0x20 : { // Section 15.11.3, transmit : addr         => NACK
          LPC111X_I2C->CONSET = (  0x1 << 2 ) |       // set   AA 
                                (  0x1 << 4 ) ;       // set   STO
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 5 ) ;       // clear STA

          return SCALE_I2C_STATUS_FAILURE_NACK_ADDR;
      }
      case 0x28 : { // Section 15.11.3, transmit : data         =>  ACK
        if( --n == 0 ) {
          LPC111X_I2C->CONSET = (  0x1 << 2 ) |       // set   AA 
                                (  0x1 << 4 ) ;       // set   STO
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI 
                                (  0x1 << 5 ) ;       // clear STA

          return SCALE_I2C_STATUS_SUCCESS;
        }
        else {
          LPC111X_I2C->DAT    = *x++;                 // set   data
          LPC111X_I2C->CONSET = (  0x1 << 2 ) ;       // set   AA 
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI 
                                (  0x1 << 4 ) |       // clear STO
                                (  0x1 << 5 ) ;       // clear STA          

          break;
        }
      }
      case 0x30 : { // Section 15.11.3, transmit : data         => NACK
          LPC111X_I2C->CONSET = (  0x1 << 2 ) |       // set   AA 
                                (  0x1 << 4 ) ;       // set   STO
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 5 ) ;       // clear STA
   
          return SCALE_I2C_STATUS_FAILURE_NACK_DATA;
      }
      case 0x38 : { // Section 15.11.3, transmit : arbitration error
          LPC111X_I2C->CONSET = (  0x1 << 2 ) |       // set   AA 
                                (  0x1 << 5 ) ;       // set   STA
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 4 ) ;       // clear STO

          break;
      }
      case 0x40 : { // Section 15.11.3, receive  : addr         =>  ACK
        if( n > 1 ) {
          LPC111X_I2C->CONSET = (  0x1 << 2 ) ;       // set   AA 
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 4 ) |       // clear STO
                                (  0x1 << 5 ) ;       // clear STA

          break;
        }
        else {
          LPC111X_I2C->CONCLR = (  0x1 << 2 ) |       // clear AA 
                                (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 4 ) |       // clear STO
                                (  0x1 << 5 ) ;       // clear STA

          break;
        }
      }
      case 0x48 : { // Section 15.11.3, receive  : addr         => NACK
          LPC111X_I2C->CONSET = (  0x1 << 2 ) |       // set   AA 
                                (  0x1 << 4 ) ;       // set   STO
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 5 ) ;       // clear STA

          return SCALE_I2C_STATUS_FAILURE_NACK_ADDR;
      }
      case 0x50 : { // Section 15.11.3, receive  : data         =>  ACK
        if( --n <= 1 ) {
          *x++        = LPC111X_I2C->DAT;             // get   data         
          LPC111X_I2C->CONCLR = (  0x1 << 2 ) |       // clear AA
                                (  0x1 << 3 ) |       // clear SI 
                                (  0x1 << 4 ) |       // clear STO
                                (  0x1 << 5 ) ;       // clear STA

          break;
        }
        else {
          *x++        = LPC111X_I2C->DAT;             // get   data         
          LPC111X_I2C->CONSET = (  0x1 << 2 ) ;       // set   AA 
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI 
                                (  0x1 << 4 ) |       // clear STO
                                (  0x1 << 5 ) ;       // clear STA          

          break;
        }
      }
      case 0x58 : { // Section 15.11.3, receive  : data         => NACK
        if( n != 0 ) {
          *x++        = LPC111X_I2C->DAT;             // get   data 
        }
          LPC111X_I2C->CONSET = (  0x1 << 2 ) |       // set   AA 
                                (  0x1 << 4 ) ;       // set   STO
          LPC111X_I2C->CONCLR = (  0x1 << 3 ) |       // clear SI
                                (  0x1 << 5 ) ;       // clear STA        

          return SCALE_I2C_STATUS_SUCCESS;
      }
    }
  }

  return SCALE_I2C_STATUS_SUCCESS;
}

scale_i2c_status_t scale_i2c_rd( scale_i2c_addr_t addr, uint8_t* x, size_t n ) {
  return scale_i2c_fsm( addr, SCALE_I2C_MODE_RD, x, n );
}

scale_i2c_status_t scale_i2c_wr( scale_i2c_addr_t addr, uint8_t* x, size_t n ) {
  return scale_i2c_fsm( addr, SCALE_I2C_MODE_WR, x, n );
}
