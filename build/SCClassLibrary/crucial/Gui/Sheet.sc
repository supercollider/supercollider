

Sheet {
	*new { arg buildDialog,name="",x=100,y=100,width=600,height=600;
		var layout;
		layout = PageLayout(name,width,height,x,y);
		buildDialog.value(layout);
		layout.resizeWindowToFit;
		layout.front;
		^layout
	}
}

ModalDialog { // hit ok or cancel

	*new { arg buildDialog,okFunc,name="?";
	
		Sheet({ arg layout;
			var returnObjects;

			returnObjects=buildDialog.value(layout);
		
			ActionButton(layout,"OK",{
				okFunc.value(returnObjects);
				layout.close;
			});
			
			ActionButton(layout,"Cancel",{
				layout.close;
			});
		
		},name);
	}

}
