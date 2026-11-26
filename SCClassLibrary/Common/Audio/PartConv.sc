//Partitioned Convolution

PartConv : UGen {
	*ar { |in, fftsize, irbufnum, mul = 1.0, add = 0.0|
		^this.multiNew('audio', in, fftsize, irbufnum).madd(mul, add)
	}

	*calcNumPartitions { |fftsize, irbuffer|
		var siz, partitionsize;

		partitionsize = fftsize.div(2);

		siz = irbuffer.numFrames;
		^((siz / partitionsize).roundUp)
		//bufsize = numpartitions*fftsize;
	}

	*calcBufSize { |fftsize, irbuffer|
		^ fftsize * (PartConv.calcNumPartitions(fftsize, irbuffer))
	}

}


+ Buffer {

	preparePartConv { |buf, fftsize|
		server.listSendMsg(["/b_gen", bufnum, "PreparePartConv", buf.bufnum, fftsize])
	}

}
