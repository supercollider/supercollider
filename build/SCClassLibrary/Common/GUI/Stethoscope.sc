Stethoscope {
	var <server, <numChannels, <numFrames;
	var buffer, <window, synth, <index;
	var b, n, c, d, sl, zx, zy, spec, rate = \audio, <size=200;
	

	*new { arg server, numFrames=4096, numChannels=2, zoom;
		if(server.inProcess.not, { "scope works only with internal server".error; ^this.halt });		^super.newCopyArgs(server, numChannels).makeWindow.zoom_(zoom).allocBuffer(numFrames).run;
	}
	
	windowBounds { ^Rect(10 + 20  + 256, 0 + 10, size + 10 , size + 40) }
	
	makeWindow { arg w;
		var style=0;
		if(w.isNil,{
			window = w = SCWindow("stethoscope", this.windowBounds);
			w.view.decorator = FlowLayout(w.view.bounds);
			w.front;
			w.onClose = { this.free };
			w.view.keyDownAction = 
			{ arg view, char; 
				if(char === $i) { this.setIndexToFirstHardwareBus; this.rate = \audio }; 
				if(char === $ ) { this.run };
				if(char === $s) { n.style = style = (style + 1) % 3 };
				if(char === $j or: { char.ascii === 0 }) { this.index = index - 1 };
				if(char === $k) { this.switchRate };
				if(char === $l or: { char.ascii === 1 }) { this.index = index + 1 };
				if(char === $-) {  zx = zx + 0.25; n.xZoom = 2 ** zx };
				if(char === $+) {  zx = zx - 0.25; n.xZoom = 2 ** zx };				if(char === $*) {  zy = zy + 0.25; n.yZoom = 2 ** zy };
				if(char === $_) {  zy = zy - 0.25; n.yZoom = 2 ** zy };
			};
		});
		
		n = SCScope(w, Rect(0,0, size, size));
		n.background = Color.black;		n.resize = 5;
		zx = n.xZoom.log2;
		zy = n.yZoom.log2;
		
		spec = ControlSpec.specs.at(\audiobus);
		
		sl = SCSlider(w, Rect(10, 10, w.view.bounds.width - 100, 20));
		sl.action = { arg x;
				var i; 
				i = spec.map(x.value);
				this.index = i;
			};
		sl.resize = 8;
		c = SCNumberBox(w, Rect(10,10, 25,20)).value_(0);
		c.action = { this.index = c.value;  };
		c.resize = 9;
		d = SCNumberBox(w, Rect(10,10, 25,20)).value_(numChannels);
		d.action = { this.numChannels = d.value.asInteger  };
		d.resize = 9;
		b = SCButton(w,Rect(10,10,25,20));
		b.states = [
			["ar",Color.black,Color.clear],
			["kr",Color.black,Color.clear]
		];
		b.resize = 9;
		b.action = {this.switchRate};
		b.valueAction = 0;
		this.index = 0;
	}
	
	allocBuffer { arg argNumFrames;
		numFrames = argNumFrames ? numFrames;
		if(buffer.notNil) { buffer.free };
		buffer = Buffer.alloc(server, numFrames, numChannels);
		n.bufnum = buffer.bufnum;
		if(synth.isPlaying) { synth.set(\bufnum, buffer.bufnum) };
	}
	
	run {
		if(synth.isPlaying.not) {
			synth = SynthDef("stethoscope", { arg in, switch, bufnum;
				var z;
				z = Select.ar(switch, [In.ar(in, numChannels), K2A.ar(In.kr(in, numChannels))]); 
				ScopeOut.ar(z, bufnum);
			}).play(server, [\bufnum, buffer.bufnum, \in, index, \switch] 
				++ if('audio' === rate) { 0 } { 1 }
			);
			synth.isPlaying = true;
			NodeWatcher.register(synth);
			this.updateColors;
		}
	}
	
	free {
		buffer.free;
		if(synth.isPlaying) { synth.free };
		synth = nil;
		if(server.scopeWindow === this) { server.scopeWindow = nil }
	}
	
	numChannels_ { arg n;
	
		var isPlaying;
		if(n != numChannels) {Ê
			
			isPlaying = synth.isPlaying;
			if(isPlaying) { synth.free; synth.isPlaying = false; }; // immediate
			numChannels = n;
			
			d.value = n;
			this.allocBuffer;
			if(isPlaying) {Êthis.run };
		};
	}
	
	index_ { arg val=0;
		index = spec.constrain(val);
		if(synth.isPlaying) { synth.set(\in, index) };
		c.value = index;
		sl.value = spec.unmap(index);
	}
	
	rate_ { arg argRate=\audio;
		
		if(argRate === \audio)
		{ 
				b.value = 0;
				synth.set(\switch, 0);
				rate = \audio;
				this.updateColors;
		}
		{
				b.value = 1;
				synth.set(\switch, 1);
				rate = \control;
				this.updateColors;
		}
	}
	size_ { arg val;
		size = val;
		window.bounds = this.windowBounds;
	}
	zoom_ { arg val;
		val = val ? 1;
		zx = zy = val.log2;
		n.xZoom = val;
		n.yZoom = val;
	}
	
	updateColors {
		n.waveColors = if(\audio === rate) { 
			Array.fill(numChannels, { rgb(255, 218, 000) }); 
		} { 
			Array.fill(numChannels, { rgb(125, 255, 205) }); 
		}
	}
	
	switchRate { if(rate === \control) { this.rate = \audio } {  this.rate = \control } }
	
	setIndexToFirstHardwareBus {
		this.index = server.options.numInputBusChannels;
		this.rate = \audio;
	}

}
