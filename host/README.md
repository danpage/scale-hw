# [SCALE](http://www.github.com/danpage/scale): hardware-oriented offshoot (host board)

<!--- -------------------------------------------------------------------- --->

## Hardware 

Revision | Prototype                                                                                                 | PCB                                                                                           | Design files                                                                                                                                                                                                                 |
:------: | :-------------------------------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
C.1      | <a href='./image/board-prototype_rev_c.jpg'><img src='./image/board-prototype_rev_c.jpg' width='200'></a> | <a href='./image/board-pcb_rev_c.jpg'><img src='./image/board-pcb_rev_c.jpg' width='200'></a> | [schematic](./board.sch) (plus [PDF](./board.pdf)), [board](./board.brd), [BOM](./board.csv), [OSH Park (PCB)](http://www.oshpark.com/shared_projects/F13RPtpf), [Octopart (BOM)](http://www.octopart.com/bom-tool/LBFQutEU) |

- All communication with a control platform is performed via a
  [USB](http://en.wikipedia.org/wiki/USB)-to-[UART](http://en.wikipedia.org/wiki/Universal_asynchronous_receiver/transmitter)
  bridge, namely an
  [FTDI FT232RL](http://www.ftdichip.com/Support/Documents/DataSheets/ICs/DS_FT232R.pdf)
  (which ultimately connects to `DUT_RXD` and `DUT_TXD`).
  The surrounding design

  - basically matches the
    [SparkFun FTDI Basic Breakout](http://cdn.sparkfun.com/datasheets/BreakoutBoards/FTDI%20Basic-v22-3.3V.pdf),
    but includes a polyfuse (the breadboard prototype seems to pull about
    30 mA @ 3.3V),

  - differs from Figure 7.4 (page 31) of FT232RL datasheet wrt. both the 
    capacitors, and lack of ferrite bead.

- Both `DUT_RST` and `DUT_GPI` are connected to
  [SPST switches](http://en.wikipedia.org/wiki/Switch)
  switches.
  The surrounding design is motivated by 
  [Arduino UNO](http://www.arduino.cc/en/uploads/Main/arduino-uno-schematic.pdf),
  replacing `GND` with `UART_DTR` on the non-switch side of the capacitor
  to allow a programmer, e.g., `avrdude`, to perform a "remote reset".

- The center pin of `H.J2` ultimately supplies power the target board via 
  `DUT_VCC`: a jumper can be used to select between

  - a   5 V supply stemming from the FT232RL,
    or
  - a 3.3 V supply stemming from the FT232RL,

  or, by default (i.e., with no jumper), an external power supply which
  is provided via the labelled SMA connector and hence `PSU`.

- A signal is acquired from either `DUT_VCC` or `DUT_SIGI`, depending on
  `H.J4`, before being processed by a pipeline of components and produced
  via `DUT_SIGO`.

  - The pipeline includes a 
    100 ohm shunt resistor,
    which can be enabled or disabled using `H.J3`.

  - The pipeline includes a
    [Low Noise Amplifier (LNA)](http://en.wikipedia.org/wiki/Low-noise_amplifier),
    namely an
    [NXP BGA2801](http://www.nxp.com/documents/data_sheet/BGA2801.pdf),
    which can be enabled or disabled using `H.J5`.
    The surrounding design is basically the same as the
    [ChipWhisperer LNA module](http://wiki.newae.com/CW502_Low_Noise_Amplifier),
    which, in turn, is basically the same as the NXP reference design in
    Figure 1 (page 4) of the BGA2801 datasheet.
  
  - The pipeline includes a
    [low-pass filter](http://en.wikipedia.org/wiki/Low-pass_filter);
    which can be enabled or disabled using `H.J6`.
    The surrounding design is based in a "pi-section" structure, i.e.,
  
    ```
                 L
      ----o---8888888---o----
          |             |
          = C/2         = C/2
          |             |
         GND           GND
    ```
  
    where we select
    L   =  10.0 uH
    and
    C/2 =   1.5 nF
    to produce roughly a 2.6 MHz cut-off.

- `DUT_SCL` and `DUT_SDA` are connected to three
  [Atmel CryptoAuthentication](http://www.atmel.com/products/security-ics/cryptoauthentication)
  I2C peripherals, 
  which can be enabled or disabled using `H.J7`.
  More specifically, those peripherals are one each of

  - [ATAES132A](http://www.atmel.com/Images/Atmel-8914-CryptoAuth-ATAES132A-Datasheet.pdf),
  - [ATSHA204A](http://www.atmel.com/images/Atmel-8885-CryptoAuth-ATSHA204A-Datasheet.pdf),
    and
  - [ATECC508A](http://www.atmel.com/Images/Atmel-8923S-CryptoAuth-ATECC508A-Datasheet-Summary.pdf),

  that offer hardware-backed, cryptography-related functionality and
  some amount of generic 
  [EEPROM](http://en.wikipedia.org/wiki/EEPROM)
  storage.

<!--- -------------------------------------------------------------------- --->
