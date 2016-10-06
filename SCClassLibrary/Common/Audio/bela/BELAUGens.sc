/*
 * BELAUGens to access the analog and digital I/O
 *  created by nescivi, (c) 2016
 *  https://www.nescivi.eu
 */

/* input: id of analog pin to read; can be modulated at audiorate
 * output: value of analog analogPin
 */
AnalogIn : UGen {
    signalRange { ^\unipolar }
    
    *ar { arg analogPin = 0, mul=1.0, add=0.0;
        ^this.multiNew('audio', analogPin ).madd(mul,add)
    }
    *kr { arg analogPin = 0, mul=1.0, add=0.0;
        ^this.multiNew('control', analogPin ).madd(mul,add)
    }
}

/* input 1: id of analog pin to read; can be modulated at audiorate
 * input 2: value to write out
 * output: none
 */
AnalogOut : UGen {
    *ar { arg analogPin = 0, output=0, mul=1.0, add=0.0;
        ^this.multiNew('audio', analogPin, output ).madd(mul,add)
    }
    *kr { arg analogPin = 0, output=0, mul=1.0, add=0.0;
        ^this.multiNew('control', analogPin, output ).madd(mul,add)
    }
}

/* input: id of digital pin to read; cannot be modulated
 * output: value of digital pin
 */
DigitalIn : UGen {
    signalRange { ^\unipolar }
    
    *ar { arg digitalPin = 0, mul=1.0, add=0.0;
        ^this.multiNew('audio', digitalPin ).madd(mul,add)
    }
    *kr { arg digitalPin = 0, mul=1.0, add=0.0;
        ^this.multiNew('control', digitalPin ).madd(mul,add)
    }
}

/* input 1: id of digital pin to read; cannot be modulated
 * input 2: value to write out
 * output: none
 */
DigitalOut : UGen {
    *ar { arg digitalPin = 0, output=0, writeMode=0, mul=1.0, add=0.0;
        ^this.multiNew('audio', digitalPin, output, writeMode ).madd(mul,add)
    }
    *kr { arg digitalPin = 0, output=0, writeMode=0, mul=1.0, add=0.0;
        ^this.multiNew('control', digitalPin, output, writeMode ).madd(mul,add)
    }
}

/* input 1: id of digital pin to read; cannot be modulated
 * input 2: value to write out
 * input 3: pin mode ( < 0.5 = input, otherwise output)
 * output: value of digital pin (last read value)
 */
DigitalIO : UGen {
    signalRange { ^\unipolar }
    
    *ar { arg digitalPin = 0, output=0, pinMode=0, mul=1.0, add=0.0;
        ^this.multiNew('audio', digitalPin, output, pinMode ).madd(mul,add)
    }
    *kr { arg digitalPin = 0, output=0, pinMode=0, mul=1.0, add=0.0;
        ^this.multiNew('control', digitalPin, output, pinMode ).madd(mul,add)
    }
    
}
