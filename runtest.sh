#!/bin/bash
if [ ! -f "test/test.tm" ]; then
    if [ ! -f "cminus" ]; then
        make
    fi
    ./cminus test/test.cm
fi
./tm test/test.tm