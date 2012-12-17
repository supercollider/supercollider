QStethoscope2 {
  classvar ugenScopes;

  // internal functions
  var playSynthDef, makeGui, setCycle, setYZoom, setIndex, setNumChannels,
      setRate, setStyle, updateColors;

  // gui objects
  var <window, <view, <scopeView, cycleSlider, yZoomSlider,
      idxNumBox, chNumBox, styleMenu, rateMenu;

  // static (immutable runtime environment)
  var <server, synth;
  var maxBufSize;
  var aBusSpec, cBusSpec, cycleSpec, yZoomSpec;
  var <>smallSize, <>largeSize;

  // runtime (mutable at runtime)
  var <bus; // partly immutable; can't change numChannels at runtime
  var busSpec; // either aBusSpec or cBusSpec, depending on bus rate
  var  <cycle, <yZoom;
  var sizeToggle=false;

  *implementsClass {^'Stethoscope'}

  *defaultServer { ^if( Server.default.isLocal, Server.default, Server.local ) }

  *isValidServer { arg aServer; ^aServer.isLocal }

  *ugenScopes {
    if (ugenScopes.isNil) { ugenScopes = Set.new };
    ^ugenScopes
  }

  *tileBounds {
    var screenBounds = QWindow.availableBounds;
    var w = 250, h = 250;
    var x = (ugenScopes.size * (w + 10)) + 10;
    var right = x + w;
    var y = floor(right / screenBounds.width) * (h + 20) + 20;
    if(right > screenBounds.width)
      { x = floor(right % screenBounds.width / (w + 10)) * (w + 10) + 10 };
    x = x + screenBounds.left;
    ^Rect(x, y, w, h)
  }

  *new {
    arg server, numChannels = 2, index = 0, bufsize = 4096,
        zoom = 1.0, rate = \audio, view, bufnum;
    var bus;

    if(server.isNil) {server = this.defaultServer};
    if(server.isLocal.not) {Error("Can not scope on remote server.").throw};

    bus = Bus(rate, index, numChannels, server);

    ^super.new.initQStethoscope( server, view, bus, bufsize, 1024 * zoom.asFloat.reciprocal );
  }

  initQStethoscope { arg server_, parent, bus_, bufsize_, cycle_;
    var singleBus;

    server = server_;
    synth = BusScopeSynth(server);

    maxBufSize = max(bufsize_, 128);

    bus = bus_;
    singleBus = bus.class === Bus;

    aBusSpec = ControlSpec(0, server.options.numAudioBusChannels, step:1);
    cBusSpec = ControlSpec(0, server.options.numControlBusChannels, step:1);
    if( singleBus ) {
      busSpec = if(bus.rate===\audio){aBusSpec}{cBusSpec};
    };

    cycleSpec = ControlSpec( maxBufSize, 64, \exponential );
    yZoomSpec = ControlSpec( 0.125, 16, \exponential );
    cycle = cycleSpec.constrain(cycle_);
    yZoom = 1.0;

    smallSize = Size(250,250);
    largeSize = Size(500,500);

    makeGui = { arg parent;
      var gizmo;

      // WINDOW, WRAPPER VIEW

      if( window.notNil ) {window.close};

      if( parent.isNil ) {
        view = window = QWindow(
          bounds: (smallSize).asRect.center_(QWindow.availableBounds.center)
        ).name_("Stethoscope");
      }{
        view = QView( parent, Rect(0,0,250,250) );
        window = nil;
      };

      // WIDGETS

      scopeView = QScope2();
      scopeView.server = server;
      scopeView.canFocus = true;

      cycleSlider = QSlider().orientation_(\horizontal).value_(cycleSpec.unmap(cycle));
      yZoomSlider = QSlider().orientation_(\vertical).value_(yZoomSpec.unmap(yZoom));

      rateMenu = QPopUpMenu().items_(["Audio","Control"]).enabled_(singleBus);
      idxNumBox = QNumberBox().decimals_(0).step_(1).scroll_step_(1).enabled_(singleBus);
      chNumBox = QNumberBox().decimals_(0).step_(1).scroll_step_(1)
        .clipLo_(1).clipHi_(128).enabled_(singleBus);

      if( singleBus ) {
        rateMenu.value_(if(bus.rate===\audio){0}{1});
        idxNumBox.clipLo_(busSpec.minval).clipHi_(busSpec.maxval).value_(bus.index);
        chNumBox.value_(bus.numChannels);
      };

      styleMenu = QPopUpMenu().items_(["Tracks","Overlay","X/Y"]);

      // LAYOUT

      gizmo = "999".bounds( idxNumBox.font ).width + 20;
      idxNumBox.fixedWidth = gizmo;
      chNumBox.fixedWidth = gizmo;
      idxNumBox.align = \center;
      chNumBox.align = \center;

      view.layout =
        QGridLayout()
          .add(
            QHLayout(
              rateMenu,
              idxNumBox,
              chNumBox,
              nil,
              styleMenu
            ).margins_(0).spacing_(2), 0, 0
          )
          .add(scopeView,1,0)
          .add(yZoomSlider.maxWidth_(15), 1,1)
          .add(cycleSlider.maxHeight_(15), 2,0)
          .margins_(2).spacing_(2);

      // ACTIONS

      cycleSlider.action = { |me| setCycle.value(cycleSpec.map(me.value)) };
      yZoomSlider.action = { |me| setYZoom.value(yZoomSpec.map(me.value)) };
      idxNumBox.action = { |me| setIndex.value(me.value) };
      chNumBox.action = { |me| setNumChannels.value(me.value) };
      rateMenu.action = { |me| setRate.value(me.value) };
      styleMenu.action = { |me| setStyle.value(me.value) };
      view.asView.keyDownAction = { |v, char, mod| this.keyDown(char, mod) };
      view.onClose = { view = nil; this.quit; };

      // LAUNCH

      scopeView.focus;
      if( window.notNil ) { window.front };
    };

    setCycle = { arg val;
      cycle = val;
      synth.setCycle(val);
    };

    setYZoom = { arg val;
      yZoom = val;
      scopeView.yZoom = val;
    };

    // NOTE: assuming a single Bus
    setIndex = { arg i;
      bus = Bus(bus.rate, i, bus.numChannels, bus.server);
      synth.setBusIndex(i);
    };

    // NOTE: assuming a single Bus
    setNumChannels = { arg n;
      // we have to restart the whole thing:
      bus = Bus(bus.rate, bus.index, n, bus.server);
      updateColors.value;
      this.run;
    };

    // NOTE: assuming a single Bus
    setRate = { arg val;
      val.switch (
        0, {
          bus = Bus(\audio, bus.index, bus.numChannels, bus.server);
          busSpec = aBusSpec;
        },
        1, {
          bus = Bus(\control, bus.index, bus.numChannels, bus.server);
          busSpec = cBusSpec;
        }
      );
      synth.setRate(val);
      idxNumBox.clipLo_(busSpec.minval).clipHi_(busSpec.maxval).value_(bus.index);
      this.index = bus.index; // ensure conformance with busSpec;
      updateColors.value;
    };

    setStyle = { arg val;
      if(this.numChannels < 2 and: { val == 2 }) {
        "QStethoscope: x/y scoping with one channel only; y will be a constant 0".warn;
      };
      scopeView.style = val;
    };

    updateColors = {
      var colors;
      bus.do { |b|
        var c = if(b.rate === \audio){Color.new255(255, 218, 000)}{Color.new255(125, 255, 205)};
        colors = colors ++ Array.fill(b.numChannels, c);
      };
      scopeView.waveColors = colors;
    };

    makeGui.value(parent);
    updateColors.value;

    ServerTree.add(this, server);
    ServerQuit.add(this, server);
    this.run;
  }

  doOnServerTree {
    this.run;
  }

  doOnServerQuit {
    this.stop;
  }

  run {
    synth.play(maxBufSize, bus, cycle);
    if( view.notNil && synth.bufferIndex.notNil) {
      scopeView.bufnum = synth.bufferIndex;
      scopeView.start;
    };
  }

  stop {
    if( view.notNil ) { {scopeView.stop}.defer };
    synth.stop;
  }

  quit {
    var win;
    this.stop;
    synth.free;
    if(window.notNil) { win = window; window = nil; { win.close }.defer; };
    ServerTree.remove(this, server);
    ServerQuit.remove(this, server);
  }

  setProperties { arg numChannels, index, bufsize, zoom, rate;
    var new_bus;

    // process args

    if(index.notNil || numChannels.notNil || rate.notNil) {
      bus = if(bus.class === Bus) {
        Bus (
          rate ? bus.rate,
          index ? bus.index,
          numChannels ? bus.numChannels,
          server
        )
      }{
        Bus (
          rate ? \audio,
          index ? 0,
          numChannels ? 2,
          server
        )
      };
    };
    if(bufsize.notNil) { maxBufSize = max(bufsize, 128) };

    // set other vars related to args

    busSpec = if(bus.rate === \audio) {aBusSpec} {cBusSpec};
    cycleSpec = ControlSpec( maxBufSize, 64, \exponential );
    if(zoom.notNil)
    { cycle = cycleSpec.constrain( 1024 * zoom.asFloat.reciprocal ) };

    // update GUI

    cycleSlider.value = cycleSpec.unmap(cycle);
    rateMenu.value_(if(bus.rate === \audio){0}{1}).enabled_(true);
    idxNumBox.clipLo_(busSpec.minval).clipHi_(busSpec.maxval).value_(bus.index).enabled_(true);
    chNumBox.value_(bus.numChannels).enabled_(true);
    updateColors.value;

    if (synth.isRunning) { synth.play(maxBufSize, bus, cycle) };
  }

  bufsize { ^maxBufSize }

  bus_ { arg b;
    var isSingle = b.class === Bus;

    bus = b;

    if( isSingle ) {
      busSpec = if(bus.rate === \audio) {aBusSpec} {cBusSpec};
      rateMenu.value = if(b.rate===\audio){0}{1};
      idxNumBox.clipLo_(busSpec.minval).clipHi_(busSpec.maxval).value_(bus.index);
      chNumBox.value = b.numChannels;
    }{
      busSpec = nil;
      rateMenu.value = nil;
      idxNumBox.string = "-";
      chNumBox.string = "-";
    };
    rateMenu.enabled = isSingle;
    idxNumBox.enabled = isSingle;
    chNumBox.enabled = isSingle;

    updateColors.value;

    if (synth.isRunning) { synth.play(maxBufSize, bus, cycle); };
  }

  numChannels {
    var num;
    if( bus.class === Bus ) {
      ^bus.numChannels;
    }{
      num = 0; bus.do { |b| num = num + b.numChannels };
      ^num;
    }
  }

  // will always be clipped between 0 and the amount of channels
  // at the beginning of the current run
  numChannels_ { arg n;
    if( (bus.class === Bus).not ) { ^this };
    setNumChannels.value(n);
    chNumBox.value = n;
  }

  index {
    if( bus.class === Bus ) { ^bus.index } { nil }
  }

  index_ { arg i;
    if( (bus.class === Bus).not ) { ^this };
    setIndex.value( busSpec.constrain(i) );
    idxNumBox.value = i;
  }

  rate {
    if( bus.class === Bus ) { ^bus.rate } { nil }
  }

  rate_ { arg argRate=\audio;
    var val;
    if( (bus.class === Bus).not ) { ^this };
    val = if(argRate===\audio){0}{1};
    setRate.value(val);
    rateMenu.value = val;
  }

  switchRate {
    if( bus.class === Bus ) {
      this.rate = if(bus.rate === \control) {\audio} {\control};
    }
  }

  // [0, 1] -> [64, 8192] frames
  cycle_ { arg val;
    setCycle.value( cycleSpec.constrain(val) );
    cycleSlider.value = cycleSpec.unmap(val);
  }

  xZoom_ { arg val; this.cycle = 1024 * val.asFloat.reciprocal }
  xZoom { ^(1024 * cycle.reciprocal) }

  zoom { ^this.xZoom }
  zoom_ { arg val; this.xZoom_(val ? 1) }

  // [0, 1] -> [0.125, 16] y scaling factor
  yZoom_ { arg val;
    setYZoom.value( yZoomSpec.constrain(val) );
    yZoomSlider.value = yZoomSpec.unmap(val);
  }

  style_ { arg val;
    setStyle.value(val);
    styleMenu.value = val;
  }

  size_ { arg value;
    var sz = value.asSize;
    if( window.notNil ) { window.setInnerExtent(sz.width,sz.height) };
  }

  toggleSize {
    if(window.notNil) {
      sizeToggle = sizeToggle.not;
      if(sizeToggle)
        { this.size = largeSize }
        { this.size = smallSize };
    }
  }

  toInputBus {
    var i = server.options.numOutputBusChannels;
    var c = server.options.numInputBusChannels;
    this.bus = Bus(\audio, i, c, server);
  }

  toOutputBus {
    var c = server.options.numOutputBusChannels;
    this.bus = Bus(\audio, 0, c, server);
  }

  keyDown { arg char, mod;
    if (mod != 0) { ^false };
    case (
      { char === $i }, { this.toInputBus },
      { char === $o }, { this.toOutputBus },
      { char === $  }, { this.run },
      { char === $s }, { this.style = (scopeView.style + 1).wrap(0,2) },
      { char === $S }, { this.style = 2 },
      { char === $j }, { if(this.index.notNil) {this.index = this.index - 1} },
      { char === $k }, { this.switchRate; },
      { char === $l }, { if(this.index.notNil) {this.index = this.index + 1} },
      { char === $- }, { cycleSlider.increment; cycleSlider.doAction },
      { char === $+ }, { cycleSlider.decrement; cycleSlider.doAction },
      { char === $* }, { yZoomSlider.increment; yZoomSlider.doAction },
      { char === $_ }, { yZoomSlider.decrement; yZoomSlider.doAction },
      { char === $m }, { this.toggleSize },
      { char === $.}, { this.stop },
      { ^false }
    );
    ^true;
  }
}

