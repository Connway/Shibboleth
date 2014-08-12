#!/bin/bash
m4 -D NS_FILE=$1 -D NS_NAMESPACE=$2 header_generator.m4 > src/$3/include/$4.h
