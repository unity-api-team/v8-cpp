#!/bin/bash

DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

export PATH=$DIR/depot_tools:"$PATH"

cd $DIR/v8

gclient sync

../gm.py x64.release
