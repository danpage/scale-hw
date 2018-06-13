/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

/* Throughout the following, note that all Chapter, Section, Page references 
 * are to the atmega328p data sheet at
 *
 * http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf
 */

#include "scale.h"

scale_conf_t SCALE_CONF = {
  .clock_type        = SCALE_CLOCK_TYPE,
  .clock_freq_source = SCALE_CLOCK_FREQ_SOURCE,
  .clock_freq_target = SCALE_CLOCK_FREQ_TARGET
};

bool scale_init( scale_conf_t* conf ) {
  /* Figure 13-1 provides a high level overview of clock generation
   * 
   *                                                                            +-- clk_IO    --> to GPIO etc.
   * internal oscillator -->|\                                                  |
   *                        | |     +-----------+               +---------------+
   * external oscillator -->| | --> | prescaler | -- clk_SYS -->| clock control |-- clk_CPU   --> to core and RAM
   *                        | |     +-----------+               +---------------+
   * external clock      -->|/                                                  |
   *                                                                            +-- ...       --> ...
   * 
   * where basically everything is selected by using fuses; this means we're 
   * stuck with static selection of various parameters, e.g., CKSEL and SUT.
   * This means we rely on the build system to 
   *
   * 1. configure fuses related to clock generation, and
   * 2. define F_CPU, allowing AVR libc to support more or less everything 
   *    else automatically,
   *
   * then reject any configuration other than the default (which is fixed to
   * match the static parameters).
   */

  if( conf != &SCALE_CONF ) {
    return false;
  }

  memcpy( &scale_conf, conf, sizeof( scale_conf_t ) );

  /* Chapter 18 details the (GP)I/O ports.
   *
   * For each board-specific pin, we need to set the direction and also an
   * initial value.
   */

  DDRB   |=  ( 0x1 << PB0     ); // port B, pin 0 = 1 => SCALE_GPIO_PIN_TRG = output
  DDRD   |=  ( 0x1 << PD7     ); // port D, pin 7 = 1 => SCALE_GPIO_PIN_GPO = output
  DDRD   &= ~( 0x1 << PD2     ); // port D, pin 2 = 0 => SCALE_GPIO_PIN_GPI =  input

  PORTD  |=  ( 0x1 << PD2     ); // initialise SCALE_GPIO_PIN_GPI = pull-up

  PORTB  &= ~( 0x1 << PB0     ); // initialise SCALE_GPIO_PIN_TRG = 0
  PORTD  &= ~( 0x1 << PD7     ); // initialise SCALE_GPIO_PIN_GPO = 0

  /* Chapter 24 details the UART.
   * 
   * We need to 
   * 
   * 1. configure the baud rate,
   * 2. configure the frame format, then
   * 3. enable the USART.
   *
   * The first task is made a easier by AVR libc, which basically implements
   * the formula in Table 24-1 given definitions for F_CPU and BAUD.
   */

  UBRR0L  = UBRRL_VALUE;         // Section 24.12.5
  UBRR0H  = UBRRH_VALUE;         // Section 24.12.6

  #if USE_2X
  UCSR0A |=  ( 0x1 << U2X0    ); // Section 24.12.2: UCSR0A[ 1   ] = 1  => double-speed
  #else
  UCSR0A &= ~( 0x1 << U2X0    ); // Section 24.12.2: UCSR0A[ 1   ] = 0  => single-speed
  #endif

  UCSR0C &= ~( 0x1 << UCPOL0  ); // Section 24.12.4: UCSR0C[ 0   ] =  0 => polarity = disabled
  UCSR0C |=  ( 0x1 << UCSZ00  ); // Section 24.12.4: UCSR0C[ 2:1 ] = 11 => size     = 8
  UCSR0C |=  ( 0x1 << UCSZ01  ); //
  UCSR0C &= ~( 0x1 << USBS0   ); // Section 24.12.4: UCSR0C[ 3   ] =  1 => stop     = 1
  UCSR0C &= ~( 0x1 << UPM00   ); // Section 24.12.4: UCSR0C[ 5:4 ] = 00 => parity   = none 
  UCSR0C &= ~( 0x1 << UPM01   ); //
  UCSR0C &= ~( 0x1 << UMSEL00 ); // Section 24.12.4: UCSR0C[ 7:6 ] = 00 => mode     = asynchronous USART
  UCSR0C &= ~( 0x1 << UMSEL01 ); // 

  UCSR0B |=  ( 0x1 << TXEN0   ); // Section 24.12.3: UCSR0B[ 3   ] =  1 => enable Tx
  UCSR0B |=  ( 0x1 << RXEN0   ); // Section 24.12.3: UCSR0B[ 4   ] =  1 => enable Rx

  /* Chapter 26 details the I2C interface (or TWI in Atmel terminology).
   *
   * We need to configure the clock generator so it matches the required I2C
   * clock frequency of 100 kHz (standard mode).  Section 26.5.2 says
   *
   * SCL freq. = core clock freq. / ( 16 + 2 * TWBR * ( 4^TWPS ) ) .
   */

  uint8_t __TWSR, __TWBR;

  for( __TWSR = 0; __TWSR <= 3; __TWSR++ ) {
    for( __TWBR = 0; __TWBR <= 255; __TWBR++ ) {
      if( UINT32_C( 100000 ) == ( UINT32_C( F_CPU ) / ( 16 + 2 * ( uint32_t )( __TWBR ) * ( 1 << ( 2 * ( uint32_t )( __TWSR ) ) ) ) ) ) {
        goto done_i2c;
      }
    }
  }

  return false;

  done_i2c:

  TWSR = __TWSR;                 // Section 26.9.2: TWSR[ 1:0 ] = TWS => TWI prescaler
  TWBR = __TWBR;                 // Section 26.9.1: TWBR              => TWI bit rate

  return  true;
}

