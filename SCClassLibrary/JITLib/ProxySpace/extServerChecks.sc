+ Server {
	checkBlockSize { |blockSize, label=""|
		// allow nil to clear settings, dont reblock when same blocksize
		case { blockSize.isNil or: { blockSize == this.options.blockSize }} {
			^true
		} { blockSize.isNumber.not } {
			"%: blockSize % is not a number.".postf(label, blockSize);
			^false
		} { blockSize.isPowerOfTwo.not } {
			"%: blockSize % is not a power of two.".postf(label, blockSize);
			^false
		} { blockSize.inclusivelyBetween(1, this.options.blockSize).not } {
			"%: blockSize % is outside range [1, %]."
			.postf(label, blockSize, this.options.blockSize);
			^false
		};
		^true
	}
	checkUpsample { |upsample, label=""|
		// allow nil to clear settings, dont reblock when 1
		case { upsample.isNil { upsample == 1 } } {
			^true
		}  { upsample.isNumber.not } {
			"%: upsample % is not a number.".postf(label, upsample);
			^false
		} { upsample.isPowerOfTwo.not } {
			"%: upsample % is not a power of two.".postf(label, upsample);
			^false
		} { upsample.inclusivelyBetween(1, 256).not } {
			"%: upsample % is outside range [1, %]."
			.postf(label, upsample, this.options.blockSize);
			^false
		};
		^true
	}
}
