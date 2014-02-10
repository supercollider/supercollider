Download : QObject {
	classvar requests;
	var <>finishedFunc, <>errorFunc, <>progressFunc;
	var error = false, started = false;

	*new {|requestedURL, localPath, finishedFunc, errorFunc, progressFunc|
		^super.new.init(requestedURL, localPath, finishedFunc, errorFunc, progressFunc);
	}

	*qtClass { ^'QtDownload' }

	init {|requestedURL, localPath, argfinFunc, argerrFunc, argprogFunc|
		localPath = localPath ?? { Platform.defaultTempDir +/+ "download" };
		finishedFunc = argfinFunc;
		errorFunc = argerrFunc;
		progressFunc = argprogFunc;
		if(requests.isNil, { requests = Set.new; });
		requests.add(this);
		this.startDownload(requestedURL, localPath);
	}

	cancel {
		"Download of % cancelled\n".postf(this.getProperty(\source));
		this.invokeMethod(\cancel);
		this.cleanup;
	}

	*cancelAll { requests.do({|dl| dl.cancel }); }

	// private

	doOnShutDown { this.cancel }

	startDownload { |requestedURL, localPath|
		if(started.not, {
			started = true;
			ShutDown.add(this);
			this.setProperty(\source, requestedURL);
			this.setProperty(\destination, localPath);
			this.connectMethod('doFinished()', 'doFinished');
			this.connectMethod('doError()', 'doError');
			this.connectMethod('doProgress(int, int)', 'doProgress');
			this.invokeMethod(\download);
		});
	}

	doFinished {
		"Download of % finished\n".postf(this.getProperty(\source));
		this.cleanup;
		if(error.not, {
			finishedFunc.value;
		});
	}

	cleanup {
		requests.remove(this);
		heap.remove(this);
		ShutDown.remove(this);
	}

	doError {
		error = true;
		this.cleanup;
		errorFunc.value;
	}

	doProgress {|bytesReceived, bytesTotal|
		if(error.not, {
			progressFunc.value(bytesReceived, bytesTotal);
		});
	}

}