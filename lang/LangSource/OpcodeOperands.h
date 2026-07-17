#pragma once
#include "ByteCodeArray.h"
#include "OpcodeDetails.h"

namespace Operands::details {

// Types of Operand

struct NamedByte {
    Byte value;
    constexpr operator Byte() const { return value; }
    constexpr int asInt() const { return static_cast<int>(value); }
};

template <typename Enum_T> struct OperandEnumWrapper {
    using UnderlyingEnum = Enum_T;
    static_assert(std::is_convertible_v<std::underlying_type_t<Enum_T>, Byte>);
    static constexpr Byte COUNT = (Byte)Enum_T::COUNT;
    constexpr OperandEnumWrapper(Enum_T e) noexcept: value(e) {}
    Enum_T value;
    explicit constexpr operator Byte() const { return static_cast<Byte>(value); }
    constexpr operator Enum_T() const { return value; }
};


}

// Named enums with COUNT

enum struct OpUnaryMathNibble : Byte {
    Neg,
    Not,
    IsNil,
    NotNil,
    BitNot,
    Abs,
    AsFloat,
    AsInteger,
    Ceil,
    Floor,
    Frac,
    Sign,
    Squared,
    Cubed,
    Sqrt,
    Exp,
    COUNT
};

enum struct OpBinaryMathNibble : Byte {
    Add,
    Sub,
    Mul,
    IDiv,
    FDiv,
    Mod,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    Min,
    Max,
    BitAnd,
    BitOr,
    COUNT
};

enum struct OpSpecialClassEnum : Byte {
    Object,
    Symbol,
    Nil_,
    Boolean,
    True,
    False,
    Magnitude,
    Char,
    Number,
    Complex,
    Simple_number,
    Int,
    Float,
    Method,
    Fundef,
    Stream,
    Func,
    Frame,
    Process,
    Main,
    Class,
    String,
    Collection,
    Sequenceable_collection,
    Arrayed_collection,
    Array,
    Int8array,
    Int16array,
    Int32array,
    Floatarray,
    Signal,
    Doublearray,
    Symbolarray,
    List,
    Linkedlist,
    Bag,
    Set,
    Identityset,
    Dictionary,
    Identitydictionary,
    Sortedlist,
    Synth,
    Ref,
    Environment,
    Event,
    Wavetable,
    Env,
    Routine,
    Color,
    Rect,
    COUNT
};

enum struct OpPseudoVarEnum : Byte {
    Process,
    Method,
    FunctionDef,
    Function,
    Thread,
    NumPseudoVars,
    COUNT,
};

enum struct OpSpecialNumbers : Byte {
    MinusOne,
    Zero,
    One,
    Two,
    Half,
    MinusOneFloat,
    ZeroFloat,
    OneFloat,
    TwoFloat,
    COUNT
};

enum struct OpSpecialSelectors : Byte {
    New,
    Init,
    At,
    Put,
    Next,
    Reset,
    Value,
    CopyToEnd,
    Add,
    Size,
    Class,
    If,
    While,
    For,
    And,
    Or,
    Case,
    Switch,
    Identical,
    NotIdentical,
    Print,
    Remove,
    IndexOf,
    WrapAt,
    ClipAt,
    FoldAt,
    WrapPut,
    ClipPut,
    FoldPut,
    Do,
    Collect,
    Select,
    Reject,
    Any,
    Every,
    Find,
    Choose,
    ValueList,
    AddFirst,
    PrimitiveFailed,
    SubclassResponsibility,
    ShouldNotImplement,
    NotYetImplemented,
    DoesNotUnderstand,
    AtSign,
    WrapAtSign,
    ClipAtSign,
    FoldAtSign,
    NewClear,
    NewCopyArgs,
    MultiNew,
    MultiNewList,
    AR,
    KR,
    IR,
    Copy,
    PerformList,
    IsKindOf,
    Postln,
    AsString,
    EnvirGet,
    EnvirPut,
    Halt,
    ForBy,
    ForSeries,
    ReverseDo,
    Loop,
    NonBooleanError,
    PlusPlus,
    LTLT,
    QuestionMark,
    DoubleQuestionMark,
    ExclamationQuestionMark,
    Yield,
    Name,
    MulAdd,
    Series,
    COUNT
};

enum struct OpSpecialValue : Byte {
    True,
    False,
    Nil_,
    Inf,
    COUNT,
};

enum struct OpUnaryMath : Byte {
    Neg,
    Not,
    IsNil,
    NotNil,
    BitNot,
    Abs,
    AsFloat,
    AsInteger,
    Ceil,
    Floor,
    Frac,
    Sign,
    Squared,
    Cubed,
    Sqrt,
    Exp,
    Recip,
    MIDICPS,
    CPSMIDI,
    MIDIRatio,
    RatioMIDI,
    DbAmp,
    AmpDb,
    OctCPS,
    CPSOct,
    Log,
    Log2,
    Log10,
    Sin,
    Cos,
    Tan,
    ArcSin,
    ArcCos,
    ArcTan,
    SinH,
    CosH,
    TanH,
    Rand,
    Rand2,
    LinRand,
    BiLinRand,
    Sum3Rand,
    Distort,
    SoftClip,
    Coin,
    DigitValue,
    Silence,
    Thru,
    RectWindow,
    HanWindow,
    WelchWindow,
    TriWindow,
    Ramp,
    SCurve,
    COUNT
};

