#!/bin/bash

DIR=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

cd $DIR

export PATH=./depot_tools:"$PATH"
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

cd v8
make -j3 native i18nsupport=off
