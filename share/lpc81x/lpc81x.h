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

#ifndef __LPC81X_H
#define __LPC81X_H

#include "bsp.h"

// Chapter  4, Section  4.6, Table  17: system configuration

typedef struct {
          RW uint32_t SYSMEMREMAP;       // 0x0000          : system memory remap
          RW uint32_t PRESETCTRL;        // 0x0004          : peripheral reset control
          RW uint32_t SYSPLLCTRL;        // 0x0008          : system PLL control
          RO uint32_t SYSPLLSTAT;        // 0x000C          : system PLL status
          RO RSVD(  0, 0x0010, 0x001C ); // 0x0010...0x001C : reserved
          RW uint32_t SYSOSCCTRL;        // 0x0020          : system oscillator control
          RW uint32_t WDTOSCCTRL;        // 0x0024          : system watchdog   control
          RO RSVD(  1, 0x0028, 0x002C ); // 0x0028...0x002C : reserved
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
          RW uint32_t UARTCLKDIV;        // 0x0094          : UART       clock source divider
          RO RSVD(  6, 0x0098, 0x00DC ); // 0x0098...0x00DC : reserved
          RW uint32_t CLKOUTSEL;         // 0x00E0          : CLKOUT     clock source select
          RW uint32_t CLKOUTUEN;         // 0x00E4          : CLKOUT     clock source update enable
          RW uint32_t CLKOUTDIV;         // 0x00E8          : CLKOUT     clock source divider
          RO RSVD(  7, 0x00EC, 0x00EC ); // 0x00EC          : reserved
          RW uint32_t UARTFRGDIV;        // 0x00F0          : UART fractional generator divider    value
          RW uint32_t UARTFRGMULT;       // 0x00F4          : UART fractional generator multiplier value
          RO RSVD(  8, 0x00F8, 0x00F8 ); // 0x00F8          : reserved
          RW uint32_t EXTTRACECMD;       // 0x00FC          : external trace buffer command register
          RO uint32_t PIOPORCAP0;        // 0x0100          : POR captured PIO status 0
          RO RSVD(  9, 0x0104, 0x0130 ); // 0x0104...0x0130 : reserved
          RW uint32_t IOCONCLKDIV6;      // 0x0134          : peripheral clock 6
          RW uint32_t IOCONCLKDIV5;      // 0x0138          : peripheral clock 5
          RW uint32_t IOCONCLKDIV4;      // 0x013C          : peripheral clock 4
          RW uint32_t IOCONCLKDIV3;      // 0x0140          : peripheral clock 3
          RW uint32_t IOCONCLKDIV2;      // 0x0144          : peripheral clock 2
          RW uint32_t IOCONCLKDIV1;      // 0x0148          : peripheral clock 1
          RW uint32_t IOCONCLKDIV0;      // 0x014C          : peripheral clock 0
          RW uint32_t BODCTRL;           // 0x0150          : brown-out detection
          RW uint32_t SYSTCKCAL;         // 0x0154          : system tick counter calibration
          RO RSVD( 10, 0x0158, 0x016C ); // 0x0158...0x016C : reserved
          RW uint32_t IRQLATENCY;        // 0x0170          : IRQ delay
          RW uint32_t NMISRC;            // 0x0174          : NMI source control
          RW uint32_t PINTSEL0;          // 0x0178          : GPIO pin interrupt select register 0
          RW uint32_t PINTSEL1;          // 0x017C          : GPIO pin interrupt select register 1
          RW uint32_t PINTSEL2;          // 0x0180          : GPIO pin interrupt select register 2
          RW uint32_t PINTSEL3;          // 0x0184          : GPIO pin interrupt select register 3
          RW uint32_t PINTSEL4;          // 0x0188          : GPIO pin interrupt select register 4
          RW uint32_t PINTSEL5;          // 0x018C          : GPIO pin interrupt select register 5
          RW uint32_t PINTSEL6;          // 0x0190          : GPIO pin interrupt select register 6
          RW uint32_t PINTSEL7;          // 0x0194          : GPIO pin interrupt select register 7
          RO RSVD( 11, 0x0198, 0x0200 ); // 0x0198...0x0200 : reserved
          RW uint32_t STARTERP0;         // 0x0204          : start logic enable register 0
          RO RSVD( 12, 0x0208, 0x0210 ); // 0x0208...0x0210 : reserved
          RW uint32_t STARTERP1;         // 0x0214          : start logic enable register 1
          RO RSVD( 13, 0x0218, 0x022C ); // 0x0218...0x022C : reserved
          RW uint32_t PDSLEEPCFG;        // 0x0230          : power-down states for           deep-sleep mode
          RW uint32_t PDAWAKECFG;        // 0x0234          : power-down states for wake from deep-sleep mode
          RW uint32_t PDRUNCFG;          // 0x0238          : power configuration register
          RO RSVD( 14, 0x023C, 0x03F4 ); // 0x023C...0x03F4 : reserved
          RO uint32_t DEVICE_ID;         // 0x03F8          : device ID
} lpc81x_syscon_dev_t;