enum struct OpBinaryMath : Byte {
    Add,
    Sub,
    Mul,
    IDiv,
    FDiv,
    Mod,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    Min,
    Max,
    BitAnd,
    BitOr,
    BitXor,
    LCM,
    GCD,
    Round,
    RoundUp,
    Trunc,
    Atan2,
    Hypot,
    Hypotx,
    Pow,
    ShiftLeft,
    ShiftRight,
    UnsignedShift,
    Fill,
    Ring1, // a * (b + 1) == a * b + a
    Ring2, // a * b + a + b
    Ring3, // a*a*b
    Ring4, // a*a*b - a*b*b
    DifSqr, // a*a - b*b
    SumSqr, // a*a + b*b
    SqrSum, // (a + b)^2
    SqrDif, // (a - b)^2
    AbsDif, // |a - b|
    Thresh,
    AMClip,
    ScaleNeg,
    Clip2,
    Excess,
    Fold2,
    Wrap2,
    FirstArg,
    RandRange,
    ExpRandRange,
    COUNT
};

enum struct OpTrinaryMath : Byte {
    Divz,
    Clip,
    Wrap,
    Fold,
    RampMult,
    Mix,
    COUNT,
};

namespace Operands {
struct Index : Operands::details::NamedByte {
    static constexpr const char* name = "Index";
    constexpr static Index fromRaw(int b) { return { Opcode::details::to_byte(b) }; }
    constexpr static Index fromRaw(Byte b) { return { b }; }
};
struct FrameOffset : Operands::details::NamedByte {
    static constexpr const char* name = "FrameOffset";
    constexpr static FrameOffset fromRaw(int b) { return { Opcode::details::to_byte(b) }; }
    constexpr static FrameOffset fromRaw(Byte b) { return { b }; }
};
struct Class : Operands::details::NamedByte {
    static constexpr const char* name = "Class";
    constexpr static Class fromRaw(int b) { return { Opcode::details::to_byte(b) }; }
    constexpr static Class fromRaw(Byte b) { return { b }; }
};
struct ArgumentCount : Operands::details::NamedByte {
    static constexpr const char* name = "ArgumentCount";
    constexpr static ArgumentCount fromRaw(int b) { return { Opcode::details::to_byte(b) }; }
    constexpr static ArgumentCount fromRaw(Byte b) { return { b }; }
};
struct KwArgumentCount : Operands::details::NamedByte {
    static constexpr const char* name = "KwArgumentCount";
    constexpr static KwArgumentCount fromRaw(int b) { return { Opcode::details::to_byte(b) }; }
    constexpr static KwArgumentCount fromRaw(Byte b) { return { b }; }
};

template <unsigned int TOTAL, unsigned int PART> struct UnsignedInt {
    static_assert(8 * PART < TOTAL);
    static constexpr const char* name = "UnsignedInt";
    constexpr static UnsignedInt<TOTAL, PART> fromRaw(Byte b) { return { b }; }

    constexpr static UnsignedInt<TOTAL, PART> fromFull(unsigned int i) {
        assert(i < (1ULL << TOTAL));
        return { Opcode::details::to_byte(i >> (8U * PART)) };
    }

    constexpr static UnsignedInt<TOTAL, PART> fromFull(int i) {
        assert(i >= 0);
        return UnsignedInt::fromFull(static_cast<unsigned int>(i));
    }

    template <typename... TS> unsigned int asInt(const TS&... ts) const {
        static_assert((PART + 1U) * 8U == TOTAL, "Can only call get on the highest byte");
        static_assert(sizeof...(TS) == PART, "Not all parts were provided");
        return asIntPart() | (ts.asIntPart() | ...);
    }
    unsigned int asInt() const {
        static_assert(TOTAL == 8);
        return static_cast<unsigned int>(value);
    }

    unsigned int asIntPart() const { return static_cast<unsigned int>(value) << (PART * 8U); }

    Byte value;
    explicit constexpr operator Byte() const { return value; }
};

template <unsigned int TOTAL, unsigned int PART> struct Int {
    static_assert(8 * PART < TOTAL);
    static constexpr unsigned int PARTSIZE = TOTAL / 8;
    static constexpr const char* name = "Int";
    constexpr static Int<TOTAL, PART> fromRaw(Byte b) { return { b }; }

    static constexpr unsigned int down_shift = 32U - TOTAL;
    static constexpr unsigned int up_shift = 32U - (((PARTSIZE - PART)) * 8U);

    constexpr static Int<TOTAL, PART> fromFull(int i) {
        assert(i >= -(1LL << (TOTAL - 1)) || i <= (1LL << (TOTAL - 1)) - 1);
        return { Opcode::details::to_byte(i >> (8 * PART)) };
    }

    template <typename... TS> int asInt(TS... ts) const {
        static_assert((PART + 1) * 8 == TOTAL, "Can only call get on the highest byte");
        static_assert(sizeof...(TS) == PART, "Not all parts were provided");
        static constexpr unsigned int down_shift = 32U - TOTAL;
        return (asIntPart() | (ts.asIntPart() | ...)) >> down_shift;
    }
    int asInt() const {
        static_assert(TOTAL == 8);
        return asIntPart() >> down_shift;
    }

    int asIntPart() const { return static_cast<int>(value) << up_shift; }

    Byte value;
    explicit constexpr operator Byte() const { return value; }
};


struct SpecialClass : Operands::details::OperandEnumWrapper<OpSpecialClassEnum> {
    constexpr SpecialClass(OpSpecialClassEnum s) noexcept:
        Operands::details::OperandEnumWrapper<OpSpecialClassEnum>(s) {}
    static constexpr const char* name = "SpecialClass";
    constexpr static SpecialClass fromRaw(int b) { return { Opcode::details::to_enum<OpSpecialClassEnum>(b) }; }
};
struct PseudoVar : Operands::details::OperandEnumWrapper<OpPseudoVarEnum> {
    constexpr PseudoVar(OpPseudoVarEnum s) noexcept: Operands::details::OperandEnumWrapper<OpPseudoVarEnum>(s) {}
    static constexpr const char* name = "PseudoVar";
    constexpr static PseudoVar fromRaw(int b) { return { Opcode::details::to_enum<OpPseudoVarEnum>(b) }; }
};

struct SpecialSelectors : Operands::details::OperandEnumWrapper<OpSpecialSelectors> {
    constexpr SpecialSelectors(OpSpecialSelectors s) noexcept:
        Operands::details::OperandEnumWrapper<OpSpecialSelectors>(s) {}
    static constexpr const char* name = "SpecialSelectors";
    constexpr static SpecialSelectors fromRaw(int b) { return { Opcode::details::to_enum<OpSpecialSelectors>(b) }; }
};

struct UnaryMath : Operands::details::OperandEnumWrapper<OpUnaryMath> {
    constexpr UnaryMath(OpUnaryMath s) noexcept: Operands::details::OperandEnumWrapper<OpUnaryMath>(s) {}
    static constexpr const char* name = "UnaryMath";
    constexpr static UnaryMath fromRaw(int b) { return { Opcode::details::to_enum<OpUnaryMath>(b) }; }
};

struct UnaryMathNibble : Operands::details::OperandEnumWrapper<OpUnaryMathNibble> {
    constexpr UnaryMathNibble(OpUnaryMathNibble n) noexcept:
        Operands::details::OperandEnumWrapper<OpUnaryMathNibble>(n) {}
    static constexpr const char* name = "UnaryMathNibble";
    constexpr static UnaryMathNibble fromRaw(int b) { return { Opcode::details::to_enum<OpUnaryMathNibble>(b) }; }
};
struct BinaryMath : Operands::details::OperandEnumWrapper<OpBinaryMath> {
    constexpr BinaryMath(OpBinaryMath s) noexcept: Operands::details::OperandEnumWrapper<OpBinaryMath>(s) {}
    static constexpr const char* name = "BinaryMath";
    constexpr static BinaryMath fromRaw(int b) { return { Opcode::details::to_enum<OpBinaryMath>(b) }; }
};
struct TrinaryMath : Operands::details::OperandEnumWrapper<OpTrinaryMath> {
    constexpr TrinaryMath(OpTrinaryMath s) noexcept: Operands::details::OperandEnumWrapper<OpTrinaryMath>(s) {}
    static constexpr const char* name = "TrinaryMath";
    constexpr static TrinaryMath fromRaw(int b) { return { Opcode::details::to_enum<OpTrinaryMath>(b) }; }
};
struct BinaryMathNibble : Operands::details::OperandEnumWrapper<OpBinaryMathNibble> {
    constexpr BinaryMathNibble(OpBinaryMathNibble s) noexcept:
        Operands::details::OperandEnumWrapper<OpBinaryMathNibble>(s) {}
    static constexpr const char* name = "BinaryMathNibble";
    constexpr static BinaryMathNibble fromRaw(int b) { return { Opcode::details::to_enum<OpBinaryMathNibble>(b) }; }
};

struct SpecialValue : Operands::details::OperandEnumWrapper<OpSpecialValue> {
    constexpr SpecialValue(OpSpecialValue s) noexcept: Operands::details::OperandEnumWrapper<OpSpecialValue>(s) {}
    constexpr static SpecialValue fromRaw(int b) { return { Opcode::details::to_enum<OpSpecialValue>(b) }; }
    static constexpr const char* name = "SpecialValue";
};
struct SpecialNumber : Operands::details::OperandEnumWrapper<OpSpecialNumbers> {
    constexpr SpecialNumber(OpSpecialNumbers s) noexcept: Operands::details::OperandEnumWrapper<OpSpecialNumbers>(s) {}
    constexpr static SpecialNumber fromRaw(int b) { return { Opcode::details::to_enum<OpSpecialNumbers>(b) }; }
    static constexpr const char* name = "SpecialNumber";
};
};
