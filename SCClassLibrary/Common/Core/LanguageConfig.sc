LanguageConfig {
	*store {|file|
		_LanguageConfig_writeConfigFile
		^this.primitiveFailed
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
}
