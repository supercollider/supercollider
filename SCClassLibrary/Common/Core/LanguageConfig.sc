LanguageConfig {
	*store {|file|
		_LanguageConfig_writeConfigFile
		^this.primitiveFailed
	}

	*currentPath {
		_LanguageConfig_getCurrentConfigPath
		^this.primitiveFailed
	}

	*currentDirectory {
		_LanguageConfig_getCurrentConfigDirectory
		^this.primitiveFailed
	}

	*prMakeAbsoluteToProjectRoot { |paths|
		var root;
		^if( LanguageConfig.projectOpen ) {
			root = this.currentDirectory;
			paths.collect{ |p|
				if( PathName(p).isRelativePath ) {
					root +/+ p
				} {
					p
				}
			}
		} {
			paths
		}
	}

	*includePaths {
		_LanguageConfig_getIncludePaths
		^this.primitiveFailed
	}
	*includePathsAbsolute {
		^this.prMakeAbsoluteToProjectRoot(this.includePaths)
	}
	*addIncludePath {|aPath|
		_LanguageConfig_addIncludePath
		^this.primitiveFailed
	}
	*removeIncludePath {|aPath|
		_LanguageConfig_removeIncludePath
		^this.primitiveFailed
	}

	*excludePaths {
		_LanguageConfig_getExcludePaths
		^this.primitiveFailed
	}
	*excludePathsAbsolute {
		^this.prMakeAbsoluteToProjectRoot(this.excludePaths)
	}
	*addExcludePath {|aPath|
		_LanguageConfig_addExcludePath
		^this.primitiveFailed
	}
	*removeExcludePath {|aPath|
		_LanguageConfig_removeExcludePath
		^this.primitiveFailed
	}

	*postInlineWarnings {
		_LanguageConfig_getPostInlineWarnings
		^this.primitiveFailed
	}
	*postInlineWarnings_ {|aBoolean|
		_LanguageConfig_setPostInlineWarnings
		^this.primitiveFailed
	}

	*projectOpen {
		_LanguageConfig_getProjectOpen
		^this.primitiveFailed
	}

	*defaultPathsExcluded {
		_LanguageConfig_getExcludeDefaultPaths
		^this.primitiveFailed
	}
}
