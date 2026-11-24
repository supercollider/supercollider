/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

//! @file SC_APIVersion.h
//! @brief This file implements SC's plugin API versioning.
//! For more information, see the README.md

//! @brief Turn an API version tuple into a 16-bit hexadecimal version code.
//! The upper byte contains the major version, the lower byte the minor version.
//! For example, API version 4.1 is encoded as 0x0401.
#define SC_MAKE_API_VERSION(major, minor) (((major) << 8) | ((minor)&0xFF))

//! @brief The major API version.
//! @note For SC developers only!
//! SC developers must increment this in case of an ABI break. Try to avoid if possible!
//! A Server must only load plugins with the same major API version.
//!
//! @todo The plugin API version has been temporarily bumped to avoid potential crashes
//! with existing plugins on the develop branch. After all plugin API changes have been
//! made for SC 3.15, the major API version should be restored to 4.
#define SC_API_VERSION_MAJOR 8

//! @brief The minor API version.
//! @note For SC developers only!
//! SC developers must increment this whenever they add new members to one or more versioned structs.
#define SC_API_VERSION_MINOR 0

//! @brief The plugin API version code.
//! @see SC_MAKE_API_VERSION for the version code format.
#define SC_API_VERSION SC_MAKE_API_VERSION(SC_API_VERSION_MAJOR, SC_API_VERSION_MINOR)

//! @brief Plugins: request a specific *minimum* required API version.
//! The version is provided as a hexadecimal version code, @see SC_MAKE_API_VERSION().
//!
//! @note Make sure to set this for all your headers and sources, ideally via the build system.
//! For example:
//! `-DSC_MIN_REQUIRED_API_VERSION=0x0402`
//! or
//! `-DSC_MIN_REQUIRED_API_VERSION=SC_MAKE_API_VERSION(4, 2)`
//!
//! Any fields that have been added up to (and including) SC_MIN_REQUIRED_API_VERSION are always
//! accessible to the plugin. Trying to access any later fields gives a compile time error.
//! A plugin shall only be loaded if the exported SC_MIN_REQUIRED_API_VERSION is less than or
//! equal to the Server's own API version.
//!
//! The default value is rather conservative, but suitable for most plugins. Plugin authors
//! are advised to only set it as high as necessary so that their plugin can still be loaded
//! in older Server versions.
#ifndef SC_MIN_REQUIRED_API_VERSION
#    define SC_MIN_REQUIRED_API_VERSION SC_MAKE_API_VERSION(4, 0)
#endif

#if SC_MIN_REQUIRED_API_VERSION > SC_API_VERSION
#    error SC_API_VERSION does not fullfill the minimum requirements.                                                 \
           You might need to build against newer API headers.
#endif

//! @brief Plugins: enable all API extensions.
//!
//! Normally, any fields that have been added *after* <SC_MIN_REQUIRED_API_VERSION> are not
//! accessable to the plugin. Setting SC_ENABLE_ALL_API_EXTENSIONS to 1 enables these fields.
//! However, since some of the fields might not be actually implemented yet, they must be checked
//! at runtime with the SC_IMPLEMENTS_FIELD macro.
//!
//! Usually, plugin author should only enable this option if they want to use certain (optional)
//! fields from more recent API versions while staying compatible with older API versions.
//! In that case, they would set SC_MIN_REQUIRED_API_VERSION to the oldest API version they want
//! to support and explicitly check any fields that have been added in subsequent versions.
//!
//! Disabled by default for safety reasons.
#ifndef SC_ENABLE_ALL_API_EXTENSIONS
#    define SC_ENABLE_ALL_API_EXTENSIONS 0
#endif

