/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

#include "example.h"

int main( int argc, char* argv[] ) {
  if( !scale_init( &SCALE_CONF ) ) {
    return -1;
  }

  char x[] = "hello world";

  while( true ) {
    bool t = scale_gpio_rd( SCALE_GPIO_PIN_GPI        );
             scale_gpio_wr( SCALE_GPIO_PIN_GPO, t     );

             scale_gpio_wr( SCALE_GPIO_PIN_TRG, true  );

    scale_delay_ms( 500 );

             scale_gpio_wr( SCALE_GPIO_PIN_TRG, false );

    scale_delay_ms( 500 );

    int n = strlen( x );

    for( int i = 0; i < n; i++ ) {
      scale_uart_wr( SCALE_UART_MODE_BLOCKING, x[ i ] );
    }
  }

  return 0;
}
