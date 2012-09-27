/*
 *  Copyright 2003 Maurizio Umberto Puxeddu <umbpux@tin.it>
 *  Copyright 2011 Jakob Leben
 *
 *  This file is part of SuperCollider.
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 *  USA
 *
 */

#ifndef SC_LANGUAGECONFIG_HPP_INCLUDED
#define SC_LANGUAGECONFIG_HPP_INCLUDED

#include <vector>
#include <string>

class SC_LanguageConfig
{
public:
	typedef std::vector<std::string> DirVector;
	SC_LanguageConfig();

	const DirVector& includedDirectories() { return mIncludedDirectories; }
	const DirVector& excludedDirectories() { return mExcludedDirectories; }

	void postExcludedDirectories(void);
	bool forEachIncludedDirectory(bool (*func)(const char *, int));

	bool pathIsExcluded(const char *path);

	void addIncludedDirectory(const char *name);
	void addExcludedDirectory(const char *name);
	void removeIncludedDirectory(const char *name);
	void removeExcludedDirectory(const char *name);

	// convenience functions to access the global library config
	static bool readLibraryConfigYAML(const char* fileName);
	static bool writeLibraryConfigYAML(const char* fileName);
	static void freeLibraryConfig();
	static bool defaultLibraryConfig(void);
	static bool readDefaultLibraryConfig();

private:
	DirVector mIncludedDirectories;
	DirVector mExcludedDirectories;
	DirVector mDefaultClassLibraryDirectories;
};

extern SC_LanguageConfig* gLanguageConfig;

#endif // SC_LANGUAGECONFIG_HPP_INCLUDED
