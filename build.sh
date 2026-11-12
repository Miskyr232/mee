
#!/bin/bash

set -e

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DMEE_BUILD_TESTS=ON \
    -DMEE_BUILD_EXAMPLES=ON \
    -DCMAKE_INSTALL_PREFIX=~/.local

cmake --build build -j$(nproc)
cmake --install build
