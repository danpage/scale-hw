# [SCALE](http://www.github.com/danpage/scale): hardware-oriented offshoot

<!--- -------------------------------------------------------------------- --->

## Concept

The equipment used for a typical 
[side-channel attack](http://en.wikipedia.org/wiki/Side-channel_attack)
based on
[power analysis](http://en.wikipedia.org/wiki/Power_analysis)
might resemble the following:

```
                                                                   .... +-------------+
                                                                  /     | SCALE       |
+-------------+         +-------------+   trigger  +-------------+      | target      |
| control     |   USB   | acquisition | <--------- | target      |      | board       |
| platform    | <-----> | platform    |   signal   | platform    |      +-------------+
|             |         |             | <--------- |             |      | SCALE       |
+-------------+         +-------------+            +-------------+      | host        |
      ^                                                  ^        \     | board       |   
      |           USB                                    |         .... +-------------+
      +--------------------------------------------------+
```

The basic idea is that

- the target      platform represents the device under attack,
  or the
  [Device Under Test (DUT)](http://en.wikipedia.org/wiki/Device_under_test)
  in more common terms,

- the acquisition platform represents a   device, e.g., an
  [oscilloscope](http://en.wikipedia.org/wiki/Oscilloscope),
  to capture a signal from the target platform (under control of a trigger)
  and relay it to the control platform,

- the control     platform represents a workstation, which orchestrates the 
  attack by controlling the target and acquisition platforms: it will often
  repeat an acquisition loop, i.e.,

  - configure (e.g., fix the acquisition period) and initialise (e.g., arm 
    the trigger) the acquisition platform,
  - send    input     (e.g.,  plaintext) to   target      platform,
  - invoke  operation (e.g., encryption) on   target      platform,
  - receive output    (e.g., ciphertext) from target      platform,
    then,
  - receive trace                        from acquisition platform,

  to acquire a set of traces, then apply some 
  [(crypt)analytic](http://en.wikipedia.org/wiki/Cryptanalysis) 
  technique to them in order to recover information.

None of the above is an unreasonable expectation within the context of a 
research-oriented lab. but may present challenges in a teaching-oriented
alternative.  For example, particularly at scale, it may be that

- the 
  [Total Cost of Ownership (TCO)](http://en.wikipedia.org/wiki/Total_cost_of_ownership),
 i.e., capital investment plus maintenance, 
- the domain-specific requirements on 
  [Commercial Off-The-Shelf (COTS)](http://en.wikipedia.org/wiki/Commercial_off-the-shelf)
  target boards, such as the need to insert a shunt resistor into the 
  power rail,
- the need to expose at least some
  [EE](http://en.wikipedia.org/wiki/Electrical_engineering)-style 
  detail to students without a suitable background,

prevent such an approach being viable (or make it less effective).

The SCALE hardware-oriented offshoot represents a suite of 
bespoke, hardware-based target platform designs:
when combined with suitable control and acquisition platforms, it offers
a real, albeit sanitised "kit" for learning about side-channel attacks.
It has a strict remit, namely educational use (e.g., as a lab. exercise 
or assignment), which to some extent determines goals, design decisions, 
and, ultimately, the content.

<!--- -------------------------------------------------------------------- --->

## Content

#### Prepare pre-requisites

Some content is stored using
[Git Large File Storage (LFS)](http://git-lfs.github.com/):
it makes sense to install this first, otherwise that content appears
as a set of pointers to data vs. the data itself.

#### Prepare     repository

This is actually a 
[submodule](http://www.git-scm.com/docs/git-submodule)
of the
[SCALE](http://www.github.com/danpage/scale)
repository: you can *either*

1. follow the instructions to prepare the SCALE repository, then
   use

   ```
   cd ${SCALE}/hw ; export SCALE_HW="${PWD}"
   ```

   to fix the working directory, 
   *or*

2. treat it as a standalone repository,
   using

   ```
   git clone http://www.github.com/danpage/scale-hw.git
   ```
   
   to clone the repository, then

   ```
   cd scale-hw ; export SCALE_HW="${PWD}"
   ```

   to fix the working directory.

Note that, either way, the associated path is denoted by 
`${SCALE_HW}` 
from here on.  

#### Use       content

Each target platform is split into two components:
     
- A [host board](./host/README.md)
  (or   [motherboard](http://en.wikipedia.org/wiki/Motherboard)),
  which houses all the generic  functionality;
  this is conceptually analogous to the
  [ChipWhisperer](http://www.newae.com/tools/chipwhisperer)
  [UFO   board](http://store.newae.com/cw308-ufo-board).

- A target board
  (or [daughterboard](http://en.wikipedia.org/wiki/Expansion_card)),
  which houses     the specific functionality for a given device (e.g., 
  a specific micro-controller) and can be switched between as need be;
  this is conceptually analogous to a
  [ChipWhisperer](http://www.newae.com/tools/chipwhisperer)
  target board, such as that for the
  [ST stm32f0](http://store.newae.com/stm32f0-target-for-cw308-arm-cortex-m0-128kb-flash-8kb-sram) (an ARM Cortex-M0).
  Those currently supported are
   
  - [Atmel atmega328p](./target/atmega328p/README.md) (an AVR),
  - [NXP lpc812m101jdh16](./target/lpc812m101jdh16/README.md) (an ARM Cortex-M0+),
  - [NXP lpc1114fn28](./target/lpc1114fn28/README.md) (an ARM Cortex-M0),
    and
  - [NXP lpc1313fbd48](./target/lpc1313fbd48/README.md) (an ARM Cortex-M3).
   
Some associated resources supplement the host and target board designs.
Consider an example wherein

```
export TARGET="${SCALE_HW}/target/lpc1114fn28"
```

denotes the path to a particular target board implementation,
in this case `lpc1114fn28`:
   
1. For each target board, a form of 
   [Board Support Package (BSP)](http://en.wikipedia.org/wiki/Board_support_package)
   is included to make development easier: this includes at least
      
   - support for the compilation, linkage, and execution of bare-metal 
     C programs,
   - a target board library, abstracting details of initialisation and
     interaction with relevant peripheral devices.
      
   You can build the BSP as follows:
      
   1. fix the working directory
      
      ```
      cd ${TARGET}
      ```
      
   2. build 
      the BSP
  
      ```
      make --no-builtin-rules clean all
      ```
      
2. A generic demonstration or 
   "[hello world](http://en.wikipedia.org/wiki/"Hello,_World!"_program)" 
   example is included, which uses the BSP:
      
   1. build the target board BSP as above
      
   2. fix the working directory
      
      ```
      cd ${SCALE_HW}/example
      ```
      
   3. build, program, then execute 
      the example
      
      ```
      make --no-builtin-rules -f ${TARGET}/build/lib/scale.mk BSP="${TARGET}/build" USB="/dev/ttyUSB0" PROJECT="example" clean all program
      ```
      
   Once executing, you *should* find that
      
   - the LED attached to `DUT_TRG` will flash on and off,
   - the LED attached to `DUT_GPO` is         on or  off 
     depending on whether or not the switch attached to `DUT_GPI`
     is pressed,
   - the string `hello world` is repeatedly written to the UART.

<!--- -------------------------------------------------------------------- --->

## FAQs

- **"Is there anything else similar I could look at?"**
  Yes: there are plenty of great resources available, some offering more
  direct alternatives to SCALE and some of more tangential interest.
  In particular, you should have a look at

  - [Side-channel Attack Standard Evaluation BOard (SASEBO)](http://www.risec.aist.go.jp/project/sasebo) (a wide-ranging project for side-channel attack evaluation platforms, focusing on FPGA-based targets),
  - [ChipWhisperer](http://www.newae.com/tools/chipwhisperer) (a mid-end solution from [NewAE Technology Inc.](http://www.newae.com)),
    plus associated projects such as [GlitchKit](http://github.com/glitchkit),
  - [HorrorScope](http://github.com/albert-spruyt/HorrorScope) (a ultra low-end solution, developed for [ScaCheapier workshop](http://events.ccc.de/congress/2018/wiki/index.php/Session:ScaCheapier) at 35c3),
  - [Generic Implementation ANalysis Toolkit (GIAnT)](https://sourceforge.net/projects/giant/) (a mid-end solution from [KAOS](http://kasper-oswald.de)),
  - [DPA WorkStation (DPAWS)](http://www.rambus.com/security/dpa-countermeasures/dpa-workstation-platform/) (a high-end solution from [Rambus/Cryptography Research Inc. (CRI)](http://www.rambus.com/security)),
  - [Inspector SCA](https://www.riscure.com/security-tools/inspector-sca) (a high-end solution from [Riscure](http://www.riscure.com)),
    and/or the associated
    [Pi&ntilde;ata](http://www.riscure.com/product/pinata-training-target) 
    development board.

  plus various more academic output such as [1,2,3,4,5,6].

- **"So if all *those* resources exist, I don't see the point of *this* resource?**
  Put simply, there are important differences in terms of the goals and
  remit.  SCALE is designed to suit *our* requirements, namely use in a 
  specific context (i.e., a lab. housing some workstations but no other
  specialist equipment), by specific users (i.e., a somewhat mixed, and
  so non-specialist cohort of students), and with a specific goal (i.e., 
  teaching skills and concepts through hands-on exploration).  

  Another way to look at it is that SCALE aims to permit *an* attack vs.
  *the* best attack possible: the target is known, and controlled st. it
  permits an attack to work *by default*.  Clearly this still leaves a
  gap to truly realistic attack scenarios, where the target is unknown
  and uncontrolled; mounting attacks against such targets, e.g., some
  sort of 
  [Commercial Off-The-Shelf (COTS)](http://en.wikipedia.org/wiki/Commercial_off-the-shelf)
  device, is outside the scope of SCALE

  All that means

  - if your goals or remit differ, then one of the resources above will
    be a *much* better option, 
    and so
  - making a direct comparison with those resources isn't so useful: it
    isn't fair to say one is better or worse, only that one is more or
    less suited to whatever *your* goal is.

- **"The documentation sucks"**
  Yes it does; sorry about that.

- **"Why so many through-hole (vs. surface-mount) components?"**
  - In some cases, the answer relates to  robustness.
    For example, the type B
    [USB](http://en.wikipedia.org/wiki/USB)
    connector has proven more robust than a micro-B alternative when
    subjected to (ab)use in "open access" lab. conditions.
  - In some cases, the answer relates to maintenance.
    For example, we started with target boards that include
    [DIP](http://en.wikipedia.org/wiki/Dual_in-line_package)-format
    micro-controllers housed in an associated socket because this made
    them easier to replace (if damaged, or bricked); this also made it
    marginally easier to move from a breadboard prototype to PCB.

- **"The component naming scheme is a bit annoying: why H.X and not just X, for example?"**
  If you only view the host and target boards in isolation, X clearly
  works just fine.  However, they are only useful in combination, and
  then it arguably becomes confusing: if X is a jumper, for example,
  you have to be careful to say "host board X" or "target board X".
  It might be uglier to use H.X (and T.X) vs. X, but at least it is 
  then unambiguous.

- **"How did you design/protype/manufacture the host and target boards?"**
  The project has evolved over quite some time, but the short answer is
  that two very typical phases were evident:
  a suite of 
  [breadboard](http://en.wikipedia.org/wiki/Breadboard)-based
  proof-of-concept prototypes were used initially, then, once they were 
  shown to be viable, focus switched to developing some
  [PCB](https://en.wikipedia.org/wiki/Printed_circuit_board)-based
  alternatives.  In detail,

  - a lot of the prototypes used SMT to DIP adaptors etc. from
    [ProtoAdvantage](http://www.proto-advantage.com),
    which aren't inexpensive, but *are* great quality,
  - the designs were produced using
    [Eagle](http://www.cadsoftusa.com),
  - the (online) BOMs are hosted at
    [Octopart](http://www.octopart.com),
  - the PCBs were generally manufactured by 
    [OSH Park](http://www.oshpark.com),
  - some stainless steel PCB stencils manufacture by 
    [OSH Stencils](http://www.oshstencils.com)
    were used during population of the PCBs,
  - the first revision was prototyped at
    [CircuitHub](https://www.circuithub.com),
    who also dealt with several bulk manufacturing orders

  *all* of which come *highly* recommended as a result, bar perhaps Eagle:
  in hindsight, using
  [KiCad](http://www.kicad.org)
  would have been preferable (given enough time to learn it).

<!--- -------------------------------------------------------------------- --->

## TODOs

- The design surrounding the two
  [SPST switches](http://en.wikipedia.org/wiki/Switch)
  could be improved (or generalised):

  - Both are currently fixed as active-low; it could be useful to allow 
    configuration this behaviour somehow, if a given target board needs 
    it.
  - The reset mechanism is motivated by 
    [Arduino UNO](http://www.arduino.cc/en/uploads/Main/arduino-uno-schematic.pdf),
    which allows `avrdude` to perform a "remote reset".  It'd clearly 
    be attractive to allow `lpc21isp` to do the same via `-control`,
    and, if possible, somehow allow both mechanisms to coexist.

- Originally, the plan was to include some mechanism for clock and/or
  power glitching; this was shelved, due 
  partly to a lack of time 
  and
  partly to a lack of design experience with FPGA components (which 
  are the obvious candidate to enable such functionality).
  It still seems possible to include this, maybe as a secondary board
  (e.g., a "control board" mounted under the host board): doing so is
  the (optimistic) purpose of the external `PSU` input.

- Various aspects of the BSP(s) warrant improvement:

  - Although the BSPs allow user configuration of the clock source and 
    frequency, they currently assume a fixed 
    UART (9600/8N1) 
    and 
    I2C  (100 kHz standard mode)
    configuration;
    it would obviously be useful to allow user configuration of both.
  - None of the BSPs do the "wake-up" process required by some of the 
    I2C peripherals: see, e.g., Section 6.1.1 of the
    [ATSHA204A](http://www.atmel.com/images/Atmel-8885-CryptoAuth-ATSHA204A-Datasheet.pdf)
    datasheet.  Clearly this would be a useful feature to include.
  - There is no specific library support for the I2C peripherals, i.e., 
    the specific functionality vs. generic I2C communication.  It would 
    be useful to wrap said functionality in an API on top of each BSP.
 
<!--- -------------------------------------------------------------------- --->

## References

1. D. Oswald.
   [Development of an Integrated Environment for Side Channel Analysis and Fault Injection](http://www.emsec.rub.de/media/crypto/attachments/files/2010/04/da_oswald.pdf).
   Diplomarbeit, Ruhr-Universit&auml;t Bochum, 2009.

2. M. Moukarzel.
   [&mu;Leech: A Side-Channel Evaluation Platform for Next Generation Trusted Embedded Systems](http://web.wpi.edu/Pubs/ETD/Available/etd-091015-155841/unrestricted/MichaelMoukarzelMSThesis.pdf).
   MSc Thesis, Worcester Polytechnic Institute, 2015.

3. R. Velegalati and J.-P. Kaps.
   [Towards a Flexible, Opensource BOard for Side-channel analysis (FOBOS)](https://cryptography.gmu.edu/fobos/download.php?docid=2169).
   In Cryptographic Architectures Embedded in Reconfigurable Devices (CRYPTARCHI), 2013.

4. R. Velegalati and J.-P. Kaps.
   [Introducing FOBOS: Flexible Open-source BOard for Side-channel analysis](http://cryptography.gmu.edu/fobos/download.php?docid=2020).
   In Constructive Side-Channel Analysis and Secure Design (COSADE), 2012.

5. J. Kim, K. Oh, D. Choi, and H. Kim.
   SCARF: profile-based Side Channel Analysis Resistant Framework.
   Security and Management International Conference (SAM), 2012.

6. L. Judge, M. Cantrell, C. Kendir, and P. Schaumont.
   [A Modular Testing Environment for Implementation Attacks](http://ieeexplore.ieee.org/document/6516434).
   In BioMedical Computing (BioMedCom), 86--95, 2012.

<!--- -------------------------------------------------------------------- --->
