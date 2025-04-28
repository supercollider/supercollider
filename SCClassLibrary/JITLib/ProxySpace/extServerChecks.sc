+ Server {
	checkBlockSize { |blockSize, label=""|
		// allow nil to clear settings
		case { blockSize.isNil } {
			^true
		} { blockSize.isNumber.not } {
			"%: blockSize % is not a number - not setting.".postf(label, blockSize);
			^false
		} { blockSize.isPowerOfTwo.not } {
			"%: blockSize % is not a power of two - not setting.".postf(label, blockSize);
			^false
		} { blockSize.inclusivelyBetween(1, this.options.blockSize).not } {
			"%: blockSize % is outside range [1, %] - not setting."
			.postf(label, blockSize, this.options.blockSize);
			^false
		};
		^true
	}
	checkUpsample { |upsample, label=""|
		// allow nil to clear settings
		case { upsample.isNil } {
			^true
		}  { upsample.isNumber.not } {
			"%: upsample % is not a number - not setting.".postf(label, upsample);
			^false
		} { upsample.isPowerOfTwo.not } {
			"%: upsample % is not a power of two - not setting.".postf(label, upsample);
			^false
		} { upsample.inclusivelyBetween(1, 256).not } {
			"%: upsample % is outside range [1, %] - not setting."
			.postf(label, upsample, this.options.blockSize);
			^false
		};
		^true
	}
}
