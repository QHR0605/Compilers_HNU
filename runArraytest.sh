#!/bin/bash
if [ ! -f "test/Arraytest.tm" ]; then
	if [ ! -f "cminus" ]; then
		make
	fi
	./cminus test/Arraytest.cm
fi
./tm test/Arraytest.tm

