#pragma once
#include "ByteCodeArray.h"
#include "OpcodeDetails.h"
namespace Operands::details {

// Types of Operand

template <typename CRTP> struct NamedByte {
    Byte value;

    constexpr operator Byte() const { return value; }
    constexpr int asInt() const { return static_cast<int>(value); }

    constexpr static CRTP fromRaw(int b) { return { Opcode::details::to_byte(b) }; }
    constexpr static CRTP fromRaw(Byte b) { return { b }; }
};

template <typename Enum_T, typename CRTP> struct OperandEnumWrapper {
    using UnderlyingEnum = Enum_T;
    static_assert(std::is_convertible_v<std::underlying_type_t<Enum_T>, Byte>);
    static constexpr Byte COUNT = (Byte)Enum_T::COUNT;
    Enum_T value;

    explicit constexpr operator Byte() const { return static_cast<Byte>(value); }
    constexpr operator Enum_T() const { return value; }

    constexpr static CRTP fromRaw(int b) { return { Opcode::details::to_enum<Enum_T>(b) }; }
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

[[nodiscard]] inline constexpr const char* asName(OpUnaryMathNibble n) noexcept {
    switch (n) {
    case OpUnaryMathNibble::Neg:
        return "neg";
    case OpUnaryMathNibble::Not:
        return "not";
    case OpUnaryMathNibble::IsNil:
        return "isNil";
    case OpUnaryMathNibble::NotNil:
        return "notNil";
    case OpUnaryMathNibble::BitNot:
        return "bitNot";
    case OpUnaryMathNibble::Abs:
        return "abs";
    case OpUnaryMathNibble::AsFloat:
        return "asFloat";
    case OpUnaryMathNibble::AsInteger:
        return "asInteger";
    case OpUnaryMathNibble::Ceil:
        return "ceil";
    case OpUnaryMathNibble::Floor:
        return "floor";
    case OpUnaryMathNibble::Frac:
        return "frac";
    case OpUnaryMathNibble::Sign:
        return "sign";
    case OpUnaryMathNibble::Squared:
        return "squared";
    case OpUnaryMathNibble::Cubed:
        return "cubed";
    case OpUnaryMathNibble::Sqrt:
        return "sqrt";
    case OpUnaryMathNibble::Exp:
        return "exp";
    default:
        assert(false);
        return "unexpected value";
    }
}

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

[[nodiscard]] inline constexpr const char* asName(OpBinaryMathNibble n) noexcept {
    switch (n) {
    case OpBinaryMathNibble::Add:
        return "add";
    case OpBinaryMathNibble::Sub:
        return "sub";
    case OpBinaryMathNibble::Mul:
        return "mul";
    case OpBinaryMathNibble::IDiv:
        return "iDiv";
    case OpBinaryMathNibble::FDiv:
        return "fDiv";
    case OpBinaryMathNibble::Mod:
        return "mod";
    case OpBinaryMathNibble::EQ:
        return "==";
    case OpBinaryMathNibble::NE:
        return "!=";
    case OpBinaryMathNibble::LT:
        return "<";
    case OpBinaryMathNibble::GT:
        return ">";
    case OpBinaryMathNibble::LE:
        return "<=";
    case OpBinaryMathNibble::GE:
        return ">=";
    case OpBinaryMathNibble::Min:
        return "min";
    case OpBinaryMathNibble::Max:
        return "max";
    case OpBinaryMathNibble::BitAnd:
        return "bitAnd";
    case OpBinaryMathNibble::BitOr:
        return "bitOr";
    default:
        assert(false);
        return "unexpected value";
    }
}

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

[[nodiscard]] inline constexpr const char* asName(OpSpecialClassEnum n) noexcept {
    switch (n) {
    case OpSpecialClassEnum::Object:
        return "Object";
    case OpSpecialClassEnum::Symbol:
        return "Symbol";
    case OpSpecialClassEnum::Nil_:
        return "Nil";
    case OpSpecialClassEnum::Boolean:
        return "Boolean";
    case OpSpecialClassEnum::True:
        return "True";
    case OpSpecialClassEnum::False:
        return "False";
    case OpSpecialClassEnum::Magnitude:
        return "Magnitude";
    case OpSpecialClassEnum::Char:
        return "Char";
    case OpSpecialClassEnum::Number:
        return "Number";
    case OpSpecialClassEnum::Complex:
        return "Complex";
    case OpSpecialClassEnum::Simple_number:
        return "Simple_number";
    case OpSpecialClassEnum::Int:
        return "Int";
    case OpSpecialClassEnum::Float:
        return "Float";
    case OpSpecialClassEnum::Method:
        return "Method";
    case OpSpecialClassEnum::Fundef:
        return "Fundef";
    case OpSpecialClassEnum::Stream:
        return "Stream";
    case OpSpecialClassEnum::Func:
        return "Func";
    case OpSpecialClassEnum::Frame:
        return "Frame";
    case OpSpecialClassEnum::Process:
        return "Process";
    case OpSpecialClassEnum::Main:
        return "Main";
    case OpSpecialClassEnum::Class:
        return "Class";
    case OpSpecialClassEnum::String:
        return "String";
    case OpSpecialClassEnum::Collection:
        return "Collection";
    case OpSpecialClassEnum::Sequenceable_collection:
        return "Sequenceable_collection";
    case OpSpecialClassEnum::Arrayed_collection:
        return "Arrayed_collection";
    case OpSpecialClassEnum::Array:
        return "Array";
    case OpSpecialClassEnum::Int8array:
        return "Int8array";
    case OpSpecialClassEnum::Int16array:
        return "Int16array";
    case OpSpecialClassEnum::Int32array:
        return "Int32array";
    case OpSpecialClassEnum::Floatarray:
        return "Floatarray";
    case OpSpecialClassEnum::Signal:
        return "Signal";
    case OpSpecialClassEnum::Doublearray:
        return "Doublearray";
    case OpSpecialClassEnum::Symbolarray:
        return "Symbolarray";
    case OpSpecialClassEnum::List:
        return "List";
    case OpSpecialClassEnum::Linkedlist:
        return "Linkedlist";
    case OpSpecialClassEnum::Bag:
        return "Bag";
    case OpSpecialClassEnum::Set:
        return "Set";
    case OpSpecialClassEnum::Identityset:
        return "Identityset";
    case OpSpecialClassEnum::Dictionary:
        return "Dictionary";
    case OpSpecialClassEnum::Identitydictionary:
        return "Identitydictionary";
    case OpSpecialClassEnum::Sortedlist:
        return "Sortedlist";
    case OpSpecialClassEnum::Synth:
        return "Synth";
    case OpSpecialClassEnum::Ref:
        return "Ref";
    case OpSpecialClassEnum::Environment:
        return "Environment";
    case OpSpecialClassEnum::Event:
        return "Event";
    case OpSpecialClassEnum::Wavetable:
        return "Wavetable";
    case OpSpecialClassEnum::Env:
        return "Env";
    case OpSpecialClassEnum::Routine:
        return "Routine";
    case OpSpecialClassEnum::Color:
        return "Color";
    case OpSpecialClassEnum::Rect:
        return "Rect";
    default:
        assert(false);
        return "unexpected value";
    }
}

enum struct OpPseudoVarEnum : Byte {
    Process,
    Method,
    FunctionDef,
    Function,
    Thread,
    NumPseudoVars,
    COUNT,
};

[[nodiscard]] inline constexpr const char* asName(OpPseudoVarEnum n) noexcept {
    switch (n) {
    case OpPseudoVarEnum::Process:
        return "Process";
    case OpPseudoVarEnum::Method:
        return "Method";
    case OpPseudoVarEnum::FunctionDef:
        return "FunctionDef";
    case OpPseudoVarEnum::Function:
        return "Function";
    case OpPseudoVarEnum::Thread:
        return "Thread";
    case OpPseudoVarEnum::NumPseudoVars:
        return "NumPseudoVars";
    default:
        assert(false);
        return "unexpected value";
    }
}

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

[[nodiscard]] inline constexpr const char* asName(OpSpecialNumbers n) noexcept {
    switch (n) {
    case OpSpecialNumbers::MinusOne:
        return "MinusOne";
    case OpSpecialNumbers::Zero:
        return "Zero";
    case OpSpecialNumbers::One:
        return "One";
    case OpSpecialNumbers::Two:
        return "Two";
    case OpSpecialNumbers::Half:
        return "Half";
    case OpSpecialNumbers::MinusOneFloat:
        return "MinusOneFloat";
    case OpSpecialNumbers::ZeroFloat:
        return "ZeroFloat";
    case OpSpecialNumbers::OneFloat:
        return "OneFloat";
    case OpSpecialNumbers::TwoFloat:
        return "TwoFloat";
    default:
        assert(false);
        return "unexpected value";
    }
}

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

[[nodiscard]] inline constexpr const char* asName(OpSpecialSelectors n) noexcept {
    switch (n) {
    case OpSpecialSelectors::New:
        return "new";
    case OpSpecialSelectors::Init:
        return "init";
    case OpSpecialSelectors::At:
        return "at";
    case OpSpecialSelectors::Put:
        return "put";
    case OpSpecialSelectors::Next:
        return "next";
    case OpSpecialSelectors::Reset:
        return "reset";
    case OpSpecialSelectors::Value:
        return "value";
    case OpSpecialSelectors::CopyToEnd:
        return "copyToEnd";
    case OpSpecialSelectors::Add:
        return "add";
    case OpSpecialSelectors::Size:
        return "size";
    case OpSpecialSelectors::Class:
        return "class";
    case OpSpecialSelectors::If:
        return "if";
    case OpSpecialSelectors::While:
        return "while";
    case OpSpecialSelectors::For:
        return "for";
    case OpSpecialSelectors::And:
        return "and";
    case OpSpecialSelectors::Or:
        return "or";
    case OpSpecialSelectors::Case:
        return "case";
    case OpSpecialSelectors::Switch:
        return "switch";
    case OpSpecialSelectors::Identical:
        return "===";
    case OpSpecialSelectors::NotIdentical:
        return "!==";
    case OpSpecialSelectors::Print:
        return "print";
    case OpSpecialSelectors::Remove:
        return "remove";
    case OpSpecialSelectors::IndexOf:
        return "indexOf";
    case OpSpecialSelectors::WrapAt:
        return "wrapAt";
    case OpSpecialSelectors::ClipAt:
        return "clipAt";
    case OpSpecialSelectors::FoldAt:
        return "foldAt";
    case OpSpecialSelectors::WrapPut:
        return "wrapPut";
    case OpSpecialSelectors::ClipPut:
        return "clipPut";
    case OpSpecialSelectors::FoldPut:
        return "foldPut";
    case OpSpecialSelectors::Do:
        return "do";
    case OpSpecialSelectors::Collect:
        return "collect";
    case OpSpecialSelectors::Select:
        return "select";
    case OpSpecialSelectors::Reject:
        return "reject";
    case OpSpecialSelectors::Any:
        return "any";
    case OpSpecialSelectors::Every:
        return "every";
    case OpSpecialSelectors::Find:
        return "find";
    case OpSpecialSelectors::Choose:
        return "choose";
    case OpSpecialSelectors::ValueList:
        return "valueList";
    case OpSpecialSelectors::AddFirst:
        return "addFirst";
    case OpSpecialSelectors::PrimitiveFailed:
        return "primitiveFailed";
    case OpSpecialSelectors::SubclassResponsibility:
        return "subclassResponsibility";
    case OpSpecialSelectors::ShouldNotImplement:
        return "shouldNotImplement";
    case OpSpecialSelectors::NotYetImplemented:
        return "notYetImplemented";
    case OpSpecialSelectors::DoesNotUnderstand:
        return "doesNotUnderstand";
    case OpSpecialSelectors::AtSign:
        return "atSign";
    case OpSpecialSelectors::WrapAtSign:
        return "wrapAtSign";
    case OpSpecialSelectors::ClipAtSign:
        return "clipAtSign";
    case OpSpecialSelectors::FoldAtSign:
        return "foldAtSign";
    case OpSpecialSelectors::NewClear:
        return "newClear";
    case OpSpecialSelectors::NewCopyArgs:
        return "newCopyArgs";
    case OpSpecialSelectors::MultiNew:
        return "multiNew";
    case OpSpecialSelectors::MultiNewList:
        return "multiNewList";
    case OpSpecialSelectors::AR:
        return "aa";
    case OpSpecialSelectors::KR:
        return "kr";
    case OpSpecialSelectors::IR:
        return "ir";
    case OpSpecialSelectors::Copy:
        return "copy";
    case OpSpecialSelectors::PerformList:
        return "performList";
    case OpSpecialSelectors::IsKindOf:
        return "isKindOf";
    case OpSpecialSelectors::Postln:
        return "postln";
    case OpSpecialSelectors::AsString:
        return "asString";
    case OpSpecialSelectors::EnvirGet:
        return "envirGet";
    case OpSpecialSelectors::EnvirPut:
        return "envirPut";
    case OpSpecialSelectors::Halt:
        return "halt";
    case OpSpecialSelectors::ForBy:
        return "forBy";
    case OpSpecialSelectors::ForSeries:
        return "forSeries";
    case OpSpecialSelectors::ReverseDo:
        return "reverseDo";
    case OpSpecialSelectors::Loop:
        return "loop";
    case OpSpecialSelectors::NonBooleanError:
        return "nonBooleanError";
    case OpSpecialSelectors::PlusPlus:
        return "plusPlus";
    case OpSpecialSelectors::LTLT:
        return "LTLT";
    case OpSpecialSelectors::QuestionMark:
        return "?";
    case OpSpecialSelectors::DoubleQuestionMark:
        return "??";
    case OpSpecialSelectors::ExclamationQuestionMark:
        return "!?";
    case OpSpecialSelectors::Yield:
        return "yield";
    case OpSpecialSelectors::Name:
        return "name";
    case OpSpecialSelectors::MulAdd:
        return "mulAdd";
    case OpSpecialSelectors::Series:
        return "series";
    default:
        assert(false);
        return "unexpected value";
    }
}

enum struct OpSpecialValue : Byte {
    True,
    False,
    Nil_,
    Inf,
    COUNT,
};

[[nodiscard]] inline constexpr const char* asName(OpSpecialValue n) noexcept {
    switch (n) {
    case OpSpecialValue::True:
        return "true";
    case OpSpecialValue::False:
        return "false";
    case OpSpecialValue::Nil_:
        return "nil";
    case OpSpecialValue::Inf:
        return "inf";
    default:
        assert(false);
        return "unexpected value";
    }
}

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

[[nodiscard]] inline constexpr const char* asName(OpUnaryMath n) noexcept {
    switch (n) {
    case OpUnaryMath::Neg:
        return "neg";
    case OpUnaryMath::Not:
        return "not";
    case OpUnaryMath::IsNil:
        return "isNil";
    case OpUnaryMath::NotNil:
        return "notNil";
    case OpUnaryMath::BitNot:
        return "bitNot";
    case OpUnaryMath::Abs:
        return "abs";
    case OpUnaryMath::AsFloat:
        return "asFloat";
    case OpUnaryMath::AsInteger:
        return "asInteger";
    case OpUnaryMath::Ceil:
        return "ceil";
    case OpUnaryMath::Floor:
        return "floor";
    case OpUnaryMath::Frac:
        return "frac";
    case OpUnaryMath::Sign:
        return "sign";
    case OpUnaryMath::Squared:
        return "squared";
    case OpUnaryMath::Cubed:
        return "cubed";
    case OpUnaryMath::Sqrt:
        return "sqrt";
    case OpUnaryMath::Exp:
        return "exp";
    case OpUnaryMath::Recip:
        return "recip";
    case OpUnaryMath::MIDICPS:
        return "midicps";
    case OpUnaryMath::CPSMIDI:
        return "cpsmidi";
    case OpUnaryMath::MIDIRatio:
        return "midiRatio";
    case OpUnaryMath::RatioMIDI:
        return "ratioMidi";
    case OpUnaryMath::DbAmp:
        return "sbAmp";
    case OpUnaryMath::AmpDb:
        return "ampDb";
    case OpUnaryMath::OctCPS:
        return "octcps";
    case OpUnaryMath::CPSOct:
        return "cpsOct";
    case OpUnaryMath::Log:
        return "log";
    case OpUnaryMath::Log2:
        return "log2";
    case OpUnaryMath::Log10:
        return "log10";
    case OpUnaryMath::Sin:
        return "sin";
    case OpUnaryMath::Cos:
        return "cos";
    case OpUnaryMath::Tan:
        return "tan";
    case OpUnaryMath::ArcSin:
        return "arcSin";
    case OpUnaryMath::ArcCos:
        return "arcCos";
    case OpUnaryMath::ArcTan:
        return "arcTan";
    case OpUnaryMath::SinH:
        return "sinH";
    case OpUnaryMath::CosH:
        return "cosH";
    case OpUnaryMath::TanH:
        return "tanH";
    case OpUnaryMath::Rand:
        return "rand";
    case OpUnaryMath::Rand2:
        return "rand2";
    case OpUnaryMath::LinRand:
        return "linRand";
    case OpUnaryMath::BiLinRand:
        return "biLinRand";
    case OpUnaryMath::Sum3Rand:
        return "sum3Rand";
    case OpUnaryMath::Distort:
        return "distort";
    case OpUnaryMath::SoftClip:
        return "softClip";
    case OpUnaryMath::Coin:
        return "coin";
    case OpUnaryMath::DigitValue:
        return "digitValue";
    case OpUnaryMath::Silence:
        return "silence";
    case OpUnaryMath::Thru:
        return "thru";
    case OpUnaryMath::RectWindow:
        return "rectWindow";
    case OpUnaryMath::HanWindow:
        return "hanWindow";
    case OpUnaryMath::WelchWindow:
        return "welchWindow";
    case OpUnaryMath::TriWindow:
        return "triWindow";
    case OpUnaryMath::Ramp:
        return "ramp";
    case OpUnaryMath::SCurve:
        return "scurve";
    default:
        assert(false);
        return "unexpected value";
    }
}

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

[[nodiscard]] inline constexpr const char* asName(OpBinaryMath n) noexcept {
    switch (n) {
    case OpBinaryMath::Add:
        return "+";
    case OpBinaryMath::Sub:
        return "-";
    case OpBinaryMath::Mul:
        return "*";
    case OpBinaryMath::IDiv:
        return "iDiv";
    case OpBinaryMath::FDiv:
        return "fDiv";
    case OpBinaryMath::Mod:
        return "mod";
    case OpBinaryMath::EQ:
        return "==";
    case OpBinaryMath::NE:
        return "!=";
    case OpBinaryMath::LT:
        return "<";
    case OpBinaryMath::GT:
        return ">";
    case OpBinaryMath::LE:
        return "<=";
    case OpBinaryMath::GE:
        return ">=";
    case OpBinaryMath::Min:
        return "min";
    case OpBinaryMath::Max:
        return "max";
    case OpBinaryMath::BitAnd:
        return "bitAnd";
    case OpBinaryMath::BitOr:
        return "bitOr";
    case OpBinaryMath::BitXor:
        return "bitXor";
    case OpBinaryMath::LCM:
        return "lcm";
    case OpBinaryMath::GCD:
        return "gcm";
    case OpBinaryMath::Round:
        return "round";
    case OpBinaryMath::RoundUp:
        return "roundUp";
    case OpBinaryMath::Trunc:
        return "trunc";
    case OpBinaryMath::Atan2:
        return "atan2";
    case OpBinaryMath::Hypot:
        return "hypot";
    case OpBinaryMath::Hypotx:
        return "hypotx";
    case OpBinaryMath::Pow:
        return "pow";
    case OpBinaryMath::ShiftLeft:
        return "shiftLeft";
    case OpBinaryMath::ShiftRight:
        return "shiftRight";
    case OpBinaryMath::UnsignedShift:
        return "unsignedShift";
    case OpBinaryMath::Fill:
        return "fill";
    case OpBinaryMath::Ring1:
        return "ring1"; // a * (b + 1) == a * b + a
    case OpBinaryMath::Ring2:
        return "ring2"; // a * b + a + b
    case OpBinaryMath::Ring3:
        return "ring3"; // a*a*b
    case OpBinaryMath::Ring4:
        return "ring4"; // a*a*b - a*b*b
    case OpBinaryMath::DifSqr:
        return "sifSpqr"; // a*a - b*b
    case OpBinaryMath::SumSqr:
        return "sumSqr"; // a*a + b*b
    case OpBinaryMath::SqrSum:
        return "sqrSum"; // (a + b)^2
    case OpBinaryMath::SqrDif:
        return "sqrDif"; // (a - b)^2
    case OpBinaryMath::AbsDif:
        return "absDif"; // |a - b|
    case OpBinaryMath::Thresh:
        return "thresh";
    case OpBinaryMath::AMClip:
        return "amClip";
    case OpBinaryMath::ScaleNeg:
        return "scaleNeg";
    case OpBinaryMath::Clip2:
        return "clip2";
    case OpBinaryMath::Excess:
        return "excess";
    case OpBinaryMath::Fold2:
        return "fold2";
    case OpBinaryMath::Wrap2:
        return "wrap2";
    case OpBinaryMath::FirstArg:
        return "firstArg";
    case OpBinaryMath::RandRange:
        return "randRange";
    case OpBinaryMath::ExpRandRange:
        return "expRandRange";
    default:
        assert(false);
        return "unexpected value";
    }
}

enum struct OpTrinaryMath : Byte {
    Divz,
    Clip,
    Wrap,
    Fold,
    RampMult,
    Mix,
    COUNT,
};

[[nodiscard]] inline constexpr const char* asName(OpTrinaryMath n) noexcept {
    switch (n) {
    case OpTrinaryMath::Divz:
        return "divz";
    case OpTrinaryMath::Clip:
        return "clip";
    case OpTrinaryMath::Wrap:
        return "wrap";
    case OpTrinaryMath::Fold:
        return "fold";
    case OpTrinaryMath::RampMult:
        return "rampMult";
    case OpTrinaryMath::Mix:
        return "mix";
    default:
        assert(false);
        return "unexpected value";
    }
}

namespace Operands {
struct Index : Operands::details::NamedByte<Index> {
    static constexpr const char* name = "Index";
};
struct SelectorIndex : Operands::details::NamedByte<SelectorIndex> {
    static constexpr const char* name = "SelectorIndex";
};
struct FrameOffset : Operands::details::NamedByte<FrameOffset> {
    static constexpr const char* name = "FrameOffset";
};
struct Class : Operands::details::NamedByte<Class> {
    static constexpr const char* name = "Class";
};
struct ArgumentCount : Operands::details::NamedByte<ArgumentCount> {
    static constexpr const char* name = "ArgumentCount";
};
struct KwArgumentCount : Operands::details::NamedByte<KwArgumentCount> {
    static constexpr const char* name = "KwArgumentCount";
};

template <unsigned int TOTAL, unsigned int PART> struct UnsignedInt {
    static_assert(8 * PART < TOTAL);
    static constexpr const char* name = "UIntPart";
    constexpr static UnsignedInt<TOTAL, PART> fromRaw(Byte b) { return { b }; }

    constexpr static UnsignedInt<TOTAL, PART> fromFull(std::uint64_t i) {
        assert(i < (1ULL << TOTAL));
        return { Opcode::details::to_byte(i >> (8U * PART)) };
    }

    constexpr static UnsignedInt<TOTAL, PART> fromFull(std::uint32_t i) {
        return UnsignedInt::fromFull(static_cast<std::uint64_t>(i));
    }

    template <typename T> constexpr static UnsignedInt<TOTAL, PART> fromFull(T) = delete;

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
    static constexpr const char* name = "IntPart";
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


struct SpecialClass : Operands::details::OperandEnumWrapper<OpSpecialClassEnum, SpecialClass> {
    static constexpr const char* name = "SpecialClass";
};
struct PseudoVar : Operands::details::OperandEnumWrapper<OpPseudoVarEnum, PseudoVar> {
    static constexpr const char* name = "PseudoVar";
};

struct SpecialSelectors : Operands::details::OperandEnumWrapper<OpSpecialSelectors, SpecialSelectors> {
    static constexpr const char* name = "SpecialSelectors";
};

struct UnaryMath : Operands::details::OperandEnumWrapper<OpUnaryMath, UnaryMath> {
    static constexpr const char* name = "UnaryMath";
};

struct UnaryMathNibble : Operands::details::OperandEnumWrapper<OpUnaryMathNibble, UnaryMathNibble> {
    static constexpr const char* name = "UnaryMathNibble";
};
struct BinaryMath : Operands::details::OperandEnumWrapper<OpBinaryMath, BinaryMath> {
    static constexpr const char* name = "BinaryMath";
};
struct TrinaryMath : Operands::details::OperandEnumWrapper<OpTrinaryMath, TrinaryMath> {
    static constexpr const char* name = "TrinaryMath";
};
struct BinaryMathNibble : Operands::details::OperandEnumWrapper<OpBinaryMathNibble, BinaryMathNibble> {
    static constexpr const char* name = "BinaryMathNibble";
};

struct SpecialValue : Operands::details::OperandEnumWrapper<OpSpecialValue, SpecialValue> {
    static constexpr const char* name = "SpecialValue";
};
struct SpecialNumber : Operands::details::OperandEnumWrapper<OpSpecialNumbers, SpecialNumber> {
    static constexpr const char* name = "SpecialNumber";
};
};
