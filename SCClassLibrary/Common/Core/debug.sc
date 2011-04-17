+ Object {

	debug { arg caller;
		if(caller.notNil,{
			Post << caller << ": " << this << Char.nl;
		},{
			Post << this << Char.nl;
		});
	}
}

+ RawArray { // string, signal

	debug { arg caller;
		if(caller.notNil,{
			Post << caller << ": " << this << Char.nl;
		},{
			Post << this << Char.nl;
		});
	}
}

+ Collection {

	debug { arg caller;
		if(caller.notNil,{
			if(this.size < 10,{
				Post << caller << ": " << this << Char.nl;
			},{
				Post << caller << ": " <<* this << Char.nl;
			});
		},{
			Post <<* this << Char.nl;
		});
	}
}
