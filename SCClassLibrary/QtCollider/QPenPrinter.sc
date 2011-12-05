QPenPrinter : QObject {
  var printFunc, cancelFunc, okFunc;

  *qtClass { ^'QcPenPrinter' }

  *new {
    ^super.new.init;
  }

  *print { |printFunc, cancelFunc|
    QPenPrinter().showDialog(
      { |p| p.print(printFunc); p.destroy },
      { |p| cancelFunc.value; p.destroy }
    )
  }

  init {
    this.connectFunction('printFunc()', synchronous:true) {
      printFunc.value(this);
      printFunc = nil;
    };
    this.connectFunction('ok()', synchronous:false) {
      okFunc.value(this);
      okFunc = nil;
    };
    this.connectFunction('cancel()', synchronous:false) {
      cancelFunc.value(this);
      cancelFunc = nil;
    };
  }

  showDialog { |aOkFunc, aCancelFunc|
    okFunc = aOkFunc;
    cancelFunc = aCancelFunc;
    this.invokeMethod(\show, synchronous:false);
  }

  print { |aPrintFunc|
    printFunc = aPrintFunc;
    this.invokeMethod(\print, synchronous:false);
  }

  newPage {
    this.invokeMethod(\newPage, synchronous:true);
  }

  pageRect { ^this.getProperty(\pageRect) }
  paperRect { ^this.getProperty(\paperRect) }
  fromPage { ^this.getProperty(\fromPage) }
  toPage { ^this.getProperty(\toPage) }
  pageSize { ^this.pageRect.size }
}
