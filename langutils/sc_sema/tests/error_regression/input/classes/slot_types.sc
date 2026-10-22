// The slot types the class library uses, which set how an instance's elements are stored, e.g., `String[char]`.
NotIndexed {}
SlotArray[slot] {}
DoubleSlots[double] {}
FloatSlots[float] {}
Int32Slots[int32] {}
Int16Slots[int16] {}
Int8Slots[int8] {}
CharSlots[char] {}
SymbolSlots[symbol] {}

// A subclass doesn't inherit its superclass's slot type, as in the old compiler, so the class library repeats it,
// e.g., `Signal[float] : FloatArray`.
SubOfFloat : FloatSlots {}
RepeatsFloat[float] : FloatSlots {}
