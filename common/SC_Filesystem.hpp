/**
 *  \file SC_Filesystem.hpp
 *
 *  \brief Filesystem utilities for SuperCollider.
 *
 *  This header provides a singleton class, SC_Filesystem, which handles many
 *  important filesystem functions:
 *  - Getting and setting application directories (such as config, resource, and extension)
 *  - Expanding ~ to the home directory
 *  - Alias resolution
 *  - Globbing
 *  - Conversion between Path and UTF-8 encoded string
 *
 *  \note This header replaced an older implementation by Tim Walters, SC_DirUtils.hpp. Most of the globbing and
 *        directory calculation code is his.
 *
 *  $Authors: Brian Heim, Tim Walters $
 *
 *  \version Latest revision by Brian Heim, 2017
 *
 *  $Date: 2017-04-30 $
 *
 *  $Contact: brianlheim@gmail.com $
 *
 *  Created on: 2017-04-03
 *
 *  Original revision by Tim Walters, 2005-10-19.
 */
/*
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

#ifndef SC_FILESYSTEM_HPP_INCLUDED
#define SC_FILESYSTEM_HPP_INCLUDED

#define DEBUG_SCFS

/// Name of the folder used for system and user extensions.
#define SC_FOLDERNAME_EXTENSIONS "Extensions"

#include <map> // map
#include <boost/filesystem/path.hpp> // path

/** \class SC_Filesystem
 *
 *  \brief A singleton class providing SuperCollider filesystem utilities.
 *
 *  - Getting and setting application directories (such as config, resource, and extension)
 *  - Expanding ~ to the home directory
 *  - Alias resolution
 *  - Globbing
 *  - Conversion between Path and UTF-8 encoded string
 */
class SC_Filesystem {
public:
	enum class DirName;
	struct     Glob;

	typedef boost::filesystem::path Path;   ///< Path type.
	typedef std::map<DirName, Path> DirMap; ///< Type of directory name-to-path map.

	/// SuperCollider common directory names.
	enum class DirName {
		SystemAppSupport,
		SystemExtension,
		UserHome,
		UserAppSupport,
		UserExtension,
		UserConfig,
		Resource
	};

	/// Singleton instance.
	static SC_Filesystem& instance()
	{
		static SC_Filesystem instance;
		return instance;
	}

	/// Get path associated with a common directory; the path is initialized if necessary.
	Path getDirectory(const DirName& dn)
	{
		const DirMap::const_iterator& it = mDirectoryMap.find(dn);
		if (it != mDirectoryMap.end()) {
			return it->second;
		}
		return mDirectoryMap[dn] = defaultDirectory(dn);
	}

	/// Set path associated with a common directory.
	inline void setDirectory(const DirName& dn, const Path& p) { mDirectoryMap[dn] = p; }

	/// Expands a path starting with `~` to use the user's home directory.
	Path expandTilde(const Path& p)
	{
		static const Path tilde("~");
		Path::const_iterator piter = p.begin();
		if (piter != p.end() && *piter == tilde) {
			Path expanded = getDirectory(DirName::UserHome);
			while (++piter != p.end())
				expanded /= *piter; // lexically_relative would expand "~" to "$HOME/." when we just want "$HOME"
			return expanded;
		} else {
			return p;
		}
	}

	// @TODO: refactor w/ lang config
	/** \brief Checks whether a directory should be compiled.
	  * \param p a directory path
	  * \return True if the directory should not be traversed during compilation.
	  *
	  * Specifically, returns true if the directory name is:
	  * - one of ".svn", ".git", or "_darcs"
	  * - one of "help" or "ignore", ignoring case
	  * - the string composed of "scide_" and a string representing the current IDE name
	  * - one of "windows", "osx", "iphone", or "linux", but that is not the name of the current platform.
	  */
	bool shouldNotCompileDirectory(const Path& p) const;

	/// Returns true if, on macOS, SuperCollider is operating as a standalone application.
	static bool isStandalone();

	/** \brief Converts a Path to a UTF-8 encoded string.
	  * 
	  * On Windows, conversion is done using the std::codecvt_utf8<wchar_t> facet.
	  * On all other platforms, this just calls .string(). */
	static std::string pathAsUTF8String(const Path&);

	/** \brief Resolves an alias on macOS.
	  * \param p a path to resolve
	  * \param isAlias set to true if p is an alias
	  * \return An empty path if resolution failed; otherwise, the resolved path.
	  * 
	  * If the path was not an alias, a copy is returned. */
	// @TODO: Could possibly be split into `isAlias` and `resolveAlias` to avoid
	// unnecessary copying
	static Path resolveIfAlias(const Path& p, bool& isAlias);

	/** \brief Makes a glob.
	  *
	  * Constructs a Glob from a given pattern. Call globNext to get the next
	  * Path in the glob. Client is responsible for calling freeGlob. */
	static Glob* makeGlob(const char* pattern);

	/// Next available Path in the glob. Returns empty path to indicate end.
	static Path globNext(Glob* g);

	/// Frees a Glob.
	static void freeGlob(Glob* g);

private:
	SC_Filesystem(SC_Filesystem const&) = delete;
	void operator=(SC_Filesystem const&) = delete;
	SC_Filesystem() {};

	/** Of the four strings "windows", "osx", "linux", "iphone", returns true if `p` is one of the
	 * three that doesn't correspond to this platform. */
	static bool isNonHostPlatformDirectory(const std::string& p);

	Path defaultDirectory(const DirName& dn)
	{
		switch (dn) {
			case DirName::SystemAppSupport:
				return defaultSystemAppSupportDirectory();
			case DirName::SystemExtension:
				return defaultSystemExtensionDirectory();
			case DirName::UserHome:
				return defaultUserHomeDirectory();
			case DirName::UserAppSupport:
				return defaultUserAppSupportDirectory();
			case DirName::UserExtension:
				return defaultUserExtensionDirectory();
			case DirName::UserConfig:
				return defaultUserConfigDirectory();
			case DirName::Resource:
				return defaultResourceDirectory();
			default:
				return Path();
		}
	}
	
	static Path defaultSystemAppSupportDirectory();
	static Path defaultUserHomeDirectory();
	static Path defaultUserAppSupportDirectory();
	static Path defaultUserConfigDirectory();
	static Path defaultResourceDirectory();

	static Path defaultSystemExtensionDirectory()
	{
		const Path& p = defaultUserAppSupportDirectory();
		return p.empty() ? p : p / SC_FOLDERNAME_EXTENSIONS;
	}

	static Path defaultUserExtensionDirectory()
	{
		const Path& p = defaultUserAppSupportDirectory();
		return p.empty() ? p : p / SC_FOLDERNAME_EXTENSIONS;
	}

	DirMap mDirectoryMap;
};

#endif // SC_FILESYSTEM_HPP_INCLUDED
