QStethoscope2 {
  // internal functions
  var playSynthDef, makeGui, setCycle, setYZoom, setIndex, setNumChannels,
      setRate, setStyle, updateColors;

  // gui objects
  var <window, <view, <scopeView, cycleSlider, yZoomSlider,
      idxNumBox, chNumBox, styleMenu, rateMenu;

  // static (immutable runtime environment)
  var <server;
  var scopeBuffer, maxBufSize;
  var aBusSpec, cBusSpec, cycleSpec, yZoomSpec;
  var <>smallSize, <>largeSize;

  // runtime (mutable at runtime)
  var <bus; // partly immutable; can't change numChannels at runtime
  var busSpec; // either aBusSpec or cBusSpec, depending on bus rate
  var  <cycle, <yZoom;
  var synth, synthWatcher, defName;
  var sizeToggle=false;
  var running = false;

  *new {
    arg server, numChannels = 2, index = 0, bufsize = 4096,
        cycle = 1024, rate = \audio, view;

    var bus = Bus(rate, index, numChannels, server);
    if(server.isNil) {server = Server.default};

    ^super.new.initQStethoscope( server, view, bus, bufsize, cycle );
  }

  initQStethoscope { arg server_, parent, bus_, bufsize_, cycle_;
    var singleBus;

    server = server_;

    maxBufSize = max(bufsize_, 128);

    bus = bus_;
    singleBus = bus.class === Bus;

    aBusSpec = ControlSpec(0, server.options.numAudioBusChannels, step:1);
    cBusSpec = ControlSpec(0, server.options.numControlBusChannels, step:1);
    if( singleBus ) {
      busSpec = if(bus.rate===\audio){aBusSpec}{cBusSpec};
    };

    cycleSpec = ControlSpec( 64, maxBufSize, \exponential );
    yZoomSpec = ControlSpec( 0.125, 16, \exponential );
    cycle = cycleSpec.constrain(cycle_);
    yZoom = yZoomSpec.unmap(1.0);

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
              idxNumBox.minWidth_(35),
              chNumBox.minWidth_(35),
              nil,
              styleMenu
            ).margins_(0).spacing_(1), 0, 0
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
      view.onClose = { view = nil; this.quit; };

      // LAUNCH

      scopeView.focus;
      if( window.notNil ) { window.front };
    };

    setCycle = { arg val;
      cycle = val;
      if( synth.notNil ) { synth.set(\frames, val) }
    };

    setYZoom = { arg val;
      yZoom = val;
      scopeView.yZoom = yZoomSpec.map(val);
    };

    // NOTE: assuming a single Bus
    setIndex = { arg i;
      bus = Bus(bus.rate, i, bus.numChannels, bus.server);
      if(synth.notNil) { synth.set(\in, i) };
    };

    // NOTE: assuming a single Bus
    setNumChannels = { arg n;
      // we have to restart the whole thing:
      this.stop;
      bus = Bus(bus.rate, bus.index, n, bus.server);
      this.run;
    };

    // NOTE: assuming a single Bus
    setRate = { arg val;
      val.switch (
        0, {
          bus = Bus(\audio, bus.index, bus.numChannels, bus.server);
          busSpec = aBusSpec;
          if(synth.notNil) { synth.set(\switch, 0) };
        },
        1, {
          bus = Bus(\control, bus.index, bus.numChannels, bus.server);
          busSpec = cBusSpec;
          if(synth.notNil) { synth.set(\switch, 1) };
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
        var c = if(b.rate === \audio){Color.new255(255, 218, 000)}{Color.new255(125, 255, 205)};
        colors = colors ++ Array.fill(b.numChannels, c);
      };
      scopeView.waveColors = colors;
    };

    playSynthDef = { arg def, args;
      if( synthWatcher.notNil ) {synthWatcher.stop};
      synthWatcher = fork {
        def.send(server);
        server.sync;
        synth = Synth.tail(RootNode(server), def.name, args);
        CmdPeriod.add(this);
        {if(view.notNil){updateColors.value; scopeView.start}}.defer;
      };
    };

    makeGui.value(parent);

    ServerBoot.add(this, server);
    ServerQuit.add(this, server);
    this.run;
  }

  doOnServerBoot {
      this.run;
  }

  doOnServerQuit {
      this.stop;
      scopeBuffer.free;
      scopeBuffer = nil;
  }

  cmdPeriod {
    synth = nil;
    this.stop;
    CmdPeriod.remove(this);
  }

  run {
    var n_chan;

    if(running || server.serverRunning.not) {^this};

    if(scopeBuffer.isNil){
      scopeBuffer = ScopeBuffer.alloc(server);
      scopeView.bufnum = scopeBuffer.index;
      defName = "stethoscope" ++ scopeBuffer.index.asString;
    };

    n_chan = this.numChannels.asInteger;

    if( bus.class === Bus ) {
      playSynthDef.value (
        SynthDef(defName, { arg in, switch, frames;
          var z;
          z = Select.ar(switch, [
            In.ar(in, n_chan),
            K2A.ar(In.kr(in, n_chan))]
          );
          ScopeOut2.ar(z, scopeBuffer.index, maxBufSize, frames );
        }),
        [\in, bus.index, \switch, if('audio' === bus.rate){0}{1}, \frames, cycle]
      )
    }{
      playSynthDef.value (
        SynthDef(defName, { arg frames;
          var z = Array(n_chan);
          bus.do { |b| z = z ++ b.ar };
          ScopeOut2.ar(z, scopeBuffer.index, maxBufSize, frames);
        }),
        [\frames, cycle]
      );
    };

    running = true;
  }

  stop {
    if( view.notNil ) { {scopeView.stop}.defer };

    if( synthWatcher.notNil ) { synthWatcher.stop };

    if( synth.notNil ) {
      synth.free;
      synth = nil;
    };

    running = false;
  }

  quit {
    var win;
    this.stop;
    ServerBoot.remove(this, server);
    ServerQuit.remove(this, server);
    CmdPeriod.remove(this);
    if(scopeBuffer.notNil) {scopeBuffer.free; scopeBuffer=nil};
    if(window.notNil) { win = window; window = nil; { win.close }.defer; };
  }

  setProperties { arg numChannels, index, bufsize=4096, zoom, rate;
      var new_bus;
      var isRunning = running;

      if (isRunning) {this.stop};

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
      if(zoom.notNil) { yZoom = yZoomSpec.constrain(zoom) };

      // set other vars related to args

      busSpec = if(bus.rate === \audio) {aBusSpec} {cBusSpec};
      cycleSpec = ControlSpec( 64, maxBufSize, \exponential );
      cycle = cycleSpec.constrain(cycle);

      // update GUI

      yZoomSlider.value = yZoom;
      cycleSlider.value = cycleSpec.unmap(cycle);
      rateMenu.value_(if(bus.rate === \audio){0}{1}).enabled_(true);
      idxNumBox.clipLo_(busSpec.minval).clipHi_(busSpec.maxval).value_(bus.index).enabled_(true);
      chNumBox.value_(bus.numChannels).enabled_(true);
      scopeView.yZoom = yZoomSpec.map(yZoom);

      if (isRunning) {this.run};
  }

  bufsize { ^maxBufSize }

  bus_ { arg b;
    var isSingle = b.class === Bus;
    var isRunning = running;

    if (isRunning) {this.stop};

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
