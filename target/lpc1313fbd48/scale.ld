/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

MEMORY {
  rom (rx)  : ORIGIN = 0x00000000, LENGTH = 32K
  ram (rwx) : ORIGIN = 0x10000000, LENGTH =  8K
}

INCLUDE lpc13xx.ld
