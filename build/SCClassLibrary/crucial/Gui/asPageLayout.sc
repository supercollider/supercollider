
+ Nil {

	asPageLayout { arg name,width,height,x,y,metal=false;
		^PageLayout(name.asString,width,height,x, y, metal: metal )
	}

}