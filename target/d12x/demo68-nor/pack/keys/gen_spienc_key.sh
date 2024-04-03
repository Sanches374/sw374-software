#!/bin/bash

dd if=/dev/urandom of=spi_aes.key bs=16 count=1
xxd -i spi_aes.key > spi_aes_key.h
cp spi_aes_key.h ../../../../../bsp/examples_bare/test-efuse/spi_aes_key.h

