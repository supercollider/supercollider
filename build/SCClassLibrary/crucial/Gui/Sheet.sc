

Sheet {
	*new { arg buildDialog,name="",x=100,y=100,width=600,height=600;
		var layout;
		layout = MultiPageLayout(name,width,height,x,y);
		//layout = FlowView(nil,Rect(x,y,width,height));
		buildDialog.value(layout);
		layout.resizeToFit;
		layout.front;
		^layout
	}
}

ModalDialog { // hit ok or cancel

	*new { arg buildDialog,okFunc,name="?";
	
		Sheet({ arg layout;
			var returnObjects;

			returnObjects=buildDialog.value(layout);
		
			layout.startRow;
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