// Chapter  6, Section  6.5, Table  61: I/O configuration

typedef struct { 
          RW uint32_t PIO0_17;           // 0x0000          : I/O configuration for pin PIO0_17
          RW uint32_t PIO0_13;           // 0x0004          : I/O configuration for pin PIO0_13
          RW uint32_t PIO0_12;           // 0x0008          : I/O configuration for pin PIO0_12
          RW uint32_t PIO0_5;            // 0x000C          : I/O configuration for pin PIO0_5
          RW uint32_t PIO0_4;            // 0x0010          : I/O configuration for pin PIO0_4
          RW uint32_t PIO0_3;            // 0x0014          : I/O configuration for pin PIO0_3
          RW uint32_t PIO0_2;            // 0x0018          : I/O configuration for pin PIO0_2
          RW uint32_t PIO0_11;           // 0x001C          : I/O configuration for pin PIO0_11
          RW uint32_t PIO0_10;           // 0x0020          : I/O configuration for pin PIO0_10
          RW uint32_t PIO0_16;           // 0x0024          : I/O configuration for pin PIO0_16
          RW uint32_t PIO0_15;           // 0x0028          : I/O configuration for pin PIO0_15
          RW uint32_t PIO0_1;            // 0x002C          : I/O configuration for pin PIO0_1
          RO RSVD(  0, 0x0030, 0x0030 ); // 0x0030          : reserved
          RW uint32_t PIO0_9;            // 0x0034          : I/O configuration for pin PIO0_9
          RW uint32_t PIO0_8;            // 0x0038          : I/O configuration for pin PIO0_8
          RW uint32_t PIO0_7;            // 0x003C          : I/O configuration for pin PIO0_7
          RW uint32_t PIO0_6;            // 0x0040          : I/O configuration for pin PIO0_6
          RW uint32_t PIO0_0;            // 0x0044          : I/O configuration for pin PIO0_0
          RW uint32_t PIO0_14;           // 0x0048          : I/O configuration for pin PIO0_14
} lpc81x_iocon_dev_t;

// Chapter  7, Section  7.6, Table  81: GPIO

typedef struct {                            
          RW  uint8_t B0[ 18 ];          // 0x0000...0x0011 : byte pin   registers, port 0
          RO RSVD(  0, 0x0012, 0x0FFF ); // 0x0012...0x0FFF : reserved
          RW uint32_t W0[ 18 ];          // 0x1000...0x1044 : word pin   registers, port 0
          RO RSVD(  1, 0x1048, 0x1FFF ); // 0x1048...0x1FFF : reserved
          RW uint32_t DIR0;              // 0x2000          : direction  register,  port 0
          RO RSVD(  2, 0x2004, 0x207C ); // 0x2004...0x207C : reserved
          RW uint32_t MASK0;             // 0x2080          : mask       register,  port 0
          RO RSVD(  3, 0x2084, 0x20FC ); // 0x2084...0x20FC : reserved
          RW uint32_t PIN0;              // 0x2100          :        pin register,  port 0
          RO RSVD(  4, 0x2104, 0x217C ); // 0x2104...0x217C : reserved
          RW uint32_t MPIN0;             // 0x2180          : masked pin register,  port 0
          RO RSVD(  5, 0x2184, 0x21FC ); // 0x2084...0x21FC : reserved
          RW uint32_t SET0;              // 0x2200          : set        register,  port 0
          RO RSVD(  6, 0x2204, 0x227C ); // 0x2204...0x227C : reserved
          WO uint32_t CLR0;              // 0x2280          : clear      register,  port 0
          RO RSVD(  7, 0x2284, 0x22FC ); // 0x2284...0x22FC : reserved
          WO uint32_t NOT0;              // 0x2300          : toggle     register,  port 0
} lpc81x_gpio_dev_t;

// Chapter  9, Section  9.5, Table 108: switch matrix

typedef struct {
          RW uint32_t PINASSIGN0;        // 0x0000          : pin assign register 0
          RW uint32_t PINASSIGN1;        // 0x0004          : pin assign register 1
          RW uint32_t PINASSIGN2;        // 0x0008          : pin assign register 2
          RW uint32_t PINASSIGN3;        // 0x000C          : pin assign register 3
          RW uint32_t PINASSIGN4;        // 0x0010          : pin assign register 4
          RW uint32_t PINASSIGN5;        // 0x0014          : pin assign register 5
          RW uint32_t PINASSIGN6;        // 0x0018          : pin assign register 6
          RW uint32_t PINASSIGN7;        // 0x001C          : pin assign register 7
          RW uint32_t PINASSIGN8;        // 0x0020          : pin assign register 8
          RO RSVD(  0, 0x0024, 0x01BC ); // 0x0024...0x01BC : reserved
          RW uint32_t PINENABLE0;        // 0x01C0          : pin enable register 0
} lpc81x_swm_dev_t;

