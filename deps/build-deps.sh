#!/bin/bash

DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

export PATH=$DIR/depot_tools:"$PATH"

cd $DIR/v8

gclient config --spec 'solutions = [
  { "name"        : "v8",
    "url"         : "https://chromium.googlesource.com/v8/v8.git",
    "deps_file"   : "DEPS",
    "managed"     : False,
    "custom_deps" : {
    },
    "safesync_url": "",
  },
]
cache_dir = None'

gclient sync

../gm.py x64.release
