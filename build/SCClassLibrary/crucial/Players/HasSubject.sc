
HasSubject : AbstractPlayer {

	var <>subject;

	*new { arg subject=0.0;
		^super.new.subject_(subject.loadDocument)
	}
	storeArgs { ^[subject] }

	ar { ^subject.ar }

	children { ^this.inputs }
	inputs { ^[subject] }

	numChannels { ^subject.tryPerform(\numChannels) ? 1 }
	guiClass { ^HasSubjectGui }
}

AbstractPlayerEffect : HasSubject {
	// has two groups so the subject and effect can both respawn at will

	var <subjectGroup,<effectGroup;

	makeResourcesToBundle { arg bundle;
		//move to children make patch out ?
		subjectGroup = Group.basicNew;
		this.annotate(subjectGroup,"subjectGroup");
		effectGroup = Group.basicNew;
		this.annotate(effectGroup,"effectGroup");

		NodeWatcher.register(subjectGroup);
		NodeWatcher.register(effectGroup);

		bundle.add( subjectGroup.addToTailMsg(group) );
		bundle.add( effectGroup.addAfterMsg(subjectGroup) );
	}
	freeResourcesToBundle { arg bundle;
		// this confuses because the subject and effect free themselves,
		// the their enclosing group is also freed.
		subjectGroup.freeToBundle(bundle);
		effectGroup.freeToBundle(bundle);
		subjectGroup = effectGroup = nil;
	}

	spawnToBundle { arg bundle;
		this.children.do({ arg child;
			child.spawnToBundle(bundle);
		});
		synth = Synth.basicNew(this.defName,server);
		this.annotate(synth,"synth");
		NodeWatcher.register(synth);
		bundle.add(
			synth.addToTailMsg(effectGroup,this.synthDefArgs)
		);
		bundle.addMessage(this,\didSpawn);
	}
	preparePlayer { arg player,bus;
		//if(this.bus != bus,{ "Buses are different !".warn; });
		^player.prepareForPlay(subjectGroup,true,bus);
	}
}

