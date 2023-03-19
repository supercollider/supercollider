// backward compatible version of SoundIn.
// The only difference is that it counts from channel 1.

AudioIn : SoundIn  {
    *ar { arg channel = 0, mul=1.0, add=0.0;
        this.deprecated(thisMethod, SoundIn.class.findMethod(\ar));
        ^super.ar(channel, mul, add)
    }
    *channelOffset {
        ^NumOutputBuses.ir - 1
    }
}
