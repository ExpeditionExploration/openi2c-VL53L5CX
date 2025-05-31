# VL53L5CX driver NodeJS-interface

Datasheet: https://www.st.com/resource/en/datasheet/vl53l5cx.pdf

This standalone interface provides I2C driver bindings for
[OpenI2C](https://github.com/ExpeditionExploration/openi2c)

## About

VL53L5CX is a Time-of-Flight 8x8 multizone ranging sensor with wide field of
view.

This repo contains Node bindings to vendor C Linux user mode driver.

The bindings are written in C, and need to be compiled for them to work. There
are no pre-compiled binaries. Installing by installing OpenI2C should get you
going, but if you want this driver only, see *Building* for how to build the
binary for Node.


## Prerequisites

A C toolchain is needed for building, as well as a working Nodejs installation.

I2C bus needs to be enabled for your Linux, in case of Raspbian, using
`sudo raspi-config`. Also, working C toolchain is needed. In
Raspbian this can be installed by `sudo apt install build-essential`.


## Building

If, however, you want to build the bindings independently, you can

```bash
npm install
npm run build
```


## Usage

See the [examples](./examples/) directory.


## Missing features

Not all of the user mode part of the driver features have been implemented.
What is missing can be quite straighforwardly be compared by checking the
[examples](./examples/) directory files and the driver's own
[examples](./src/vl53l5cx-linux/user/examples/).


## Tested boards

This has been tested on:
- *Raspberry Pi 4B* using *Node v22.14.0*

