# [SCALE](http://www.github.com/danpage/scale): hardware-oriented material (target board: atmega328p)

<!--- -------------------------------------------------------------------- --->

## Hardware 

Revision | Prototype                                                                                                 | PCB                                                                                           | Design files                                                                                                                                                                                                                 |
:------: | :-------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
C.1      | <a href='./image/board-prototype_rev_c.jpg'><img src='./image/board-prototype_rev_c.jpg' width='200'></a> | <a href='./image/board-pcb_rev_c.jpg'><img src='./image/board-pcb_rev_c.jpg' width='200'></a> | [schematic](./board.sch) (plus [PDF](./board.pdf)), [board](./board.brd), [BOM](./board.csv), [OSH Park (PCB)](http://www.oshpark.com/shared_projects/YczSc1Wc), [Octopart (BOM)](http://www.octopart.com/bom-tool/CbDjCRPB) |

- The target board houses an 
  8-bit Atmel ATmega
  core, which acts as the 
  [DUT](http://en.wikipedia.org/wiki/Device_under_test).
  More specifically, the DUT is a
  28-pin, DIP-format
  [Atmel atmega328p](http://www.atmel.com/Images/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf)
  IC; 
  it has 32 kB FLASH and 2 kB SRAM memory.

- Relevant aspects of the DUT pin-out, and their relationship with the
  target board schematic, are illustrated below:

  ```
                       +------------+
  DUT_RST   = PC6  =  1|            |28 =         DUT_SCL
  DUT_RXD   = PD0  =  2|            |27 =         DUT_SDA
  DUT_TXD   = PD1  =  3|            |26
  DUT_GPI   = PD2  =  4|            |25
                      5|            |24
                      6|            |23
  DUT_VCC   = VCC  =  7| ATmega328p |22 =  GND
              GND  =  8|            |21
  DUT_XTAL1 = PB6  =  9|            |20 =  VCC  = DUT_VCC
  DUT_XTAL2 = PB7  = 10|            |19 =  SCK
                     11|            |18 =  MISO
                     12|            |17 =  MOSI
  DUT_GPO   = PD7  = 13|            |16
  DUT_TGR   = PB0  = 14|            |15
                       +------------+
  ```

- The core voltage required on `DUT_VCC` is 5 V: 
  `H.J2` must therefore be set appropriately.

- The core has a
  1 MHz
  internal clock source, but can be driven by either 

  - an external clock source provided (on `DUT_XTAL1` and `DUT_XTAL2` via `DUT_CLK`),
    or
  - 16 MHz external crystal housed on the target board.

  To support these, some extra, carefully selected capacitors
  are required: the DUT
  [datasheet](http://www.atmel.com/Images/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf)
  specifies that

  | Type    | Reference   | XTALIN  (`DUT_XTAL1`)                                  | XTALOUT (`DUT_XTAL2`)   |
  |:-------:|:-----------:|:------------------------------------------------------:|:-----------------------:|
  | Crystal | Figure 13-2 | C1   =  22 pF = 0.022 nF                               | C2   = 22 pF = 0.022 nF |
  | Clock   | Figure 13-3 | no capacitors required                                 | disconnected            |

- Specific pins on the DUT are made accessible via `T.J2` in order to
  allow attachment of a hardware programmer or debugger.
  For example, it is possible to attach an
  [Atmel AVRISP mkII](http://www.atmel.com/Images/Atmel-42093-AVR-ISP-mkII_UserGuide.pdf)
  (or indeed *any* low-cost ISP-based programmer)
  to the DUT by connecting the 6-pin header as follows

  ```
             +-----+
     MISO = 1|     |2 = DUT_VCC
      SCK = 3| ISP |4 = MOSI
  DUT_RST = 5|     |6 = DUT_GND
             +-----+
  ```

  taking care wrt. pin 2.  Some programmers will sense a core voltage
  from this pin, some will *supply* a core voltage using it, some can
  be configured to do either: by whatever means (e.g., disconnecting
  the pin, or configuring the programmer), do *not* use it supply a 
  core voltage.

<!--- -------------------------------------------------------------------- --->

## Software

- Under
  [Ubuntu](http://www.ubuntu.com)-based
  Linux, for example, 
  the development dependencies can be installed 
  by issuing the command

  ```
  sudo apt-get install gcc-avr avrdude
  ```

- We assume an Arduino-based
  [bootloader](http://en.wikipedia.org/wiki/Booting),
  such as
  [Optiboot](http://www.github.com/Optiboot/optiboot),
  has been pre-installed to allow programming of the DUT over USB;
  this means
  [avrdude](http://www.nongnu.org/avrdude)
  can be used as is,
  as can the Arduino
  [IDE](http://www.arduino.cc/en/main/software)
  if you prefer.

- Note that the default BSP configuration is conservative, st.

  - the
    external 16 MHz crystal
    is used as the clock source,
    meaning `T.J3` and `T.J4` on the target board 
    must be set appropriately,
  - the 
    UART 
    interface operates per 9600/8N1,
  - the
    I2C
    interface operates per 100 kHz standard mode.

<!--- -------------------------------------------------------------------- --->
