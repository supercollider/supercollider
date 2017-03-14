// old version with gap first
TDuty_old  {
    *ar { arg dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0;
        this.deprecated(thisMethod, TDuty.class.findMethod(\ar));
        ^TDuty.ar(dur, reset, level, doneAction, 1)
    }
    *kr { arg dur = 1.0, reset = 0.0, level = 1.0, doneAction = 0;
        this.deprecated(thisMethod, TDuty.class.findMethod(\kr));
        ^TDuty.kr(dur, reset, level, doneAction, 1)
    }
}
