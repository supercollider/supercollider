LanguageConfig {
	*store {|file|
		_LanguageConfig_writeConfigFile
		^this.primitiveFailed
	}

	*includePaths {
		_LanguageConfig_getIncludePaths
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
	}
	*postInlineWarnings_ {|aBoolean|
		_LanguageConfig_setPostInlineWarnings
		^this.primitiveFailed
	}
}
