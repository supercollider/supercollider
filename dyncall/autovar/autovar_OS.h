/*

 Package: dyncall
 Library: autovar
 File: autovar/autovar_OS.h
 Description: 
 License:

   Copyright (c) 2011-2015 Daniel Adler <dadler@uni-goettingen.de>,
                           Tassilo Philipp <tphilipp@potion-studios.com>

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/

#ifndef AUTOVAR_OS_H
#define AUTOVAR_OS_H

#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#  define OS_Win64
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__WINDOWS__) || defined(_WINDOWS)
#  define OS_Win32
#elif defined(__APPLE__) || defined(__Darwin__)
#  define OS_Darwin
#  if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#    define OS_IOS
#  else /* defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) */
#    define OS_MacOSX
#  endif
#elif defined(__linux__) || defined(__linux) || defined(__gnu_linux__)
#  define OS_Linux
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) /* latter is (also) used by systems using FreeBSD kernel, e.g. Debian/kFreeBSD, which could be detected specifically by also checking for __GLIBC__ */
#  define OS_FreeBSD
#elif defined(__OpenBSD__)
#  define OS_OpenBSD
#elif defined(__NetBSD__)
#  define OS_NetBSD
#elif defined(__DragonFly__)
#  define OS_DragonFlyBSD
#elif defined(__sun__) || defined(__sun) || defined(sun)
#  define OS_SunOS
#elif defined(__CYGWIN__)
#  define OS_Cygwin
#elif defined(__MINGW__)
#  define OS_MinGW
#elif defined(__nds__)
#  define OS_NDS
#elif defined(__psp__) || defined(PSP)
#  define OS_PSP
#elif defined(__HAIKU__) || defined(__BEOS__)
#  define OS_BeOS
#elif defined(Plan9) || defined(__Plan9__)
#  define OS_Plan9
#elif defined(__vms)
#  define OS_VMS
#elif defined(__minix)
#  define OS_Minix
#else
#  define OS_Unknown
#endif

/** Platforms. */

#if defined(__ANDROID__)
#  define OS_Android
#endif

#endif /* AUTOVAR_OS */

