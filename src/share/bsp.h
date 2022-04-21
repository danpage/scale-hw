/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#ifndef __BSP_H
#define __BSP_H

#include <stdbool.h>
#include  <stdint.h>
#include  <string.h>

#define RSVD(x,y,z) uint8_t reserved_##x[ z - y + 1 ]

#define RO volatile const /* read  only */
#define WO volatile       /* write only */
#define RW volatile       /* read/write */
#define WR volatile       /* write/read */

typedef enum {
  SCALE_CLOCK_TYPE_INT   =  0, // internal  clock
  SCALE_CLOCK_TYPE_EXT   =  1, // external  clock, i.e., crystal
  SCALE_CLOCK_TYPE_PIN   =  2  // generated clock, i.e., via frequency generator
} scale_clock_type_t;

typedef enum {
  SCALE_CLOCK_FREQ_1MHZ  =  1,
  SCALE_CLOCK_FREQ_2MHZ  =  2,
  SCALE_CLOCK_FREQ_4MHZ  =  4,
  SCALE_CLOCK_FREQ_8MHZ  =  8,
  SCALE_CLOCK_FREQ_12MHZ = 12,
  SCALE_CLOCK_FREQ_16MHZ = 16,
  SCALE_CLOCK_FREQ_24MHZ = 24,
  SCALE_CLOCK_FREQ_32MHZ = 32
} scale_clock_freq_t;

typedef uint64_t scale_tsc_t;

typedef enum {
  SCALE_GPIO_PIN_TRG,
  SCALE_GPIO_PIN_GPO,
  SCALE_GPIO_PIN_GPI
} scale_gpio_pin_t;

typedef enum {
  SCALE_UART_MODE_BLOCKING,
  SCALE_UART_MODE_NONBLOCKING,
} scale_uart_mode_t;

typedef uint8_t scale_i2c_addr_t;

typedef enum {
  SCALE_I2C_MODE_WR = 0x00,
  SCALE_I2C_MODE_RD = 0x01
} scale_i2c_mode_t;

typedef enum { 
  SCALE_I2C_STATUS_SUCCESS           = 0x00,
  SCALE_I2C_STATUS_FAILURE           = 0x01,
  SCALE_I2C_STATUS_FAILURE_NACK_ADDR = 0x02,
  SCALE_I2C_STATUS_FAILURE_NACK_DATA = 0x03
} scale_i2c_status_t;

typedef struct {
  scale_clock_type_t clock_type;
  scale_clock_freq_t clock_freq_source;
  scale_clock_freq_t clock_freq_target;

  bool               tsc;
} scale_conf_t;

// default configuration
extern scale_conf_t SCALE_CONF;
// active  configuration
extern scale_conf_t scale_conf;

// initialise (e.g., clocks, GPIO, UART, I2C, etc.)
extern bool               scale_init( scale_conf_t* conf );

// delay (i.e., wait) for some number of microseconds
extern void               scale_delay_us( int us );
// delay (i.e., wait) for some number of milliseconds
extern void               scale_delay_ms( int ms );

// read Time Stamp Counter (TSC), or "cycle count"
extern scale_tsc_t        scale_tsc();

// read  (or sample) GPIO pin 
extern bool               scale_gpio_rd( scale_gpio_pin_t id         );
// write (or  drive) GPIO pin
extern void               scale_gpio_wr( scale_gpio_pin_t id, bool x );

// check if UART is available for read  (i.e., would doing so block or not)
extern bool               scale_uart_rd_avail();
// check if UART is available for write (i.e., would doing so block or not)
extern bool               scale_uart_wr_avail();
// read  (or  receive) 1-byte from UART
extern uint8_t            scale_uart_rd( scale_uart_mode_t mode            );
// write (or transmit) 1-byte to   UART
extern void               scale_uart_wr( scale_uart_mode_t mode, uint8_t x );

// read  (or  receive) buffer from 7-bit I2C address
extern scale_i2c_status_t scale_i2c_rd( scale_i2c_addr_t addr, uint8_t* x, size_t n );
// write (or transmit) buffer to   7-bit I2C address
extern scale_i2c_status_t scale_i2c_wr( scale_i2c_addr_t addr, uint8_t* x, size_t n );

#endif
