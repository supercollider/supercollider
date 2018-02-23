#!/bin/sh
TARGET_ARCH="-arch armv7"
mkdir -p out

# ARCHS="armv2 armv2a armv3 armv3m armv4 armv4t armv5 armv5t armv5te armv6 armv6j armv6t2 armv6z armv7-a armv7-r ep9312"
ARCHS="armv2"
# armv7 armv7-m iwmmxt

touch empty.c
touch empty.S

gcc ${TARGET_ARCH} -E -dM empty.c | sort >out/c_default
gcc ${TARGET_ARCH} -E -dM empty.S | sort >out/a_default

rm -f out/diff_c out/diff_a

for a in ${ARCHS} ; do 
gcc ${TARGET_ARCH} -E -dM -march=${a} empty.c | sort >out/c_${a}
gcc ${TARGET_ARCH} -E -dM -march=${a} empty.S | sort >out/a_${a}
echo ${a} >>out/diff_c
echo ${a} >>out/diff_a
diff out/c_default out/c_${a} >>out/diff_c
diff out/a_default out/a_${a} >>out/diff_a
done

# armv3 is default.
