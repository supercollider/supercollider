/*
[22.sep.2010]
  QStethoscope created by copying SCStethoscope and adjusting code for Qt GUI.
*/

QStethoscope {
  classvar ugenScopes;
  var <server, <numChannels, <rate,  <index;
  var <bufsize, buffer, <window, synth;
  var scopeView, indexView, nChanView, xZoomSlider, yZoomSlider;
  var style=0, sizeToggle=0, zx, zy, ai=0, ki=0, audiospec, controlspec, zoomspec;

  *implementsClass {^'Stethoscope'}

  *new { arg server, numChannels = 2, index, bufsize = 4096, zoom, rate, view, bufnum;
    if(server.inProcess.not, { "scope works only with internal server".error; ^nil });
    ^super.newCopyArgs(server, numChannels, rate ? \audio).makeWindow(view)
    .index_(index ? 0).zoom_(zoom).allocBuffer(bufsize, bufnum).run;
  }

  *tileBounds {
    var screenBounds = QWindow.screenBounds;
    var x = 520 + (ugenScopes.size * (223 + 5)) + 5;
    var right = x + 223;
    var y = floor(right / screenBounds.width) * 297 + 10;
    if(right > screenBounds.right) { x = floor(right % screenBounds.width / 223) * (223 + 5) };
    ^Rect(x, y, 223, 223)
  }

  makeBounds { arg size = 223; ^Rect(297, 5, size, size) }

  makeWindow { arg view;
    if(view.isNil)
    {
      window = QWindow("stethoscope", this.makeBounds);
      view = window.view;
      view.decorator = FlowLayout(window.view.bounds);
      window.front;
      window.onClose = { this.free };

    } {
      if(view.decorator.isNil, {
        "QStethoscope: makeWindow added a decorator to view".warn;
        view.decorator = FlowLayout(view.bounds);
      });
    };
    scopeView = QScope(view,
      Rect(0,0, view.bounds.width - 10 - 20 - 4, view.bounds.height - 40)
    );
    scopeView.background = Color.black;
    scopeView.resize = 5;
    view.keyDownAction = { arg view, char; this.keyDown(char) };
    view.background = Color.grey(0.6);
    zx = scopeView.xZoom.log2;
    zy = scopeView.yZoom.log2;

    audiospec = ControlSpec(0, server.options.numAudioBusChannels, step:1);
    controlspec = ControlSpec(0, server.options.numControlBusChannels, step:1);
    zoomspec = ControlSpec(0.125, 16, \exp);

    xZoomSlider = QSlider(view, Rect(10, 10, view.bounds.width - 80, 20));
    xZoomSlider.action = { arg x;
        /*var i;
        i = this.spec.map(x.value);
        this.index = i;*/
        this.xZoom = zoomspec.map(x.value)
      };
    xZoomSlider.resize = 8;
    xZoomSlider.value = zoomspec.unmap(this.xZoom);
    xZoomSlider.background = Color.grey(0.6);
    xZoomSlider.focusColor = Color.clear;

    indexView = QNumberBox(view, Rect(10, 10, 30, 20))
                .value_(0).decimals_(0).step_(1).scroll_step_(1);
    indexView.action = { this.index = indexView.value;  };
    indexView.resize = 9;
    indexView.font = QFont("Monaco", 9);
    nChanView = QNumberBox(view, Rect(10, 10, 25, 20))
                .value_(numChannels).decimals_(0).step_(1).scroll_step_(1);
    nChanView.action = { this.numChannels = nChanView.value.asInteger  };
    nChanView.resize = 9;
    nChanView.font = QFont("Monaco", 9);
    QStaticText(view, Rect(10, 10, 20, 20)).visible_(false);
    this.updateColors;


    view.decorator.reset;
    view.decorator.shift(scopeView.bounds.right, 0);

    yZoomSlider = QSlider(view, Rect(scopeView.bounds.right, 0, 20, scopeView.bounds.height));
    yZoomSlider.action = { arg x;
        this.yZoom = zoomspec.map(x.value)
      };
    yZoomSlider.resize = 6;
    yZoomSlider.value = zoomspec.unmap(this.yZoom);
    yZoomSlider.background = Color.grey(0.6);
    yZoomSlider.focusColor = Color.clear;
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
        if(char === $+) {  zx = zx - 0.25; this.xZoom = 2 ** zx; ^this  };        if(char === $*) {  zy = zy + 0.25; this.yZoom = 2 ** zy; ^this  };
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
    scopeView.bufnum = buffer.bufnum;
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
    if(n > 128) { "cannot display more than 128 channels at once".inform; n = 128 };
    if(n != numChannels and: { n > 0 }) {
      isPlaying = synth.isPlaying;
      if(isPlaying) { synth.free; synth.isPlaying = false; synth = nil }; // immediate
      numChannels = n;

      nChanView.value = n;
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
    indexView.value = index;
    // xZoomSlider.value = spec.unmap(index)
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

  xZoom_ { arg val;
    scopeView.xZoom = val;
    zx = val.log2;
    xZoomSlider.value = zoomspec.unmap(val);
  }
  yZoom_ { arg val;
    scopeView.yZoom = val;
    zy = val.log2;
    yZoomSlider.value = zoomspec.unmap(val);
  }
  xZoom { ^2.0 ** zx }
  yZoom { ^2.0 ** zy }

  zoom_ { arg val; this.xZoom_(val ? 1) }

  style_ { arg val;
    if(numChannels < 2 and: { val == 2 }) {
      "QStethoscope: can't do x/y scoping with one channel".warn;
      ^this;
    };
    scopeView.style = style = val
  }



  updateColors {
    scopeView.waveColors = if(\audio === rate) {
      Array.fill(numChannels, { Color.new255(255, 218, 000) });
    } {
      Array.fill(numChannels, { Color.new255(125, 255, 205) });
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
    this.allocBuffer(max(256,nextPowerOfTwo(
      asInteger(scopeView.bounds.width * scopeView.xZoom))))
  }

  // ugenScopes
  *ugenScopes {
    if(ugenScopes.isNil, { ugenScopes = Set.new; });
    ^ugenScopes
  }

  /**
   *  @return (Server) the default server to scope on
   */
  *defaultServer {
    ^Server.internal;
  }

  /**
   *  @param  aServer (Server) a server to test for scoping
   *  @return     (Boolean) indication whether the server can be scope'ed
   */
  *isValidServer { arg aServer;
    ^aServer.inProcess;
  }
}