BusScopeSynth {
  // Encapsulate management of server resources

  var server, buffer, synthDefName, synth;
  var playThread, playCond;

  *new { arg server;
    var instance;
    server = server ? Server.default;
    instance = super.newCopyArgs(server);
    ServerQuit.add(instance);
    ^instance;
  }

  play { arg bufSize, bus, cycle;
    var synthDef;
    var synthArgs;
    var bufIndex;
    var busChannels;

    if(server.serverRunning.not) { ^this };

    this.stop;

    if (buffer.isNil) {
      buffer = ScopeBuffer.alloc(server);
      synthDefName = "stethoscope" ++ buffer.index.asString;
    };

    bufIndex = buffer.index.asInteger;

    if( bus.class === Bus ) {
      busChannels = bus.numChannels.asInteger;
      synthDef = SynthDef(synthDefName, { arg busIndex, rate, cycle;
        var z;
        z = Select.ar(rate, [
          In.ar(busIndex, busChannels),
          K2A.ar(In.kr(busIndex, busChannels))]
        );
        ScopeOut2.ar(z, bufIndex, bufSize, cycle );
      });
      synthArgs = [\busIndex, bus.index.asInteger, \rate, if('audio' === bus.rate, 0, 1), \cycle, cycle];
    }{
      synthDef = SynthDef(synthDefName, { arg cycle;
        var z = Array();
        bus.do { |b| z = z ++ b.ar };
        ScopeOut2.ar(z, bufIndex, bufSize, cycle);
      });
      synthArgs =	[\cycle, cycle];
    };

    playThread = fork {
      var cond;
      //("BufScopeSynth: waiting on previous synth...").postln;
      if (playCond.notNil) { playCond.wait };
      //postln("BufScopeSynth: got way.");
      synthDef.send(server);
      server.sync;
      //postln("BufScopeSynth: synthdef sent.");
      synth = Synth.tail(RootNode(server), synthDef.name, synthArgs);
      //postln("BufScopeSynth: made synth:" + synth.nodeID);
      playCond = cond = Condition();
      synth.onFree { |thisSynth|
        //postln("BufScopeSynth: Synth freed:" + thisSynth.nodeID);
        cond.test = true; cond.signal;
        if (synth.notNil and: {synth.nodeID == thisSynth.nodeID}) { synth = nil; }
      }
    }
  }

  stop {
    if (playThread.notNil) { playThread.stop; playThread = nil };
    if (synth.notNil) {
      synth.free; synth = nil
    };
  }

  isRunning { ^playThread.notNil }

	bufferIndex { ^ buffer !? { buffer.index } }

  setBusIndex { arg index;
    if( synth.notNil ) { synth.set(\busIndex, index) };
  }

  setRate { arg rate; // 0 = audio, 1 = control
    if( synth.notNil ) { synth.set(\rate, rate) };

  }

  setCycle { arg frames;
    if( synth.notNil ) { synth.set(\cycle, frames) };
  }

  free {
    this.stop;
    if (buffer.notNil) {
      buffer.free;
      buffer = nil;
    };
    ServerQuit.remove(this, server);
  }

  doOnServerQuit {
    buffer = nil;
    synth = nil;
    playCond = nil;
  }
}
