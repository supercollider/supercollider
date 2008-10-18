//Partitioned Convolution

PartConv : UGen
{
	*ar { arg in, fftsize, irbufnum, accumbufnum,mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, fftsize, irbufnum, accumbufnum).madd(mul, add);
	}
	
	*calcNumPartitions {arg fftsize, irbuffer;
		var siz, partitionsize;

		partitionsize=fftsize.div(2);
		
		siz= irbuffer.numFrames;
		^((siz/partitionsize).roundUp);
		//bufsize = numpartitions*fftsize;
	}
	
	*calcAccumSize {arg fftsize, irbuffer;
		^ fftsize* (PartConv.calcNumPartitions(fftsize,irbuffer));
	}
	
}


+ Buffer {

	preparePartConv { arg buf, fftsize;
		server.listSendMsg(["/b_gen", bufnum, "PreparePartConv", buf.bufnum, fftsize]);
	}

}