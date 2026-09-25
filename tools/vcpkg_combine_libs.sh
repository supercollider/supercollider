#!/usr/bin/env bash
set -euo pipefail

# This script merges two vcpkg triplets into a universal triplet for macOS
# It combines static and shared libraries, headers, and other resources
# note: it will attempt to combine all packages found in the first triplet

# environment variables:
# VCPKG_ROOT: root directory of vcpkg installation
# ARCH1: first triplet (e.g. x64-osx-release)
# ARCH2: second triplet (e.g. arm64-osx-release)
# ARCH_UNIVERSAL: output universal triplet (e.g. x64-arm64-osx-release)

# Ensure required environment variables are set
: "${VCPKG_ROOT:?VCPKG_ROOT must be set to the vcpkg installation root directory}"
: "${ARCH1:?ARCH1 must be set to the first triplet (e.g. x64-osx-release)}"
: "${ARCH2:?ARCH2 must be set to the second triplet (e.g. arm64-osx-release)}"
: "${ARCH_UNIVERSAL:?ARCH_UNIVERSAL must be set to the output universal triplet (e.g. x64-arm64-osx-release)}"

INSTALL_DIR="${VCPKG_ROOT}/installed"
OUT_DIR="${INSTALL_DIR}/${ARCH_UNIVERSAL}"
mkdir -p "$OUT_DIR"

echo -e "\nMerging packages from ${INSTALL_DIR}/${ARCH1} and ${INSTALL_DIR}/${ARCH2} into ${OUT_DIR}"

for pkg in $(ls "${INSTALL_DIR}/${ARCH1}/share"); do
  echo "Merging package: $pkg"
  pkg_lower=$(echo "$pkg" | tr '[:upper:]' '[:lower:]')

  PKG1="${INSTALL_DIR}/${ARCH1}"
  PKG2="${INSTALL_DIR}/${ARCH2}"
  PKG_UNI="${OUT_DIR}"

  # Merge static libraries
  for libtype in lib debug/lib; do
    if [[ -d "${PKG1}/${libtype}" && -d "${PKG2}/${libtype}" ]]; then
      mkdir -p "${PKG_UNI}/${libtype}"
      for lib1 in "${PKG1}/${libtype}/lib${pkg_lower}"*.a; do
        [[ -f "$lib1" ]] || continue
        libname=$(basename "$lib1")
        lib2="${PKG2}/${libtype}/${libname}"
        if [[ -f "$lib2" ]]; then
          echo "  - [static] $libname"
          lipo -create "$lib1" "$lib2" -output "${PKG_UNI}/${libtype}/$libname"
        fi
      done
    fi
  done

  # Merge shared libraries
  for libtype in lib debug/lib; do
    if [[ -d "${PKG1}/${libtype}" && -d "${PKG2}/${libtype}" ]]; then
      mkdir -p "${PKG_UNI}/${libtype}"
      for dylib1 in "${PKG1}/${libtype}/lib${pkg_lower}"*.dylib; do
        [[ -f "$dylib1" ]] || continue
        libname=$(basename "$dylib1")
        dylib2="${PKG2}/${libtype}/${libname}"
        if [[ -f "$dylib2" ]]; then
          echo "  - [shared] $libname"
          lipo -create "$dylib1" "$dylib2" -output "${PKG_UNI}/${libtype}/$libname"
          install_name_tool -id "@rpath/$libname" "${PKG_UNI}/${libtype}/$libname"
        fi
      done
    fi
  done

  # Copy headers
  if [[ -d "${PKG1}/include" ]]; then
      echo "  - Copying headers"
      cp -a "${PKG1}/include" "${PKG_UNI}/"
  fi

  # Copy share/ usage & config files
  if [[ -d "${PKG1}/share/${pkg_lower}" ]]; then
      echo "  - Copying share/${pkg_lower}"
      mkdir -p "${PKG_UNI}/share"
      cp -a "${PKG1}/share/${pkg_lower}" "${PKG_UNI}/share/"
  fi

  # Copy tools if needed
  if [[ -d "${PKG1}/tools/${pkg_lower}" ]]; then
      echo "  - Copying tools/${pkg_lower}"
      mkdir -p "${PKG_UNI}/tools"
      cp -a "${PKG1}/tools/${pkg_lower}" "${PKG_UNI}/tools/"
  fi
done

echo "Finished merging into: ${OUT_DIR}"
