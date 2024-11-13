#!/bin/bash
# clump tests into $NCLUMPS jobs
#
# Demo:
# $ NCLUMPS=5 GITHUB_OUTPUT=gh ./setup_test_clumps_output.sh
# $ cat gh
# n-clumps=5
# test-clumps-matrix=[0,1,2,3,4]

set -euo pipefail


if [ $NCLUMPS -lt 1 ]; then
  echo "Clumps must be positive"
  exit 1
fi
echo "Setting output: n-clumps=$NCLUMPS"
echo "n-clumps=$NCLUMPS" >> $GITHUB_OUTPUT
TEST_CLUMPS='[0'
for (( i=1; i<NCLUMPS; i++ )) ; {
  TEST_CLUMPS="${TEST_CLUMPS},${i}"
}
TEST_CLUMPS="${TEST_CLUMPS}]"
echo "Setting output: test-clumps-matrix=${TEST_CLUMPS}"
echo "test-clumps-matrix=${TEST_CLUMPS}" >> $GITHUB_OUTPUT
