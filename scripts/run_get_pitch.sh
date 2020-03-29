#!/bin/bash

# Put here the program (maybe with path)
GETF0="get_pitch"

if [ -z "$1" ]; then
    th1=0.7
else
    th1=$1
fi
if [ -z "$2" ]; then
    th2=0.3
else
    th2=$2
fi
if [ -z "$3" ]; then
    thPot=50.5
else
    thPot=$3
fi

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 $th1 $th2 $thPot> /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done

exit 0

