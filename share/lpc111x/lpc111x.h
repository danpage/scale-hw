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

#ifndef __LPC111X_H
#define __LPC111X_H

#include "bsp.h"

// Chapter  3, Section  3.5, Table   7: system configuration

typedef struct {
          RW uint32_t SYSMEMREMAP;       // 0x0000          : system memory remap
          RW uint32_t PRESETCTRL;        // 0x0004          : peripheral reset control
          RW uint32_t SYSPLLCTRL;        // 0x0008          : system PLL control
          RO uint32_t SYSPLLSTAT;        // 0x000C          : system PLL status
          RO RSVD(  0, 0x0010, 0x001C ); // 0x0010...0x001C : reserved
          RW uint32_t SYSOSCCTRL;        // 0x0020          : system oscillator control
          RW uint32_t WDTOSCCTRL;        // 0x0024          : system watchdog   control
          RW uint32_t IRCCTRL;           // 0x0028          : IRC               control
          RO RSVD(  1, 0x002C, 0x002C ); // 0x002C          : reserved
          RW uint32_t SYSRSTSTAT;        // 0x0030          : system reset status register
          RO RSVD(  2, 0x0034, 0x003C ); // 0x0034...0x003C : reserved
          RW uint32_t SYSPLLCLKSEL;      // 0x0040          : system PLL clock source select
          RW uint32_t SYSPLLCLKUEN;      // 0x0044          : system PLL clock source update enable
          RO RSVD(  3, 0x0048, 0x006C ); // 0x0048...0x006C : reserved
          RW uint32_t MAINCLKSEL;        // 0x0070          : main       clock source select
          RW uint32_t MAINCLKUEN;        // 0x0074          : main       clock source update enable
          RW uint32_t SYSAHBCLKDIV;      // 0x0078          : system     clock source divider
          RO RSVD(  4, 0x007C, 0x007C ); // 0x007C          : reserved
          RW uint32_t SYSAHBCLKCTRL;     // 0x0080          : system     clock source control
          RO RSVD(  5, 0x0084, 0x0090 ); // 0x0084...0x0090 : reserved
          RW uint32_t SSP0CLKDIV;        // 0x0094          : SPI0       clock source divider
          RW uint32_t UARTCLKDIV;        // 0x0098          : UART       clock source divider
          RW uint32_t SSP1CLKDIV;        // 0x009C          : SPI1       clock source divider
          RO RSVD(  6, 0x00A0, 0x00CC ); // 0x00A0...0x00CC : reserved
          RW uint32_t WDTCLKSEL;         // 0x00D0          : watchdog   clock source select
          RW uint32_t WDTCLKUEN;         // 0x00D4          : watchdog   clock source update enable 
          RW uint32_t WDTCLKDIV;         // 0x00D8          : watchdog   clock source divider
          RO RSVD(  7, 0x00DC, 0x00DC ); // 0x00DC          : reserved
          RW uint32_t CLKOUTSEL;         // 0x00E0          : CLKOUT     clock source select
          RW uint32_t CLKOUTUEN;         // 0x00E4          : CLKOUT     clock source update enable
          RW uint32_t CLKOUTDIV;         // 0x00E8          : CLKOUT     clock source divider
          RO RSVD(  8, 0x00EC, 0x00FC ); // 0x00EC...0x00FC : reserved
          RO uint32_t PIOPORCAP0;        // 0x0100          : POR captured PIO status 0
          RO uint32_t PIOPORCAP1;        // 0x0104          : POR captured PIO status 1
          RO RSVD(  9, 0x0108, 0x014C ); // 0x0108...0x014C : reserved
          RW uint32_t BODCTRL;           // 0x0150          : brown-out detection
          RW uint32_t SYSTCKCAL;         // 0x0154          : system tick counter calibration
          RW RSVD( 10, 0x0158, 0x016C ); // 0x0158...0x016C : reserved
          RW uint32_t IRQLATENCY;        // 0x0170          : IRQ delay
          RW uint32_t NMISRC;            // 0x0174          : NMI source control
          RW RSVD( 11, 0x0178, 0x01FC ); // 0x0178...0x01FC : reserved
          RW uint32_t STARTAPRP0;        // 0x0200          : start logic edge   register 0
          RW uint32_t STARTERP0;         // 0x0204          : start logic enable register 0
          WO uint32_t STARTRSRP0CLR;     // 0x0208          : start logic reset  register 0
          RO uint32_t STARTSRP0;         // 0x020C          : start logic status register 0
          RO RSVD( 12, 0x0210, 0x022C ); // 0x0210...0x022C : reserved
          RW uint32_t PDSLEEPCFG;        // 0x0230          : power-down states for           deep-sleep mode
          RW uint32_t PDAWAKECFG;        // 0x0234          : power-down states for wake from deep-sleep mode
          RW uint32_t PDRUNCFG;          // 0x0238          : power configuration register
          RO RSVD( 14, 0x023C, 0x03F0 ); // 0x023C...0x03F0 : reserved
          RO uint32_t DEVICE_ID;         // 0x03F4          : device ID
} lpc111x_syscon_dev_t;

