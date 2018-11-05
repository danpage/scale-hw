# [SCALE](http://www.github.com/danpage/scale): hardware-oriented offshoot (target board: lpc114fn28)

<!--- -------------------------------------------------------------------- --->

## Hardware 

Revision | Prototype                                                                                                 | PCB                                                                                           | Design files                                                                                                                                                                                                                 |
:------: | :-------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
C.1      | <a href='./image/board-prototype_rev_c.jpg'><img src='./image/board-prototype_rev_c.jpg' width='200'></a> | <a href='./image/board-pcb_rev_c.jpg'><img src='./image/board-pcb_rev_c.jpg' width='200'></a> | [schematic](./board.sch) (plus [PDF](./board.pdf)), [board](./board.brd), [BOM](./board.csv), [OSH Park (PCB)](http://www.oshpark.com/shared_projects/XgebkWyr), [Octopart (BOM)](http://www.octopart.com/bom-tool/vM74sniH) |

- The target board houses a
  32-bit ARM Cortex-M0 
  core, which acts as the 
  [DUT](http://en.wikipedia.org/wiki/Device_under_test).
  More specifically, the DUT is a
  28-pin, DIP-format
  [NXP lpc114fn28/102](http://www.nxp.com/documents/data_sheet/LPC111X.pdf)
  IC (the
  [user manual](http://www.nxp.com/documents/user_manual/UM10398.pdf)
  for which is more detailed); 
  it has 32 kB FLASH and 4 kB SRAM memory.
  Unfortunately NXP have discontinued this part, although you can often
  still source instances via
  [ebay](http://www.ebay.co.uk/sch/lpc114fn28).

- Relevant aspects of the DUT pin-out, and their relationship with the
  target board schematic, are illustrated below:

  ```
                       +----------------+
  DUT_TRG = PIO0_8 =  1|                |28
  DUT_GPO = PIO0_9 =  2|                |27           = DUT_SCL            
            SWCLK  =  3|                |26             
                      4|                |25             
  DUT_SDA =           5|                |24 = PIO0_1  = DUT_GPI
                      6|                |23           = DUT_RST       
                      7| lpc114fn28/102 |22 = GND     = DUT_GND
                      8|                |21 = VCC     = DUT_VCC
                      9|                |20 = XTALIN  = DUT_XTAL1
                     10|                |19 = XTALOUT = DUT_XTAL2
                     11|                |18             
            SWDIO  = 12|                |17             
                     13|                |16           = DUT_TXD       
                     14|                |15           = DUT_RXD
                       +----------------+
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
  [datasheet](http://www.nxp.com/documents/data_sheet/LPC111X.pdf)
  specifies that for C_L = 10 pF crystal load capacitance,

  | Type    | Reference             | XTALIN  (`DUT_XTAL1`)                                  | XTALOUT (`DUT_XTAL2`)   |
  |:-------:|:---------------------:|:------------------------------------------------------:|:-----------------------:|
  | Crystal | Figure 50, Table  30  | C_X1 =  18 pF = 0.018 nF                               | C_X2 = 18 pF = 0.018 nF |
  | Clock   | Figure 49             | C_i  = 100 pF = 0.100 nF <br/> C_g = 300 pF = 0.300 nF | disconnected            |

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
