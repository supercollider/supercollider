Module {
	var server, name, id, values, presets, presetIndex, <isOn = false;

	*new { arg server, name, id;
		^super.newCopyArgs(server, name, id).init
	}
	init {
		this.load;
	}
	store {
		if (presetIndex > presets.size, {
			presets = presets.add(values);
		});
		presets.put(presetIndex, values);
	}
	recall {
		if (presetIndex < presets.size, {
			values = presets.at(presetIndex).copy;
			// set gui elems and synth controls
		});
	}
	save {
		presets.writeArchive("server/presets/" ++ name ++ ".scpreset");
	}
	load {
		var filename;
		filename = "server/presets/" ++ name ++ ".scpreset";
		if (File.exists(filename), {
			presets = Object.readArchive(filename);
		});
	}
	start {
		var msg;
		isOn = true;
		msg = ["/s_new", name, id, 0];
		values.keysValuesDo({ arg key, value; msg = msg.addAll([key, value]); });
		server.addr.sendBundle(nil, msg);
	}
	stop {
		isOn = false;
		server.sendMsg("/n_set", id, \gate, 0);
	}
}