void scale_delay_us( int us ) {
  for( int i = 0; i < us; i++ ) {
    _delay_us(    1 );
  }

  return;
}

void scale_delay_ms( int ms ) {
  for( int i = 0; i < ms; i++ ) {
    _delay_us( 1000 );
  }

  return;
}

bool scale_gpio_rd( scale_gpio_pin_t id         ) {
  switch( id ) {
    case SCALE_GPIO_PIN_TRG : {
      return ( PINB >> PB0 ) & 1;
    }
    case SCALE_GPIO_PIN_GPO : {
      return ( PIND >> PD7 ) & 1;
    }
    case SCALE_GPIO_PIN_GPI : {
      return ( PIND >> PD2 ) & 1;
    }
  }

  return false;
}

void scale_gpio_wr( scale_gpio_pin_t id, bool x ) {
  switch( id ) {
    case SCALE_GPIO_PIN_TRG : {
      if( x ) {
        PORTB |=  ( 1 << PB0 ); 
      }
      else {
        PORTB &= ~( 1 << PB0 ); 
      }
      break;
    }
    case SCALE_GPIO_PIN_GPO : {
      if( x ) {
        PORTD |=  ( 1 << PD7 ); 
      }
      else {
        PORTD &= ~( 1 << PD7 ); 
      }
      break;
    }
    case SCALE_GPIO_PIN_GPI : {
      if( x ) {
        PORTD |=  ( 1 << PD2 ); 
      }
      else {
        PORTD &= ~( 1 << PD2 ); 
      }
      break;
    }
  }

  return;
}

uint8_t scale_uart_rd( void      ) {
  while( !( UCSR0A & ( 1 << RXC0  ) ) );
  return ( UDR0     );
}

void    scale_uart_wr( uint8_t x ) {
  while( !( UCSR0A & ( 1 << UDRE0 ) ) );
         ( UDR0 = x );
  return;
}

