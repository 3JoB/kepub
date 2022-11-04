#!/bin/bash

set -e

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  curl -fsSL https://github.com/3JoB/klib/releases/download/v1.25.2-Patch/klib-1.25.2-Linux.deb \
    -o klib.deb
  sudo dpkg -i klib.deb
else
  echo "The system does not support: $OSTYPE"
  exit 1
fi
