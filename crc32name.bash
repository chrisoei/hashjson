#!/bin/bash

x="$1"
crc32=`cko-multidigest "$x" | grep 'CRC32:' | awk '{print $2}'|tr '[a-z]' '[A-Z]'`

if [ -z "$2" ]
then
  if [[ $x =~ ' ' ]]
  then
    spacer=' '
  else
    spacer='_'
  fi
else
  spacer="$2"
fi

if [ ! -z "$crc32" ]
then
  y="${x%.*}$spacer[$crc32].${x##*.}"
  echo $y
  mv "$x" "$y"
else
  echo "Unable to crc32 $x!"
fi


