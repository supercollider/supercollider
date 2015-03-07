ScopeView : View {
	var <bufnum;
	var <xZoom=1.0, <yZoom=1.0, <x=0.0, <y=0.0, <fill=true;
	var <waveColors;

	*qtClass { ^'QcScopeShm' }

	bufnum_ { arg buf;
		bufnum = buf.asControlInput;
		this.setProperty( \bufferNumber, bufnum );
	}

	server_ { arg server;
		var port = if(server.inProcess){ thisProcess.pid }{ if(server.isLocal){server.addr.port} };
		if(port.isNil) {
			"ScopeView: Can not scope on the desired server: %".format(server).warn
		} {
			this.setProperty(\serverPort, port);
		};
	}

	start { this.invokeMethod(\start); }
	stop { this.invokeMethod(\stop); }

	style { ^this.getProperty(\style) }

	style_ { arg value; this.setProperty(\style, value) }

	xZoom_ { arg aFloat;
		xZoom = aFloat;
		this.setProperty( \xZoom, aFloat );
	}

	yZoom_ { arg aFloat;
		yZoom = aFloat;
		this.setProperty( \yZoom, aFloat );
	}

	x_ { arg aFloat;
		x = aFloat;
		this.setProperty( \xOffset, aFloat );
	}

	y_ { arg aFloat;
		y = aFloat;
		this.setProperty( \yOffset, aFloat );
	}

	fill_ {
		arg aFill;
		fill = aFill;
		this.setProperty( \fill, aFill );
	}

	waveColors_ { arg aColorArray;
		waveColors = aColorArray;
		this.setProperty( \waveColors, aColorArray );
	}
}