// Chapter 14, Section 14.6, Table 166: system tick timer

typedef struct {
          RO RSVD(  0, 0x0000, 0x000F ); // 0x0004...0x000F : reserved
          RW uint32_t SYST_CSR;          // 0x0010          : control and status
          RW uint32_t SYST_RVR;          // 0x0014          :      reload value
          RW uint32_t SYST_CVR;          // 0x0018          :     current value
          RO uint32_t SYST_CALIB;        // 0x001C          : calibration value
} lpc81x_systick_dev_t;

// Chapter 15, Section 15.6, Table 172: UART

typedef struct {
          RW uint32_t CFG;               // 0x0000          : configuration
          RW uint32_t CTRL;              // 0x0004          : control      
          RW uint32_t STAT;              // 0x0008          : status       
          RW uint32_t INTENSET;          // 0x000C          : interrupt enable and set
          WO uint32_t INTENCLR;          // 0x0010          : interrupt enable and clear
          RO uint32_t RXDATA;            // 0x0014          : receive  data
          RO uint32_t RXDATA_STAT;       // 0x0018          : receive  data with status
          RW uint32_t TXDATA;            // 0x001C          : transmit data
          RW uint32_t BRG;               // 0x0020          : baud rate generator
          RO uint32_t INTSTAT;           // 0x0024          : interrupt status
} lpc81x_uart_dev_t;

// Chapter 16, Section 16.5, Table 184: I2C

typedef struct {
          RW uint32_t CFG;               // 0x0000          : configuration
          RW uint32_t STAT;              // 0x0004          : status
          RW uint32_t INTENSET;          // 0x0008          : interrupt enable set
          WO uint32_t INTENCLR;          // 0x000C          : interrupt enable clear
          RW uint32_t TIMEOUT;           // 0x0010          : time-out value    
          RW uint32_t CLKDIV;            // 0x0014          : clock configuration
          RO uint32_t INTSTAT;           // 0x0018          : interrupt status
          RO RSVD(  0, 0x001C, 0x001C ); // 0x001C          : reserved
          RW uint32_t MSTCTL;            // 0x0020          : master control
          RW uint32_t MSTTIME;           // 0x0024          : master timing configuration
          RW uint32_t MSTDAT;            // 0x0028          : master data
          RO RSVD(  1, 0x002C, 0x003C ); // 0x002C...0x003C : reserved
          RW uint32_t SLVCTL;            // 0x0040          : slave  control
          RW uint32_t SLVDAT;            // 0x0044          : slave  data
          RW uint32_t SLVADR0;           // 0x0048          : slace address mask 0
          RW uint32_t SLVADR1;           // 0x004C          : slace address mask 1
          RW uint32_t SLVADR2;           // 0x0050          : slace address mask 2
          RW uint32_t SLVADR3;           // 0x0054          : slace address mask 3
          RW uint32_t SLVQUAL0;          // 0x0058          : slace qualification
          RO RSVD(  2, 0x005C, 0x007C ); // 0x005C...0x007C : reserved
          RO uint32_t MONRXDAT;          // 0x0080          : monitor receive data
} lpc81x_i2c_dev_t;

// Chapter 20, Section 20.4, Table 225: flash controller

typedef struct { 
          RO RSVD(  0, 0x0000, 0x000F ); // 0x0004...0x000F : reserved
          RW uint32_t FLASHCFG;          // 0x0010          : flash configuration
          RO RSVD(  1, 0x0014, 0x001F ); // 0x0014...0x001F : reserved
          RW uint32_t FMSSTART;          // 0x0020          : signature start address
          RW uint32_t FMSSTOP;           // 0x0024          : signature stop  address
          RO RSVD(  2, 0x0028, 0x002B ); // 0x0014...0x001F : reserved
          RO uint32_t FMSW0;             // 0x002C          : word 0
} lpc81x_fmc_dev_t;

extern lpc81x_syscon_dev_t*  LPC81X_SYSCON;
extern lpc81x_iocon_dev_t*   LPC81X_IOCON;
extern lpc81x_gpio_dev_t*    LPC81X_GPIO;
extern lpc81x_swm_dev_t*     LPC81X_SWM;
extern lpc81x_uart_dev_t*    LPC81X_UART0;
extern lpc81x_uart_dev_t*    LPC81X_UART1;
extern lpc81x_uart_dev_t*    LPC81X_UART2;
extern lpc81x_i2c_dev_t*     LPC81X_I2C;
extern lpc81x_fmc_dev_t*     LPC81X_FMC;

extern lpc81x_systick_dev_t* LPC81X_SYSTICK;

#if defined( LPC81X_SYSTICK_56BIT )
extern uint32_t lpc81x_tsc;
#endif

//  enable interrupts                                                                                                                                                                                                                        $
extern void     lpc81x_irq_enable();
// disable interrupts                                                                                                                                                                                                                        $
extern void     lpc81x_irq_unable();

#endif
