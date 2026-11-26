+ Object {

	debug { |caller|
		if(caller.notNil) {
			Post << caller << ": " << this << Char.nl
		} {
			Post << this << Char.nl
		}
	}
}

+ RawArray { // string, signal

	debug { |caller|
		if(caller.notNil) {
			Post << caller << ": " << this << Char.nl
		} {
			Post << this << Char.nl
		}
	}
}

+ Collection {

	debug { |caller|
		if(caller.notNil) {
			if(this.size < 10) {
				Post << caller << ": " << this << Char.nl
			} {
				Post << caller << ": " <<* this << Char.nl
			}
		} {
			Post <<* this << Char.nl
		}
	}
}