// Chapter  7, Section  7.4, Table  56: I/O configuration

typedef struct { 
          RW uint32_t PIO2_6;            // 0x0000          : I/O configuration for pin PIO2_6
          RO RSVD(  0, 0x0004, 0x0004 ); // 0x0004          : reserved
          RW uint32_t PIO2_0;            // 0x0008          : I/O configuration for pin PIO2_0
          RW uint32_t PIO0_0;            // 0x000C          : I/O configuration for pin PIO0_0
          RW uint32_t PIO0_1;            // 0x0010          : I/O configuration for pin PIO0_1
          RW uint32_t PIO1_8;            // 0x0014          : I/O configuration for pin PIO1_8
          RO RSVD(  1, 0x0018, 0x0018 ); // 0x0018          : reserved
          RW uint32_t PIO0_2;            // 0x001C          : I/O configuration for pin PIO0_2
          RW uint32_t PIO2_7;            // 0x0020          : I/O configuration for pin PIO2_7
          RW uint32_t PIO2_8;            // 0x0024          : I/O configuration for pin PIO2_8
          RW uint32_t PIO2_1;            // 0x0028          : I/O configuration for pin PIO2_1
          RW uint32_t PIO0_3;            // 0x002C          : I/O configuration for pin PIO0_3
          RW uint32_t PIO0_4;            // 0x0030          : I/O configuration for pin PIO0_4
          RW uint32_t PIO0_5;            // 0x0034          : I/O configuration for pin PIO0_5
          RW uint32_t PIO1_9;            // 0x0038          : I/O configuration for pin PIO1_9
          RW uint32_t PIO3_4;            // 0x003C          : I/O configuration for pin PIO3_4
          RW uint32_t PIO2_4;            // 0x0040          : I/O configuration for pin PIO2_4
          RW uint32_t PIO2_5;            // 0x0044          : I/O configuration for pin PIO2_5
          RW uint32_t PIO3_5;            // 0x0048          : I/O configuration for pin PIO3_5
          RW uint32_t PIO0_6;            // 0x004C          : I/O configuration for pin PIO0_6
          RW uint32_t PIO0_7;            // 0x0050          : I/O configuration for pin PIO0_7
          RW uint32_t PIO2_9;            // 0x0054          : I/O configuration for pin PIO2_9
          RW uint32_t PIO2_10;           // 0x0058          : I/O configuration for pin PIO2_10
          RW uint32_t PIO2_2;            // 0x005C          : I/O configuration for pin PIO2_2
          RW uint32_t PIO0_8;            // 0x0060          : I/O configuration for pin PIO0_8
          RW uint32_t PIO0_9;            // 0x0064          : I/O configuration for pin PIO0_9
          RW uint32_t PIO0_10;           // 0x0068          : I/O configuration for pin PIO0_10
          RW uint32_t PIO1_10;           // 0x006C          : I/O configuration for pin PIO1_10
          RW uint32_t PIO2_11;           // 0x0070          : I/O configuration for pin PIO2_11
          RW uint32_t PIO0_11;           // 0x0074          : I/O configuration for pin PIO0_11
          RW uint32_t PIO1_0;            // 0x0078          : I/O configuration for pin PIO1_0
          RW uint32_t PIO1_1;            // 0x007C          : I/O configuration for pin PIO1_1
          RW uint32_t PIO1_2;            // 0x0080          : I/O configuration for pin PIO1_2
          RW uint32_t PIO3_0;            // 0x0084          : I/O configuration for pin PIO3_0
          RW uint32_t PIO3_1;            // 0x0088          : I/O configuration for pin PIO3_1
          RW uint32_t PIO2_3;            // 0x008C          : I/O configuration for pin PIO2_3
          RW uint32_t PIO1_3;            // 0x0090          : I/O configuration for pin PIO1_3
          RW uint32_t PIO1_4;            // 0x0094          : I/O configuration for pin PIO1_4
          RW uint32_t PIO1_11;           // 0x0098          : I/O configuration for pin PIO1_11
          RW uint32_t PIO3_2;            // 0x009C          : I/O configuration for pin PIO3_2
          RW uint32_t PIO1_5;            // 0x00A0          : I/O configuration for pin PIO1_5
          RW uint32_t PIO1_6;            // 0x00A4          : I/O configuration for pin PIO1_6
          RW uint32_t PIO1_7;            // 0x00A8          : I/O configuration for pin PIO1_7
          RW uint32_t PIO3_3;            // 0x00AC          : I/O configuration for pin PIO3_3
          RW uint32_t SCK_LOC;           // 0x00B0          : SCK pin location
          RW uint32_t DSR_LOC;           // 0x00B4          : DSR pin location
          RW uint32_t DCD_LOC;           // 0x00B8          : DCD pin location
          RW uint32_t  RI_LOC;           // 0x00BC          :  RI pin location
} lpc111x_iocon_dev_t;

