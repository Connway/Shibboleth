#!/bin/bash
premake5 "$@"

if [ -e Temp.sln ]; then
	rm Temp.sln
fi

if [ -e Makefile ]; then
	rm Makefile
fi
