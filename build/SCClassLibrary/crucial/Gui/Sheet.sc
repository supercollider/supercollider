

Sheet {
	*new { arg buildDialog,name="";
		var layout;
		layout = PageLayout(name,600,600,100,100);
		buildDialog.value(layout);
		layout.resizeWindowToFit;
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
