#!/bin/bash

git remote add qmkfirmware https://github.com/qmk/qmk_firmware.git 2>/dev/null
git fetch --prune qmkfirmware
git rebase qmkfirmware/master
git submodule update --init --recursive

exit 0
