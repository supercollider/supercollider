/*
// Helper, ClassHelper, UGenHelper, TopicHelper
// (In this file: Helper)
// ________________________________
//
// Helper chooses  between ClassHelper, UGenHelpe, TopicHelper by verifying
// the class of the passed undocumentedObject. Only Helper must be used directly
// UGenHelper is fitted for ar/kr/ir methods.
// UGens not implementing the rate methods receive as a tag line:
// Name -->no ar/kr/ir methods: sorry, you have to complete as required
//
// Accepts as args the class name (for classes and ugens) or a string describing a topic
// Generates an html and then open it in with openHelpFile
//
// Usage: Helper(undocumentedObject, path)
// where if path.isNil, it prompts for a path where to save as
//
Helper(Helper, "Help/Helper.html")
// ok
Helper(Helper)
// prompt for path (must be complete).
//
Helper(SinOsc, "/Users/andreavalle/Library/Application\ Support/SuperCollider/Extensions/Help/mine/SinOsc.html")
Helper(SinOsc)
// In this case SinOsc has already a help file, so Helper opens that one
// as it calls SinOsc.openHelpFile
//
Helper("On_Helper", "/Users/andreavalle/Library/Application\ Support/SuperCollider/Extensions/Help/mine/On_Helper.html")
Helper("On_Helper")
// it's a bit boring to repeat the class or the topic name with html as a path
// but I don't see a way to propose to GUI save dialog a fileName, so in that case you would have
// to specify it the same.
// Document was indeed more flexible
//
// or with an Help extension:
//
// + Help {
//
//	*makeHelp { arg undocumentedObject, path ;
//		Helper( undocumentedObject )
//
//	}
//
// }
// Help.makeHelp(MyClass)
// e.g.
Help.makeHelp(Helper)
//
// Help.makeHelp(MyUGen)
// e.g.
Help.makeHelp(SinOsc)
//
// Help.makeHelp("MyClass")
// e.g. Help.makeHelp("Helper's corner")
//
// Escapes make the sources totally unreadable...
// andreavalle (started: 04/11/2007)
*/


Helper {

	*new { arg undocumentedObject, path ;
		this.deprecated;
		^super.new.initHelper( undocumentedObject, path )
	}

	initHelper { arg undocumentedObject, path ;
		case
			{ undocumentedObject.class == Meta_Object }
				{ ^ClassHelper.new( undocumentedObject, path )  }
			{ undocumentedObject.class == String }
				{ ^TopicHelper.new( undocumentedObject, path ) }
			{ undocumentedObject.superclasses.includes(UGen).not }
				{ ^ClassHelper.new( undocumentedObject, path ) }
			{ undocumentedObject.superclasses.includes(UGen) }
			  	{ ^UGenHelper.new( undocumentedObject, path ) }
		}
}