//! @brief A macro for marking plugin API extensions.
//! @note For SC developers only!
//! All struct members added in a particular SC version must be wrapped in a SC_API_EXTENSION(X, Y)
//! clause, where X is the current major API version and Y is the *new* minor API version.
//!
//! Hypothetical example:
//! @todo Replace this with an actual example once we add the first extension.
//! @code{.c}
//! struct Unit {
//!     size_t mStructSize;
//!     World* mWorld;
//!     // etc.
//!     int32 mBufLength;
//! #if SC_API_EXTENSIONS(4, 1)
//!     int32 mFoo;
//!     int32 mBar;
//! #endif
//! };
//! @endcode
//!
//! @note Whenever the *major* API version is bumped, you can remove all SC_API_EXTENSION()
//! clauses because the fields have now become all mandatory.
#if defined(BUILDING_SCSYNTH) || SC_ENABLE_ALL_API_EXTENSIONS
#    define SC_API_EXTENSION(major, minor) 1
#else
#    define SC_API_EXTENSION(major, minor) (SC_MAKE_API_VERSION((major), (minor)) <= SC_MIN_REQUIRED_API_VERSION)
#endif

//! @defgroup Versioned_structs Versioned Structs
//!
//! A 'versioned struct' is just a regular C struct whose first member (`mStructSize`)
//! contains the (implemented) struct size. This way the struct user can check at runtime
//! whether certain (optional) fields are actualy implemented.
//!
//! Currently, the following public structs are versioned:
//! - InterfaceTable
//! - World
//! - Unit
//! - Graph
//! - SndBuf
//!
//! @{

//! @brief Calculates the implemented size of a versioned struct.
//! @param structName   The name of the struct
//! @param memberName   The name of the *last* implemented member in that struct
//!
//! This macro should be used for initializing the `mStructSize` member of a versioned struct.
//! Currently, all public (versioned) structs are provided by the Server, so this is macro is never
//! used by plugins. This might change in the future once plugins also create and pass versioned
//! structs to the Server.
//!
//! @attention Do **not** use sizeof() because the actual value might change accross API versions.
//! Also, sizeof() may include padding bytes, in which case it could wrongly indicate the presence
//! of a member that actually does not exist.
//!
//! Example:
//! @code{.c}
//! struct Unit {
//!     size_t mStructSize;
//!     World* mWorld;
//!     // etc.
//!     int32 mBufLength;
//! };
//!
//! // Server:
//! unit->mStructSize = SC_IMPLEMENTED_STRUCT_SIZE(Unit, mBufLength);
//! @endcode
#if defined(__cplusplus)
#    define SC_IMPLEMENTED_STRUCT_SIZE(structName, memberName)                                                         \
        (offsetof(structName, memberName) + sizeof(static_cast<structName*>(nullptr)->memberName))
#else
#    define SC_IMPLEMENTED_STRUCT_SIZE(StructType, memberName)                                                         \
        (offsetof(StructType, memberName) + sizeof(((StructType*)NULL)->memberName))
#endif

//! @brief Check whether a versioned struct implements a particular field.
//! @param pointerToStruct  Pointer to a struct instance
//! @param structName       The name of the struct
//! @param memberName       The name of the member to check
//!
//! This is only necessary for optional fields, i.e. those that have been introduced after
//! SC_MIN_REQUIRED_API_VERSION and enabled with SC_ENABLE_ALL_API_EXTENSIONS.
//!
//! Hypothetical example:
//! @todo Replace this with an actual example once we add the first extension.
//! @code{.c}
//! struct Unit {
//!     size_t mStructSize;
//!     World* mWorld;
//!     // etc.
//!     int32 mBufLength;
//! #if SC_API_EXTENSIONS(4, 1)
//!     int32 mFoo;
//! #endif
//! };
//!
//! #define SC_ENABLE_ALL_API_EXTENSIONS 1
//!
//! // check mFoo before using it
//! if (SC_IMPLEMENTS_FIELD(unit, Unit, mFoo))
//!     unit->mFoo = 0;
//! @endcode
#define SC_IMPLEMENTS_FIELD(pointerToStruct, structName, memberName)                                                   \
    ((pointerToStruct)->mStructSize > offsetof(structName, memberName))

//! @}
