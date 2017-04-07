/*
 *  Copyright (c) 2017 Tim Walters. All rights reserved.
 *  Created by Brian Heim 2017-04-03.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

/*
 *  Filesystem utilities and properties for SuperCollider.
 *  Uses boost::filesystem::path as its main interface for
 *  cross-platform compatibility and ease of notation.
 */

#ifndef SC_FILESYSTEM_HPP_INCLUDED
#define SC_FILESYSTEM_HPP_INCLUDED

#include <boost/filesystem/path.hpp> // path

#if defined(__APPLE__) && !defined(SC_IPHONE)
#  include "SC_StandAloneInfo_Darwin.h"
#endif

enum class SC_DirectoryName;
class SC_Filesystem;

// Enumerated standard directory names used by SuperCollider
enum class SC_DirectoryName {
	SystemAppSupport,
	SystemExtension,
	UserHome,
	UserAppSupport,
	UserExtension,
	UserConfig,
	Resource
};

// @TODO: should probably make this a namespace instead?
// @TODO: determine whether more functions need to be added (read_dir, etc)
class SC_Filesystem {
public:
	typedef boost::filesystem::path Path;
	struct Glob;

	// Gets the path associated with the directory name.
	// The path is initialized if it is not already set.
	static Path getDirectory(const SC_DirectoryName&);

	// Sets the path associated with the directory name
	static void setDirectory(const SC_DirectoryName&, const Path&);

	// Expands a path starting with `~` to use the user's home directory.
	// Works cross-platform.
	static Path expandTilde(const Path&);

	// Returns `true` if the directory is to be ignored during compilation.
	static bool shouldNotCompileDirectory(const Path&);

	// Resolves a path if it is an alias (only affects macOS)
	// If it is an alias, the second argument is set to `true`.
	static Path resolveIfAlias(const Path&, bool&);

#if defined(__APPLE__) && !defined(SC_IPHONE)
	static const char* getBundleName();

	// Returns `true` if this is a standalone (only on macOS)
	static inline bool isStandalone() { 	return SC_StandAloneInfo::IsStandAlone();	}
#else
	static inline bool isStandalone() { return false; }
#endif // defined(__APPLE__) && !defined(SC_IPHONE)

	static Glob* makeGlob(const char*);

	// Returns empty path if end of stream is reached
	static Path globNext(Glob*);

	static void freeGlob(Glob*);

private:
	// singleton; @TODO: better to make this a namespace
	SC_Filesystem() = delete;
	~SC_Filesystem() = delete;
	SC_Filesystem(const SC_Filesystem&) = delete;
	SC_Filesystem(SC_Filesystem&&) = delete;
	SC_Filesystem& operator=(const SC_Filesystem&) = delete;

	static bool initDirectory(const SC_DirectoryName&);
};

#endif // SC_FILESYSTEM_HPP_INCLUDED
