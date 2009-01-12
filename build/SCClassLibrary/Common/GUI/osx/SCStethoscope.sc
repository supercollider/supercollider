SCStethoscope {
	classvar ugenScopes;
	var <server, <numChannels, <rate,  <index;
	var <bufsize, buffer, <window, synth;
	var n, c, d, sl, sl2;
	var style=0, sizeToggle=0, zx, zy, ai=0, ki=0, audiospec, controlspec, zoomspec;
	

	*new { arg server, numChannels = 2, index, bufsize = 4096, zoom, rate, view, bufnum;
		if(server.inProcess.not, { "scope works only with internal server".error; ^nil });		^super.newCopyArgs(server, numChannels, rate ? \audio).makeWindow(view)
		.index_(index ? 0).zoom_(zoom).allocBuffer(bufsize, bufnum).run;
	}
	
	*tileBounds {
		var screenBounds = SCWindow.screenBounds;
		var x = 544 + (ugenScopes.size * 222);
		var right = x + 212;
		var y = floor(right / screenBounds.width) * 242 + 10;
		if(right > screenBounds.right) { x = floor(right % screenBounds.width / 222) * 222 };
		^Rect(x, y, 212, 212)
	}
	
	makeBounds { arg size=212; ^Rect(322, 16, size, size) }
	
	makeWindow { arg view;
		if(view.isNil) 
		{
			window = SCWindow("stethoscope", this.makeBounds);
			view = window.view;
			view.decorator = FlowLayout(window.view.bounds);
			window.front;
			window.onClose = { this.free };
			
		};
		n = SCScope(view, Rect(0,0, view.bounds.width - 10 - 20 - 4, view.bounds.height - 40));
		n.background = Color.black;
		n.resize = 5;
		view.keyDownAction = { arg view, char; this.keyDown(char) };
			
		zx = n.xZoom.log2;
		zy = n.yZoom.log2;
		
		audiospec = ControlSpec(0, server.options.numAudioBusChannels, step:1);
		controlspec = ControlSpec(0, server.options.numControlBusChannels, step:1);
		zoomspec = ControlSpec(0.125, 16, \exp);
						
		sl = SCSlider(view, Rect(10, 10, view.bounds.width - 80, 20));
		sl.action = { arg x;
				/*var i; 
				i = this.spec.map(x.value);
				this.index = i;*/
				this.xZoom = zoomspec.map(x.value)
			};
		sl.resize = 8;
		sl.value = zoomspec.unmap(this.xZoom);
		sl.background = Color.grey(0.6);
		sl.focusColor = Color.clear;
		
		c = SCNumberBox(view, Rect(10, 10, 30, 20)).value_(0);
		c.action = { this.index = c.value;  };
		c.resize = 9;
		c.font = Font("Monaco", 9);
		d = SCNumberBox(view, Rect(10, 10, 25, 20)).value_(numChannels);
		d.action = { this.numChannels = d.value.asInteger  };
		d.resize = 9;
		d.font = Font("Monaco", 9);
		SCStaticText(view, Rect(10, 10, 20, 20)).visible_(false);
		this.updateColors;
		
		
		view.decorator.reset;
		view.decorator.shift(n.bounds.right, 0);
		
		sl2 = SCSlider(view, Rect(n.bounds.right, 0, 20, n.bounds.height));
		sl2.action = { arg x;
				this.yZoom = zoomspec.map(x.value)
			};
		sl2.resize = 6;
		sl2.value = zoomspec.unmap(this.yZoom);
		sl2.background = Color.grey(0.6);
		sl2.focusColor = Color.clear;
	}
	
	keyDown { arg char;
				if(char === $i) { this.toInputBus; ^this }; 
				if(char === $o) { this.toOutputBus;  ^this  }; 
				if(char === $ ) { this.run;  ^this  };
				if(char === $s) { this.style = (style + 1) % 2; ^this  };
				if(char === $S) { this.style = 2;  ^this  };
				if(char === $j or: { char.ascii === 0 }) { this.index = index - 1; ^this  };
				if(char === $k) { this.switchRate; ^this  };
				if(char === $l or: { char.ascii === 1 }) { this.index = index + 1 };
				if(char === $-) {  zx = zx + 0.25; this.xZoom = 2 ** zx; ^this  };
				if(char === $+) {  zx = zx - 0.25; this.xZoom = 2 ** zx; ^this  };				if(char === $*) {  zy = zy + 0.25; this.yZoom = 2 ** zy; ^this  };
				if(char === $_) {  zy = zy - 0.25; this.yZoom = 2 ** zy; ^this  };
				if(char === $A) {  this.adjustBufferSize; ^this  };
				if(char === $m) { this.toggleSize; ^this  };
				if(char === $.) { if(synth.isPlaying) { synth.free } };

	}
	
	spec { ^if(rate === \audio) { audiospec } {  controlspec } }
	
	setProperties { arg numChannels, index, bufsize=4096, zoom, rate;
				
				if(rate.notNil) { this.rate = rate };
				if(index.notNil) { this.index = index };
				if(numChannels.notNil) { this.numChannels = numChannels };
				if(this.bufsize != bufsize) { this.allocBuffer(bufsize) };
				if(zoom.notNil) { this.zoom = zoom };
	}
	
	allocBuffer { arg argbufsize, argbufnum;
		bufsize = argbufsize ? bufsize;
		if(buffer.notNil) { buffer.free };
		buffer = Buffer.alloc(server, bufsize, numChannels, nil, argbufnum);
		n.bufnum = buffer.bufnum;
		if(synth.isPlaying) { synth.set(\bufnum, buffer.bufnum) };
	}
	
	run {
		if(synth.isPlaying.not) {
			synth = SynthDef("stethoscope", { arg in, switch, bufnum;
				var z;
				z = Select.ar(switch, [In.ar(in, numChannels), K2A.ar(In.kr(in, numChannels))]); 
				ScopeOut.ar(z, bufnum);
			}).play(RootNode(server), [\bufnum, buffer.bufnum, \in, index, \switch] 
				++ if('audio' === rate) { 0 } { 1 },
				\addToTail
			);
			synth.isPlaying = true;
			NodeWatcher.register(synth);
		}
	}
	
	free {
		buffer.free;
		
		if(synth.isPlaying) {  synth.free };
		synth = nil;
		if(server.scopeWindow === this) { server.scopeWindow = nil }
	}
	
	quit {
		window.close;
		this.free;
	}
	
	numChannels_ { arg n;
	
		var isPlaying;
		if(n > 16) { "cannot display more than 16 channels at once".inform; n = 16 };
		if(n != numChannels and: { n > 0 }) {  
			isPlaying = synth.isPlaying;
			if(isPlaying) { synth.free; synth.isPlaying = false; synth = nil }; // immediate
			numChannels = n;
			
			d.value = n;
			this.allocBuffer;
			if(isPlaying) {  this.run };
			this.updateColors;
		};
	}
	
	index_ { arg val=0;
		var spec;
		spec = this.spec;
		index = spec.constrain(val);
		if(synth.isPlaying) { synth.set(\in, index) };
		if(rate === \audio) { ai = index } { ki = index };
		c.value = index;
		// sl.value = spec.unmap(index)
	}
	
	rate_ { arg argRate=\audio;
		if(rate === argRate) {  ^this };
		if(argRate === \audio)
		{ 
				if(synth.isPlaying) { synth.set(\switch, 0) };
				rate = \audio;
				this.updateColors;
				ki = index;
				this.index = ai;
		}
		{
				if(synth.isPlaying) { synth.set(\switch, 1) };
				rate = \control;
				this.updateColors;
				ai = index;
				this.index = ki;
		}
	}
	
	size_ { arg val; if(window.notNil) { window.bounds = this.makeBounds(val) } }
	toggleSize {  if(sizeToggle == 0) 
					{ sizeToggle = 1; this.size_(500) }
					{ sizeToggle = 0; this.size_(212) } 
	}
	
	xZoom_ { arg val; n.xZoom = val; zx = val.log2; sl.value = zoomspec.unmap(val); }
	yZoom_ { arg val; n.yZoom = val; zy = val.log2; sl2.value = zoomspec.unmap(val); }
	xZoom { ^2.0 ** zx }
	yZoom { ^2.0 ** zy }
	
	zoom_ { arg val; this.xZoom_(val ? 1) }

	style_ { arg val; n.style = style = val }
	
	
	
	updateColors {
		n.waveColors = if(\audio === rate) { 
			Array.fill(numChannels, { rgb(255, 218, 000) }); 
		} { 
			Array.fill(numChannels, { rgb(125, 255, 205) }); 
		}
	}
	
	switchRate { if(rate === \control) { this.rate = \audio } {  this.rate = \control } }
	
	toInputBus {
		this.index = server.options.numOutputBusChannels;
		this.numChannels = server.options.numInputBusChannels;
	}
	toOutputBus {
		this.index = 0;
		this.numChannels = server.options.numOutputBusChannels;
	}
	adjustBufferSize {
		this.allocBuffer(max(256,nextPowerOfTwo(asInteger(n.bounds.width * n.xZoom))))
	}
	
	// ugenScopes
	*ugenScopes {
		if(ugenScopes.isNil, { ugenScopes = Set.new; });
		^ugenScopes
	}
	
	/**
	 *	@return	(Server) the default server to scope on
	 */
	*defaultServer {
		^Server.internal;
	}
	
	/**
	 *	@param	aServer	(Server) a server to test for scoping
	 *	@return			(Boolean) indication whether the server can be scope'ed
	 */
	*isValidServer { arg aServer;
		^aServer.inProcess;
	}
}