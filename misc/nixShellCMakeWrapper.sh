#!/bin/bash

/nix/store/3af6g226v4hsv6x7xzh23d6wqyq0nzjp-nix-2.10.3/bin/nix-shell -p gcc --run "cmake $@"