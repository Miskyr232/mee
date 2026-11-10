
#!/bin/bash

set -e

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DMEE_BUILD_TESTS=ON \
    -DMEE_BUILD_EXAMPLES=ON

cmake --build build -j$(nproc)
