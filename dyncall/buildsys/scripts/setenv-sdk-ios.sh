#!/bin/sh
#export PATH=`xcode-select -print-path`/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin:${PATH}
DEVELOPER=`xcode-select -print-path`
# WORKS: P="${DEVELOPER}/Platforms/iPhoneOS.platform/Developer"
P="${DEVELOPER}/Toolchains/XcodeDefault.xctoolchain"
export MANPATH="$P/usr/share/man:${MANPATH}"
export PATH="$P/usr/bin:${PATH}"

#if [ -z "${DEVELOPER}" ]; then
#DEVELOPER=/Developer
#fi
#export PATH="${DEVELOPER}/Platforms/iPhoneOS.platform/Developer/usr/bin:${PATH}"
# export CPATH="$P/usr/include:${CPATH}"
#if [ `exec sw_vers -productVersion` ]; then
#  case `sw_vers -productVersion` in
#    10.*)
#       
#      ;;
#  esac
#export PATH=${DEVELOPER}/Platforms/iPhoneOS.platform/Developer/usr/bin:${PATH}


