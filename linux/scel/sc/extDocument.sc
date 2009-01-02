// copyright 2003 stefan kersten <steve@k-hornz.de>
// changes 2008/9 nescivi <nescivi@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA


+ Document {
	// Document class for Emacs interface.
	//	
	// Delegates to ScelDocument, which delegates to EmacsDocument.

	// moved to Emacs, upon startup
	//  *implementationClass { ^ScelDocument }

	// PRIVATE
	//	*newFromIndex { ^this.shouldNotImplement(thisMethod) }
	*prGetLast { ^allDocuments.last }
}


+ String{
	/*
	findHelpFile {
		if ( Emacs.initialized) {
			Emacs.evalLispExpression(['sclang-find-help', this].asLispString);
		}
	}
	*/

	openHelpFile {
		if ( Emacs.initialized) {
			Emacs.evalLispExpression(['sclang-find-help', this].asLispString);
			//		this.findHelpFile;
		}
	}

	openHTMLFile {
		if ( Emacs.initialized) {
		//		this.findHelpFile;
			Emacs.evalLispExpression(['w3m-browse-url', this].asLispString);
		}
	}
}

+ Class{
	openHelpFile {
		if ( Emacs.initialized) {
			this.asString.openHelpFile;
		}
	}
}

/*
+ Method{

	openHelpFile {
		if ( Emacs.initialized) {
			this.asString.openHelpFile;
		}
	}
}
*/
 


// EOF