// Chapter 12, Section 12.3, Table 173: GPIO

typedef struct {
          RO RSVD(  0, 0x0000, 0x3FFB ); // 0x0000...0x3FFB : reserved (actually, pin masks)
          RW uint32_t GPIOnDATA;         // 0x3FFC          : data
          RO RSVD(  1, 0x4000, 0x7FFC ); // 0x4000...0x7FFC : reserved
          RW uint32_t GPIOnDIR;          // 0x8000          : direction
          RW uint32_t GPIOnIS;           // 0x8004          :        interrupt sense
          RW uint32_t GPIOnIBE;          // 0x8008          :        interrupt edge
          RW uint32_t GPIOnIEV;          // 0x800C          :        interrupt event
          RW uint32_t GPIOnIE;           // 0x8010          :        interrupt mask
          RO uint32_t GPIOnRIS;          // 0x8014          : raw    interrupt status
          RO uint32_t GPIOnMIS;          // 0x8018          : masked interrupt status
          WO uint32_t GPIOnIC;           // 0x801C          :        interrupt clear
} lpc111x_gpio_dev_t;

// Chapter 13, Section 13.5, Table 184: UART

typedef struct {
  union { RO uint32_t U0RBR;             // 0x0000          : recieve  buffer
          WO uint32_t U0THR;             //                 | transmit buffer
          RW uint32_t U0DLL;          }; //                 | divisor latch LSBs
  union { RW uint32_t U0DLM;             // 0x0004          : divisor latch MSBs
          RW uint32_t U0IER;          }; //                 | interrupt enable
  union { RO uint32_t U0IIR;             // 0x0008          : interrupt ID
          WO uint32_t U0FCR;          };  //                 | FIFO  control
          RW uint32_t U0LCR;             // 0x000C          : line  control
          RW uint32_t U0MCR;             // 0x0010          : modem control
          RO uint32_t U0LSR;             // 0x0014          : line  status
          RO uint32_t U0MSR;             // 0x0018          : modem status
          RW uint32_t U0SCR;             // 0x001C          : scratch pad
          RW uint32_t U0ACR;             // 0x0020          : baud  control
          RO RSVD(  0, 0x0024, 0x0024 ); // 0x0024          : reserved
          RW uint32_t U0FDR;             // 0x0020          : fractional divider
          RO RSVD(  1, 0x002C, 0x002C ); // 0x002C          : reserved
          RW uint32_t U0TER;             // 0x0030          : transmit enable
          RO RSVD(  2, 0x0034, 0x0048 ); // 0x0034...0x0048 : reserved
          RW uint32_t U0RS485CTRL;       // 0x004C          : RS-485 control
          RW uint32_t U0RS485ADRMATCH;   // 0x0050          : RS-485 address match
          RW uint32_t U0RS485DLY;        // 0x0054          : RS-485 direction
} lpc111x_uart_dev_t;

// Chapter 15, Section 15.7, Table 219: I2C

