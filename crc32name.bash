#!/bin/bash

x="$1"
crc32=`cko-multidigest "$x" | grep 'CRC32:' | awk '{print $2}'|tr '[a-z]' '[A-Z]'`


y="${x%.*} [$crc32].${x#*.}"
mv "$x" "$y"

