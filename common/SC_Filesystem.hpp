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
 *  Singleton class that loads default directories if they
 *  are requested before being set.
 *  Uses boost::filesystem::path as its main interface for
 *  cross-platform compatibility and ease of notation.
 */

#ifndef SC_FILESYSTEM_HPP_INCLUDED
#define SC_FILESYSTEM_HPP_INCLUDED

#include <map> // map
#include <boost/filesystem/path.hpp> // path

class SC_Filesystem {
public:
	enum class DirName;
	typedef boost::filesystem::path Path;
	typedef std::map<DirName, Path> DirMap;
	struct Glob;

	enum class DirName {
		SystemAppSupport,
		SystemExtension,
		UserHome,
		UserAppSupport,
		UserExtension,
		UserConfig,
		Resource
	};

	// singleton
	static SC_Filesystem& instance()
	{
		static SC_Filesystem instance;
		return instance;
	}

	SC_Filesystem(SC_Filesystem const&) = delete;
	void operator=(SC_Filesystem const&) = delete;

	// Gets the path associated with the directory name.
	// The path is initialized if it is not already set.
	Path getDirectory(const DirName&);

	// Sets the path associated with the directory name
	inline void setDirectory(const DirName& dn, const Path& p)
	{
		mDirectoryMap[dn] = p;
	}

	// Expands a path starting with `~` to use the user's home directory.
	// Works cross-platform.
	Path expandTilde(const Path&);

	// Returns `true` if the directory is to be ignored during compilation.
	bool shouldNotCompileDirectory(const Path&) const;

	bool        shouldNotCompileDirectory(const Path& p) const;
	static bool isStandalone();

	// postconditions: isAlias is true if path is an alias, and false otherwise
	// returns: the resolved path if resolution occurred
	//          the original path if no resolution occurred
	//          Path() if resolution failed
	// @TODO: Could possibly be split into `isAlias` and `resolveAlias` to avoid
	// unnecessary copying
	static Path resolveIfAlias(const Path& p, bool& isAlias);

	// Returns empty path if end of stream is reached
	static Path globNext(Glob*);

	static void freeGlob(Glob*);

private:
	SC_Filesystem() {};

	static bool isNonHostPlatformDirectory(const std::string&);

	bool initDirectory(const DirName&);

	// Get default directories based on the OS's filesystem.
	// Returns an empty path to indicate failure.
	static Path defaultSystemAppSupportDirectory();
	static Path defaultSystemExtensionDirectory();
	static Path defaultUserHomeDirectory();
	static Path defaultUserAppSupportDirectory();
	static Path defaultUserExtensionDirectory();
	static Path defaultUserConfigDirectory();
	static Path defaultResourceDirectory();

	DirMap mDirectoryMap;
};

#endif // SC_FILESYSTEM_HPP_INCLUDED
