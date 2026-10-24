// bytecode
//# VERSION 3.16
// A message's argument count is a byte, counting the receiver, and two for each keyword argument.
BytecodeTooManyArguments {
    // expect-error: Too many arguments
    many { |a| ^a.foo(k0: 0, k1: 0, k2: 0, k3: 0, k4: 0, k5: 0, k6: 0, k7: 0, k8: 0, k9: 0, k10: 0, k11: 0, k12: 0, k13: 0, k14: 0, k15: 0, k16: 0, k17: 0, k18: 0, k19: 0, k20: 0, k21: 0, k22: 0, k23: 0, k24: 0, k25: 0, k26: 0, k27: 0, k28: 0, k29: 0, k30: 0, k31: 0, k32: 0, k33: 0, k34: 0, k35: 0, k36: 0, k37: 0, k38: 0, k39: 0, k40: 0, k41: 0, k42: 0, k43: 0, k44: 0, k45: 0, k46: 0, k47: 0, k48: 0, k49: 0, k50: 0, k51: 0, k52: 0, k53: 0, k54: 0, k55: 0, k56: 0, k57: 0, k58: 0, k59: 0, k60: 0, k61: 0, k62: 0, k63: 0, k64: 0, k65: 0, k66: 0, k67: 0, k68: 0, k69: 0, k70: 0, k71: 0, k72: 0, k73: 0, k74: 0, k75: 0, k76: 0, k77: 0, k78: 0, k79: 0, k80: 0, k81: 0, k82: 0, k83: 0, k84: 0, k85: 0, k86: 0, k87: 0, k88: 0, k89: 0, k90: 0, k91: 0, k92: 0, k93: 0, k94: 0, k95: 0, k96: 0, k97: 0, k98: 0, k99: 0, k100: 0, k101: 0, k102: 0, k103: 0, k104: 0, k105: 0, k106: 0, k107: 0, k108: 0, k109: 0, k110: 0, k111: 0, k112: 0, k113: 0, k114: 0, k115: 0, k116: 0, k117: 0, k118: 0, k119: 0, k120: 0, k121: 0, k122: 0, k123: 0, k124: 0, k125: 0, k126: 0, k127: 0) }
    // expect: no diagnostics
    few { |a| ^a.foo(k0: 0) }
}
