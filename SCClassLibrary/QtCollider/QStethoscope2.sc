QStethoscope2 {
  // internal functions
  var makeGui, run, synthStatus, setCycle, setYZoom, setIndex, setNumChannels,
      setRate, setStyle, updateColors;

  // gui objects
  var <window, <view, <scopeView, cycleSlider, yZoomSlider,
      idxNumBox, chNumBox, styleMenu, rateMenu;

  var cycleSpec, yZoomSpec;

  // other objects
  var <server, port;
  var <bufnum, <bufsize;
  var <bus;
  var aBusSpec, cBusSpec, busSpec;

  // runtime
  var  <cycle, <yZoom;
  var sizeToggle=false, <>smallSize, <>largeSize;
  var synth;

  *new {
    arg server, numChannels = 2, index = 0, bufsize = 4096,
        cycle = 1024, rate = \audio, view, bufnum = 0;

    var bus = Bus(rate, index, numChannels, server);

    ^super.new.initQStethoscope( server, view, bus, bufnum, bufsize, cycle );
  }

  initQStethoscope { arg server_, parent, bus_, bufnum_, bufsize_, cycle_;
    var singleBus;

    server = server_;
    if( server.inProcess ) {
        port = thisProcess.pid;
    }{
      if( server.isLocal ) {
        port = server.addr.port;
      }
    };
    if( port.isNil ) {^this};

    bufnum = bufnum_;
    bufsize = max(bufsize_, 128);

    bus = bus_;
    singleBus = bus.class === Bus;

    aBusSpec = ControlSpec(0, server.options.numAudioBusChannels, step:1);
    cBusSpec = ControlSpec(0, server.options.numControlBusChannels, step:1);
    if( singleBus ) {
      busSpec = if(bus.rate===\audio){aBusSpec}{cBusSpec};
    };

    smallSize = Size(250,250);
    largeSize = Size(500,500);

    makeGui = { arg parent;
      var view, gizmo;

      // SETTINGS

      cycleSpec = ControlSpec( 64, bufsize, \exponential );
      yZoomSpec = ControlSpec( 0.125, 16, \exponential );
      cycle = cycleSpec.constrain(cycle_);
      yZoom = yZoomSpec.unmap(1.0);

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
      scopeView.setProperty(\bufferNumber, bufnum);
      scopeView.setProperty(\channelCount, this.numChannels);
      scopeView.canFocus = true;

      cycleSlider = QSlider().orientation_(\horizontal).value_(cycleSpec.unmap(cycle));
      yZoomSlider = QSlider().orientation_(\vertical).value_(yZoom);

      rateMenu = QPopUpMenu().items_(["AUDIO","CONTROL"]).enabled_(singleBus);
      idxNumBox = QNumberBox().decimals_(0).step_(1).scroll_step_(1).enabled_(singleBus);
      chNumBox = QNumberBox().decimals_(0).step_(1).scroll_step_(1)
        .clipLo_(1).clipHi_(128).enabled_(singleBus);

      if( singleBus ) {
        rateMenu.value_(if(bus.rate===\audio){0}{1});
        idxNumBox.clipLo_(busSpec.minval).clipHi_(busSpec.maxval).value_(bus.index);
        chNumBox.value_(bus.numChannels);
      };

      styleMenu = QPopUpMenu().items_(["TRACKS","MIX","X/Y"]);

      // LAYOUT

      gizmo = idxNumBox.minSizeHint.width * 2;
      idxNumBox.minWidth = gizmo;
      idxNumBox.maxWidth = gizmo;
      chNumBox.minWidth = gizmo;
      chNumBox.maxWidth = gizmo;

      view.layout = QVLayout(
        QGridLayout()
          .add(
            QHLayout(
              rateMenu,
              idxNumBox,
              chNumBox,
              styleMenu,
              nil
            ).margins_(0).spacing_(3),0,0
          )
          .add(scopeView,1,0)
          .add(yZoomSlider,1,1)
          .add(cycleSlider,2,0)
          .margins_(0).spacing_(0)
      ).margins_([5,0,0,0]).spacing_(1);

      // ACTIONS

      cycleSlider.action = { |me| setCycle.value(cycleSpec.map(me.value)) };
      yZoomSlider.action = { |me| setYZoom.value(me.value) };
      idxNumBox.action = { |me| setIndex.value(me.value) };
      chNumBox.action = { |me| setNumChannels.value(me.value) };
      rateMenu.action = { |me| setRate.value(me.value) };
      styleMenu.action = { |me| setStyle.value(me.value) };
      view.asView.keyDownAction = { |v, char| this.keyDown(char) };
      view.onClose = { this.stop };

      // LAUNCH

      updateColors.value;
      scopeView.focus;
      if( window.notNil ) { window.front };
    };

    synthStatus = { synth.notNil and: {synth.isPlaying} };

    setCycle = { arg val;
      cycle = val;
      if( synthStatus.value ) {
        synth.set(\frames, val)
      }
    };

    setYZoom = { arg val;
      yZoom = val;
      scopeView.yZoom = yZoomSpec.map(val);
    };

    // NOTE: assuming a single Bus
    setIndex = { arg i;
      bus = Bus(bus.rate, i, bus.numChannels, bus.server);
      if(synthStatus.value) { synth.set(\in, i) };
    };

    // NOTE: assuming a single Bus
    setNumChannels = { arg n;
      // we have to restart the whole thing:
      this.stop;
      bus = Bus(bus.rate, bus.index, n, bus.server);
      this.run;
      updateColors.value;
    };

    // NOTE: assuming a single Bus
    setRate = { arg val;
      val.switch (
        0, {
          bus = Bus(\audio, bus.index, bus.numChannels, bus.server);
          busSpec = aBusSpec;
          if(synthStatus.value) { synth.set(\switch, 0) };
        },
        1, {
          bus = Bus(\control, bus.index, bus.numChannels, bus.server);
          busSpec = cBusSpec;
          if(synthStatus.value) { synth.set(\switch, 1) };
        }
      );
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
        var c = if(b.rate === \audio){rgb(255, 218, 000)}{rgb(125, 255, 205)};
        colors = colors ++ Array.fill(b.numChannels, c);
      };
      scopeView.waveColors = colors;
    };

    makeGui.value(parent);
    this.run;
  }

  run {
    this.stop;

    scopeView.setProperty(\serverPort, port);
    scopeView.invokeMethod(\start);

    if( bus.class === Bus ) {
      synth = SynthDef("stethoscope", { arg in, switch, frames, bufnum;
        var z;
        z = Select.ar(switch, [
          In.ar(in, bus.numChannels.asInteger),
          K2A.ar(In.kr(in, bus.numChannels.asInteger))]
        );
        ScopeOut2.ar(z, bufnum, frames );
      })
      .play(
        RootNode(server),
        [\bufnum, bufnum, \in, bus.index, \frames, cycle,
          \switch, if('audio' === bus.rate) { 0 } { 1 } ],
        \addToTail
      );
    }{
      synth = SynthDef("stethoscope", { arg frames;
        var z = [];
        bus.do { |b| z = z ++ b.ar };
        z.postln;
        ScopeOut2.ar(z, bufnum, frames);
      })
      .play( RootNode(server), [\frames, cycle], \addToTail );
    };
    NodeWatcher.register(synth);
  }

  stop {
    scopeView.invokeMethod(\stop);
    if( synthStatus.value ) { synth.free };
    synth = nil;
  }

  quit {
    this.stop;
    if( view.notNil ) {view.close};
    window = nil;
  }

  isRunning {
    ^scopeView.getProperty(\running) == true;
  }

  bus_ { arg b;
    var isSingle = b.class === Bus;
    var isRunning = this.isRunning;

    if (isRunning) {this.stop};

    bus = b;

    if( isSingle ) {
      rateMenu.value = if(b.rate===\audio){0}{1};
      idxNumBox.value = b.index;
      chNumBox.value = b.numChannels;
      busSpec = if(bus.rate === \audio) {aBusSpec} {cBusSpec};
      idxNumBox.clipLo_(busSpec.minval).clipHi_(busSpec.maxval).value_(bus.index);
    }{
      rateMenu.value = nil;
      idxNumBox.string = "-";
      chNumBox.string = "-";
      busSpec = nil;
    };
    rateMenu.enabled = isSingle;
    idxNumBox.enabled = isSingle;
    chNumBox.enabled = isSingle;

    scopeView.setProperty(\channelCount, this.numChannels);

    updateColors.value;

    if (isRunning) {this.run};
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

  // [0, 1] -> [0.125, 16] y scaling factor
  yZoom_ { arg val;
    setYZoom.value(val);
    yZoomSlider.value = val;
  }

  zoom_ { arg val; this.xZoom_(val ? 1) }

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

  keyDown { arg char;
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
