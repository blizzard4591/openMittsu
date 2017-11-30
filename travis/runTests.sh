#!/usr/bin/env bash

# set shell settings (see https://sipb.mit.edu/doc/safe-shell/)
set -eufv -o pipefail

# run tests
if [ "${TRAVIS_OS_NAME}" = "linux" ]
then
	./build/openMittsuTests
elif [ "${TRAVIS_OS_NAME}" = "osx" ]
then
	./build/openMittsuTests
fi
