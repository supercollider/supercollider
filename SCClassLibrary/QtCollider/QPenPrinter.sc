QPenPrinter : QObject {
  var printFunc, cancelFunc, okFunc;

  *qtClass { ^'QcPenPrinter' }

  *new {
    ^super.new.init;
  }

  *print { |printFunc, cancelFunc|
    this.new.showDialog( { |p| p.print(printFunc) }, cancelFunc);
  }

  init {
    heap.remove(this);
    this.connectFunction('printFunc()', synchronous:true) {
      printFunc.value(this);
      printFunc = nil;
      heap.remove(this);
    };
    this.connectFunction('dialogDone(int)', synchronous:false) { |me, ok|
      if( ok == 1 ) {
        okFunc.value(this);
      } {
        cancelFunc.value(this);
      };
      okFunc = nil;
      cancelFunc = nil;
      heap.remove(this);
    };
  }

  showDialog { |aOkFunc, aCancelFunc|
    if(okFunc.notNil or: cancelFunc.notNil) {
      "QPenPrinter: dialog already open".warn;
      ^this;
    };
    okFunc = aOkFunc;
    cancelFunc = aCancelFunc;
    heap = heap.add(this);
    this.invokeMethod(\show, synchronous:false);
  }

  print { |aPrintFunc|
    if(printFunc.notNil) {
      "QPenPrinter: printing already in progress".warn;
      ^this;
    };
    printFunc = aPrintFunc;
    heap = heap.add(this);
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
