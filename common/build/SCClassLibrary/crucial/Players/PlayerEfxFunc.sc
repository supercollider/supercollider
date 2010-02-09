/*

PlayerEfxFunc : AbstractSinglePlayerEffect {

	var <>effect,inputIndex,playerInputProxy;

	*new { arg player,effect,inputIndex=0;
		^super.new(player).effect_(effect.loadDocument).peinit(inputIndex)
	}
	peinit { arg index;
		var proxies;
		proxies = effect.annotatedInputProxies;
		if(proxies.isEmpty,{
			("PlayerEfxFunc-peinit : No possible input found on effect " + effect).die;
		});
		inputIndex = index;
		playerInputProxy = proxies.at(inputIndex).at(0);
	}
	prepareToBundle { arg group,bundle;
		super.prepareToBundle(group,bundle);
		effect.prepareToBundle(effectGroup,bundle);
	}
	loadDefFileToBundle { } // really its Synthless or proxy

	makePatchOut { arg parentGroup,private,bus,bundle;
		// the subject
		super.makePatchOut(parentGroup,private,bus,bundle);
		effect.makePatchOut(effectGroup,true,this.bus,bundle);
		// effect reads from subject's bus
		playerInputProxy.numChannels_(subject.numChannels).spec_(subject.spec);
		playerInputProxy.setInputBus(this.bus);
	}
	spawnToBundle { arg bundle;
		effect.spawnToBundle(bundle);
		subject.spawnToBundle(bundle);
	}
	isPlaying { ^effect.isPlaying }

	children { ^[subject,effect] }
	guiClass { ^PlayerEffectGui }
}

*/

/*

XFadePauseEfxFunc : XFadeEfxFunc { // pauses the effect when xfader is 0.0
	// pre1 is the pre level, pre2 is the post level
	ar {
		var dry,wet,controller;
		controller = xfader.value;
		dry = one.value;
		wet = Pause.ar({ two.value(if(pre1 == 1.0,{ dry},{ dry * pre1.value })) },controller);
		if(pre2 != 1.0,{ wet = wet * pre2.value });
		wet = wet * (1.0 - controller);
		^Mix.ar([dry,wet])//XFader.ar(dry,wet,xfader.value)
	}

}

DualSeriesEfx : HasPatchIns {

	var <input,<efx1,<efx2,<xfader1,<xfader2,<pre1,<pre2,<post;
	var <xf1,<xf2;

	*new { arg input,efx1,efx2,xfader1,xfader2,pre1=1.0,pre2=1.0,post=1.0;
		^super.new.deinit(this.load(input),this.load(efx1),this.load(efx2),
				this.load(xfader1),this.load(xfader2),this.load(pre1),this.load(pre2),this.load(post))
	}
	deinit { arg in,e1,e2,x1,x2,p1,p2,po;
		input = in;
		efx1 = e1;
		efx2 = e2;
		xfader1 = x1;
		xfader2 = x2;
		pre1 = p1;
		pre2 = p2;
		post = po;
		xf1 = XFadeEfxFunc(in,efx1,xfader1,pre1,1.0);
		xf2 = XFadeEfxFunc(xf1,efx2,xfader2,pre2,post);
	}
	ar { arg newinput;
		^xf2.ar( xf1.ar(newinput) )
	}
	children { ^[input,efx1,efx2,xfader1,xfader2,pre1,pre2,post] }
	inputs { ^this.children } //{[input,xfader1,xfader2,pre1,pre2,post] }
	argNameAt { arg i; ^(#[\input,\efx1,\efx2,\xfader1,\xfader2,\pre1,\pre2,\post].at(i)) }
	setInput { arg ai,ag;
		if(ai == 0,{
			input = ag;
			^this
		});
		if(ai == 1,{
			efx1 = ag;
			^this
		});
		if(ai == 2,{
			efx2 = ag;
			^this
		});
		if(ai == 3,{
			xfader1 = ag;
			^this
		});
		if(ai == 4,{
			xfader2 = ag;
			^this
		});
		if(ai == 5,{
			pre1 = ag;
			^this
		});
		if(ai == 6,{
			pre2 = ag;
			^this
		});
		if(ai == 7,{
			post = ag;
			^this
		});
	}
	specAt { arg i; // using StreamSpec to avoid creating PlayerSpec just yet
		^([\audio,StreamSpec.new,StreamSpec.new,\unipolar,\unipolar,\amp,\amp,\amp].at(i).asSpec)
	}

	storeParamsOn { arg stream;
		stream.storeArgs(this.enpath([input,efx1,efx2,xfader1,xfader2,pre1,pre2,post]))
	}
	*guiClass { ^DualSeriesEfxGui }

}

*/
