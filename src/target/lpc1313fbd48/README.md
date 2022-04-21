# [SCALE](http://www.github.com/danpage/scale): hardware-oriented offshoot (target board: lpc1313fbd48)

<!--- -------------------------------------------------------------------- --->

## Hardware 

Revision | Prototype                                                                                                 | PCB                                                                                           | Design files                                                                                                                                                                                                                 |
:------: | :-------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
C.1      | <a href='./image/board-prototype_rev_c.jpg'><img src='./image/board-prototype_rev_c.jpg' width='200'></a> | <a href='./image/board-pcb_rev_c.jpg'><img src='./image/board-pcb_rev_c.jpg' width='200'></a> | [schematic](./board.sch) (plus [PDF](./board.pdf)), [board](./board.brd), [BOM](./board.csv), [OSH Park (PCB)](http://www.oshpark.com/shared_projects/dp9i0DIr), [Octopart (BOM)](http://www.octopart.com/bom-tool/5ZnEZk9y) |

- The target board houses a
  32-bit ARM Cortex-M3
  core, which acts as the 
  [DUT](http://en.wikipedia.org/wiki/Device_under_test).
  More specifically, the DUT is a
  48-pin, LQFP-format
  [NXP lpc1313fbd48/151](http://www.nxp.com/docs/en/data-sheet/LPC1311_13_42_43.pdf)
  IC (the
  [user manual](http://www.nxp.com/docs/en/user-guide/UM10375.pdf)
  for which is more detailed); 
  it has 32 kB FLASH and 8 kB SRAM memory.

- Relevant aspects of the DUT pin-out, and their relationship with the
  target board schematic, are illustrated below:

  ```
                             +----------------------- = TXD    = DUT_TXD
                             | +--------------------- = RXD    = DUT_RXD
                             | |   +----------------- = VDD    = DUT_VCC
                             | |   |     +----------- = VSS    = DUT_GND
                             | |   |     |   +------- = SWDIO
                             | |   |     |   |
                                           
                           4 4 4 4 4 4 4 4 4 3 3 3
                           8 7 6 5 4 3 2 1 0 9 8 7
                          +-----------------------+
                         1|                       |36
                         2|                       |35 
  DUT_RST             =  3|                       |34 = PIO1_1 = DUT_GPO
  DUT_GPI   = PIO0_1  =  4|                       |33 = PIO1_0 = DUT_TRG
  DUT_GND   = VSS     =  5|                       |32
  DUT_XTAL1 = XTALIN  =  6|      lpc1313fbd48     |31
  DUT_XTAL2 = XTALOUT =  7|                       |30
  DUT_VCC   = VDD     =  8|                       |29 = SWCLK
                         9|                       |28 = SWO
                        10|                       |27
                        11|                       |26
                        12|                       |25
                          +-----------------------+
                           1 1 1 1 1 1 1 2 2 2 2 2
                           3 4 5 6 7 8 9 0 1 2 3 4

                               | |
  DUT_SCL   = SCL     = -------+ |
  DUT_SDA   = SDA     = ---------+
  ```

- The core voltage required on `DUT_VCC` is 3.3 V: 
  `H.J2` must therefore be set appropriately.

- The core has a
  12 MHz
  internal oscillator, but can be driven by either 

  - an external clock source provided (on `DUT_XTAL1` and `DUT_XTAL2` via `DUT_CLK`),
    or
  - 16 MHz external crystal housed on the target board.

  To support these, some extra, carefully selected capacitors
  are required: the DUT
  [datasheet](http://www.nxp.com/docs/en/data-sheet/LPC1311_13_42_43.pdf)
  specifies that for C_L = 10 pF crystal load capacitance,

  | Type    | Reference             | XTALIN  (`DUT_XTAL1`)                                  | XTALOUT (`DUT_XTAL2`)   |
  |:-------:|:---------------------:|:------------------------------------------------------:|:-----------------------:|
  | Crystal | Figure 35, Table  21  | C_X1 =  18 pF = 0.018 nF                               | C_X2 = 18 pF = 0.018 nF |
  | Clock   | Figure 34             | C_i  = 100 pF = 0.100 nF <br/> C_g = 300 pF = 0.300 nF | disconnected            |

  In the latter case we want 200 mV < f * V_RMS < 1000 mV, where
  f = C_i / ( C_i + C_g ),
  so if we pick V_peak = 3.3 V, then because
  V_RMS  = V_peak * \sqrt{duty cycle}
         = 3.3    * \sqrt{1/2}
        ~= 2.3
  then f = 1/4 and hence C_g = 300 pF does the job.

- Specific pins on the DUT are made accessible via `T.J2` in order to
  allow attachment of a hardware programmer or debugger.
  For example, it is possible to attach a
  [Segger J-Link](http://www.segger.com/admin/uploads/productDocs/UM08001_JLink.pdf)
  to the DUT by connecting the 20-pin header as follows

  ```
              +--------+
  DUT_VCC =  1|        |2
             3|        |4  = DUT_GND
             5|        |6
    SWDIO =  7|        |8
    SWCLK =  9| J-Link |10
            11|        |12
            13|        |14
  DUT_RST = 15|        |16
            17|        |18
            19|        |20
              +--------+
  ```
               
  and thereby make use of Serial Wire Debug (SWD).

  That said, it *seems* the DUT isn't correctly identified by 
  [OpenOCD](http://openocd.org),
  at least per version 
  [0.10.0](http://sourceforge.net/p/openocd/code/ci/v0.10.0/tree/).  
  It's not totally clear why: the flash driver seems to read a part ID
  of `0x2000002B` (vs. an expected part ID `0x2C40102B`).  This can be 
  resolved by adding something like

  ```
  #define LPC1313_2 0x2000002B
  ```
 
  plus an appropriate case in the `lpc2000_auto_probe_flash` function 
  of
  [`${OPENOCD}/src/flash/nor/lpc2000.c`](http://sourceforge.net/p/openocd/code/ci/v0.10.0/tree/src/flash/nor/lpc2000.c);
  alternatively, apply the
  [patch](./openocd-0.10.0.patch)
  provided.

<!--- -------------------------------------------------------------------- --->

## Software

- Under
  [Ubuntu](http://www.ubuntu.com)-based
  Linux, for example, 
  the development dependencies can be installed 
  by issuing the command

  ```
  sudo apt-get install gcc-arm-none-eabi lpc21isp
  ```

  That said, some versions of this 
  [Ubuntu packaged](https://launchpad.net/ubuntu/+source/gcc-arm-none-eabi) 
  tool-chain have known issues 
  (see, e.g., [1](https://bugs.launchpad.net/gcc-arm-embedded/+bug/1722849)
              or
              [2](https://bugs.launchpad.net/gcc-arm-embedded/+bug/1502611)):
  take care, or, as an alternative, consider using an 
  ["official" release](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain)
  directly from ARM instead.

- A pre-installed 
  [bootloader](http://en.wikipedia.org/wiki/Booting)
  allows programming of the DUT over USB;
  this means
  [lpc21isp](http://sourceforge.net/projects/lpc21isp)
  can be used as is.  
  The GPI switch on the host board will activate the bootloader during 
  reset, so the steps required are basically

  - execute lpc2isp, which then waits,
  - press and hold the (right-hand) GPI   switch,  
  - press and hold the  (left-hand) reset switch,
  - release        the  (left-hand) reset switch,
  - transfer via lpc2isp starts,
  - release        the (right-hand) GPI   switch,
  - transfer via lpc2isp finishes.

- Note that the default BSP configuration is conservative, st.

  - the
    internal 12 MHz oscillator
    is used as the clock source,
    meaning `T.J3` and `T.J4` on the target board 
    are irrelevant,
  - the 
    UART 
    interface operates per 9600/8N1,
  - the
    I2C
    interface operates per 100 kHz standard mode.

<!--- -------------------------------------------------------------------- --->
