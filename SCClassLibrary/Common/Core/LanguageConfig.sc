LanguageConfig {
	*store {|file|
		_LanguageConfig_writeConfigFile
		^MethodError("Could not write to file %".format(file.asCompileString), this).throw
	}

	*currentPath {
		_LanguageConfig_getCurrentConfigPath
		^this.primitiveFailed
	}

	*includePaths {
		_LanguageConfig_getIncludePaths
		^this.primitiveFailed
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
	*excludeDefaultPaths {
		_LanguageConfig_getExcludeDefaultPaths
		^this.primitiveFailed
	}
	*excludeDefaultPaths_ {|aBoolean|
		_LanguageConfig_setExcludeDefaultPaths
		^this.primitiveFailed
	}
}
