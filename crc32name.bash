#!/bin/bash

x="$1"
crc32=`cko-multidigest "$x" | grep 'CRC32:' | awk '{print $2}'|tr '[a-z]' '[A-Z]'`

spacer='_'

if [ ! -z "$crc32" ]
then
  y="${x%.*}$spacer[$crc32].${x##*.}"
  echo $y
  mv "$x" "$y"
else
  echo "Unable to crc32 $x!"
fi


