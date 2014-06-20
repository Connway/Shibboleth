#!/bin/bash
premake5 $1 $2 $3 $4 $5

if [ -e Temp.sln ]; then
	rm Temp.sln
fi

if [ -e Makefile ]; then
	rm Makefile
fi
