#!/bin/bash

readonly qmk_remote=qmkfirmware

# Add upstream QMK repo
if ! git remote get-url ${qmk_remote} &>/dev/null; then
  git remote add ${qmk_remote} https://github.com/qmk/qmk_firmware.git
fi

# Fetch upstream QMK
git fetch --prune ${qmk_remote}

# Rebase current branch on QMK's master
git rebase ${qmk_remote}/master

# Update all submodules
git submodule update --init --recursive

exit 0