scale_i2c_status_t scale_i2c_fsm( scale_i2c_addr_t addr, scale_i2c_mode_t mode, uint8_t* x, size_t n ) {
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x1 << TWSTA ) ;       // set   STA

  while( true ) {
    while( !( TWCR & ( 1 << TWINT ) ) );

    switch( TWSR & 0xF8 ) {
      case TW_NO_INFO      : { // Table 26-7, shared   : no information
          break;
      }
      case TW_BUS_ERROR    : { // Table 26-7, shared   : bus error
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x1 << TWSTO ) |       // set   STO
                 (  0x0 << TWSTA ) ;       // clear STA

          return SCALE_I2C_STATUS_FAILURE;
      }
      case TW_START        : { // Table 26-3, shared   :        start => addr
          TWDR = ( addr << 1     ) | mode; // set   addr
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA

          break;
      }
      case TW_REP_START    : { // Table 26-3, shared   : repeat start => addr
          TWDR = ( addr << 1     ) | mode; // set   addr
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA

          break;
      }
      case TW_MT_SLA_ACK   : { // Table 26-3, transmit : addr         =>  ACK
        if( n > 0 ) {
          TWDR = *x++;                     // set   data
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA

          break;
        }
        else {
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x1 << TWSTO ) |       // set   STO
                 (  0x0 << TWSTA ) ;       // clear STA

          return SCALE_I2C_STATUS_SUCCESS;
        }
      }
      case TW_MT_SLA_NACK  : { // Table 26-3, transmit : addr         => NACK
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x1 << TWSTO ) |       // set   STO
                 (  0x0 << TWSTA ) ;       // clear STA

          return SCALE_I2C_STATUS_FAILURE_NACK_ADDR;
      }
      case TW_MT_DATA_ACK  : { // Table 26-3, transmit : data         =>  ACK
        if( --n == 0 ) {
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x1 << TWSTO ) |       // set   STO
                 (  0x0 << TWSTA ) ;       // clear STA

          return SCALE_I2C_STATUS_SUCCESS;
        }
        else {
          TWDR = *x++;                     // set   data
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA

          break;
        }
      }
      case TW_MT_DATA_NACK : { // Table 26-3, transmit : data         => NACK
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA 
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x1 << TWSTO ) |       // set   STO
                 (  0x0 << TWSTA ) ;       // clear STA

          return SCALE_I2C_STATUS_FAILURE_NACK_DATA;
      }
      case TW_MT_ARB_LOST  : { // Table 26-3, transmit : arbitration error
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA 
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA

          break;
      }
      case TW_MR_SLA_ACK   : { // Table 26-4, receive  : addr         =>  ACK
        if( n > 1 ) {
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x1 << TWEA  ) |       // set   EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA

          break;
        }
        else {
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA

          break;
        }
      }
      case TW_MR_SLA_NACK  : { // Table 26-4, receive  : addr         => NACK
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x1 << TWSTO ) |       // set   STO
                 (  0x0 << TWSTA ) ;       // clear STA

          return SCALE_I2C_STATUS_FAILURE_NACK_ADDR;
      }
      case TW_MR_DATA_ACK  : { // Table 26-4, receive  : data         =>  ACK
        if( --n <= 1 ) {
          *x++ = TWDR;                     // get   data         
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT 
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA          

          break;
        }
        else {
          *x++ = TWDR;                     // get   data         
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x1 << TWEA  ) |       // set   EA
                 (  0x1 << TWINT ) |       // set   INT 
                 (  0x0 << TWSTO ) |       // clear STO
                 (  0x0 << TWSTA ) ;       // clear STA          

          break;
        }
      }
      case TW_MR_DATA_NACK : { // Table 26-4, receive  : data         => NACK
        if( n != 0 ) {
          *x++ = TWDR;                     // get   data 
        }
          TWCR = (  0x1 << TWEN  ) |       // set   EN
                 (  0x0 << TWEA  ) |       // clear EA
                 (  0x1 << TWINT ) |       // set   INT
                 (  0x1 << TWSTO ) |       // set   STO
                 (  0x0 << TWSTA ) ;       // clear STA        

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