typedef struct {
          RW uint32_t CONSET;            // 0x0000          : control set
          RO uint32_t STAT;              // 0x0004          : status
          RW uint32_t DAT;               // 0x0008          : data
          RW uint32_t ADR0;              // 0x000C          : slave address 0
          RW uint32_t SCLH;              // 0x0010          : +ve, high duty cycle 
          RW uint32_t SCLL;              // 0x0014          : -ve, low  duty cycle
          WO uint32_t CONCLR;            // 0x0018          : control clear
          WO uint32_t MMCTRL;            // 0x001C          : monitor mode control
          RW uint32_t ADR1;              // 0x0020          : slave address 1
          RW uint32_t ADR2;              // 0x0024          : slave address 2
          RW uint32_t ADR3;              // 0x0028          : slave address 3
          RO uint32_t DATA_BUFFER;       // 0x002C          : data buffer
          RO uint32_t COMMASK0;          // 0x0030          : slave address mask 0
          RO uint32_t COMMASK1;          // 0x0034          : slave address mask 1
          RO uint32_t COMMASK2;          // 0x0038          : slave address mask 2
          RO uint32_t COMMASK3;          // 0x003C          : slave address mask 3
} lpc111x_i2c_dev_t;

// Chapter 24, Section 24.5, Table 357: system tick timer                                                                                                                                                                                    

typedef struct {
          RO RSVD(  0, 0x0000, 0x000F ); // 0x0004...0x000F : reserved                                                                                                                                                                        
          RW uint32_t CTRL;              // 0x0010          : control and status                                                                                                                                                              
          RW uint32_t LOAD;              // 0x0014          :      reload value                                                                                                                                                               
          RW uint32_t VAL;               // 0x0018          :     current value                                                                                                                                                               
          RO uint32_t CALIB;             // 0x001C          : calibration value                                                                                                                                                               
} lpc111x_systick_dev_t;

// Chapter 26, Section 26.9/10, Table 408/409: flash controller

typedef struct { 
          RO RSVD(  0, 0x0000, 0x000F ); // 0x0004...0x000F : reserved
          RW uint32_t FLASHCFG;          // 0x0010          : flash configuration
          RO RSVD(  1, 0x0014, 0x001F ); // 0x0014...0x001F : reserved
          RW uint32_t FMSSTART;          // 0x0020          : signature start address
          RW uint32_t FMSSTOP;           // 0x0024          : signature stop  address
          RO RSVD(  2, 0x0028, 0x002B ); // 0x0014...0x001F : reserved
          RO uint32_t FMSW0;             // 0x002C          : word 0
          RO uint32_t FMSW1;             // 0x0030          : word 1
          RO uint32_t FMSW2;             // 0x0034          : word 2
          RO uint32_t FMSW3;             // 0x0038          : word 3
          RO RSVD(  3, 0x003C, 0x0FDF ); // 0x003C...0x0FDF : reserved
          RO uint32_t FMSSTAT;           // 0x0FE0          : signature status
          RO RSVD(  4, 0x0FE4, 0x0FE4 ); // 0x003C...0x0FDF : reserved
          RO uint32_t FMSSTATCLR;        // 0x0FE8          : signature status clear
} lpc111x_fmc_dev_t;

extern lpc111x_syscon_dev_t*  LPC111X_SYSCON;
extern lpc111x_iocon_dev_t*   LPC111X_IOCON;
extern lpc111x_gpio_dev_t*    LPC111X_GPIO0;
extern lpc111x_gpio_dev_t*    LPC111X_GPIO1;
extern lpc111x_gpio_dev_t*    LPC111X_GPIO2;
extern lpc111x_gpio_dev_t*    LPC111X_GPIO3;
extern lpc111x_uart_dev_t*    LPC111X_UART;
extern lpc111x_i2c_dev_t*     LPC111X_I2C;
extern lpc111x_fmc_dev_t*     LPC111X_FMC;

extern lpc111x_systick_dev_t* LPC111X_SYSTICK;

#if defined( LPC111X_SYSTICK_56BIT )
extern uint32_t lpc111x_tsc;
#endif

//  enable interrupts                                                                                                                                                                                                                         
extern void     lpc111x_irq_enable();
// disable interrupts                                                                                                                                                                                                                         
extern void     lpc111x_irq_unable();

#endif
