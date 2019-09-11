# [SCALE](http://www.github.com/danpage/scale): hardware-oriented offshoot (target board: lpc812m101jdh16)

<!--- -------------------------------------------------------------------- --->

## Hardware 

Revision | Prototype                                                                                                 | PCB                                                                                           | Design files                                                                                                                                                                                                                 |
:------: | :-------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
C.1      | <a href='./image/board-prototype_rev_c.jpg'><img src='./image/board-prototype_rev_c.jpg' width='200'></a> | <a href='./image/board-pcb_rev_c.jpg'><img src='./image/board-pcb_rev_c.jpg' width='200'></a> | [schematic](./board.sch) (plus [PDF](./board.pdf)), [board](./board.brd), [BOM](./board.csv), [OSH Park (PCB)](http://www.oshpark.com/shared_projects/ANKAjSGc), [Octopart (BOM)](http://www.octopart.com/bom-tool/BlsVKS9M) |

- The target board houses a
  32-bit ARM Cortex-M0+
  core, which acts as the 
  [DUT](http://en.wikipedia.org/wiki/Device_under_test).
  More specifically, the DUT is a
  16-pin, TSSOP-format
  [NXP lpc812m101jdh16](http://www.nxp.com/documents/data_sheet/LPC81XM.pdf)
  IC (the
  [user manual](http://www.nxp.com/documents/user_manual/UM10601.pdf)
  for which is more detailed); 
  it has 16 kB FLASH and 4 kB SRAM memory.

- Relevant aspects of the DUT pin-out, and their relationship with the
  target board schematic, are illustrated below:

  ```
                        +-----------------+
                       1|                 |16 = PIO0_0  = DUT_RXD
  DUT_GPI = PIO0_12 =  2|                 |15 = PIO0_6  = DUT_TRG
  DUT_RST =            3|                 |14 = PIO_7   = DUT_GPO
  DUT_TXD = PIO0_4  =  4| lpc812m101jdh16 |13 = GND     = DUT_GND
            SWCLK   =  5|                 |12 = VCC     = DUT_VCC
            SWDIO   =  6|                 |11 = XTALIN  = DUT_XTAL1
  DUT_SCL =            7|                 |10 = XTALOUT = DUT_XTAL2
  DUT_SDA =            8|                 |9 
                        +-----------------+
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
  [datasheet](http://www.nxp.com/documents/data_sheet/LPC81XM.pdf)
  specifies that for C_L = 10 pF crystal load capacitance,

  | Type    | Reference             | XTALIN  (`DUT_XTAL1`)                                  | XTALOUT (`DUT_XTAL2`)   |
  |:-------:|:---------------------:|:------------------------------------------------------:|:-----------------------:|
  | Crystal | Figure 37, Table  26  | C_X1 =  18 pF = 0.018 nF                               | C_X2 = 18 pF = 0.018 nF |
  | Clock   | Figure 36             | C_i  = 100 pF = 0.100 nF <br/> C_g = 300 pF = 0.300 nF | disconnected            |

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
              [2](https://bugs.launchpad.net/gcc-arm-embedded/+bug/1502611));
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
