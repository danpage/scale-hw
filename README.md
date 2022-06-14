# [SCALE](http://www.github.com/danpage/scale): hardware-oriented material

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

This repo. captures a suite of 
bespoke, hardware-based target platform designs:
when combined with suitable control and acquisition platforms, it offers
a real, albeit sanitised "kit" for learning about side-channel attacks.
It has a strict remit, namely educational use (e.g., as a lab. exercise 
or assignment), which to some extent determines goals, design decisions, 
and, ultimately, the content.

<!--- -------------------------------------------------------------------- --->

## Quickstart

- Install any pre-requisites.

- Clone the repo.

  ```sh
  git clone https://github.com/danpage/scale-hw.git ./scale-hw
  cd ./scale-hw
  git submodule update --init --recursive
  source ./bin/conf.sh
  ```

- Create and populate a suitable Python
  [virtual environment](https://docs.python.org/3/library/venv.html)
  based on 
  [`${REPO_HOME}/requirements.txt`](./requirements.txt) 
  by executing
   
  ```sh
  make venv
  ```
   
  then activate it by executing
   
  ```sh
  source ${REPO_HOME}/build/venv/bin/activate
  ``` 

- Select a target board, e.g., by setting the environment variable

  ```sh
  export TARGET="lpc1313fbd48"
  ```

- Build the material 
  by executing

  ```sh
  make --directory="${REPO_HOME}/src/scale/target/${TARGET}" build
  ```

- Make use of the material in

  ```sh
  ${REPO_HOME}/build/${TARGET}
  ```

<!--- -------------------------------------------------------------------- --->
