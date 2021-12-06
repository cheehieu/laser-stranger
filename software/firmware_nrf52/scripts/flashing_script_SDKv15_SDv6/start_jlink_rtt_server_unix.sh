#!/bin/sh

# Start JLinkExe and connect to nRF52 target via SWD
# device/core: NRF52832_XXAA, target interface; S) SWD, speed: 4000 kHz
JLinkExe -device NRF52 -if SWD -speed 4000 -autoconnect 1

