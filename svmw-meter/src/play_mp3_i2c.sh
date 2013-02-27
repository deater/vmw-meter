#!/bin/sh

# Usage:  ./play_mp3.sh FILE.mp3

# mpg321 /archive/music/mp3/vince/WeirdAl-AmishParadise.mp3 -s | ./music_demo.ascii blah | aplay -f cd
# mpg321 $1 -s | ./music_demo.ascii $1 | aplay -f cd

{ mpg321 $1 -s | ./music_demo.i2c $1 ;}  3>&1 1>&2 2>&3  | aplay -f cd