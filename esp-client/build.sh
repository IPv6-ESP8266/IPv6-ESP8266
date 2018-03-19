#!/usr/bin/bash

export PATH=$HOME/workspace/esp-open-sdk/xtensa-lx106-elf/bin:$PATH
export SDK_PATH=$HOME/workspace/ESP8266_RTOS_SDK
export BIN_PATH=$SDK_PATH/bin


#make COMPILE=gcc BOOT=new APP=1 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=0
./gen_misc.sh
