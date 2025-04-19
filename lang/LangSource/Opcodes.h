/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once
#include "ByteCodeArray.h"
#include <tuple>


/* (Jordan Henderson)
 --- Credit goes to Moss Heim (2018-06-19) for their work deciphering and documenting what all the bytecodes do.
 --- This struct just turns the documentation into code and copies the descriptions given into comments.


This file turns the opcodes into types; meaning there no longer needs to be magic numbers when compiling or otherwise
using bytecodes and their operands.

However, this does require some more advanced C++ ideas.

Opcodes come in three types:
* Simple
   - These are bytecodes (Byte) followed by zero or more operands (Byte)
   - The number of operands is always known.
   - They bytecode takes up exactly one byte, as do the operands.
   - e.g., SendMsgX, which has the argument count, keyword argument count, and special index as operands.
* Loop
   - These start with the operand '8F' and are used by the compiler to implement loop optimisations
* SecondNibble
   - These use the second nibble of the bytecode as a number.

Most opcodes are simple.

*/

namespace details {

enum struct OpCodeType { Simple, SecondNibble, SecondNibbleViaEnum, Loop };

template <Byte CODE, typename... OPERANDS> struct SimpleOpSpec {
    static constexpr OpCodeType opCodeType { OpCodeType::Simple };
    // static constexpr Byte code { CODE };
    static constexpr auto operandCount { sizeof...(OPERANDS) };
    using Tuple = std::tuple<OPERANDS...>;

    const char* name;
    Byte code { CODE };

    Tuple pullOperandsFromInstructions(unsigned char*& ip) const {
        // increment instruction pointer and get the values for each operand.
        return { OPERANDS::fromByte(*(++ip))... };
    }

    void compile(OPERANDS... operands) const {
        compileByte(code);
        (compileByte(operands), ...);
    }
};

template <Byte STARTCODE, Byte ENDCODE, typename... OPERANDS> struct SecondNibbleOpSpec {
    static constexpr OpCodeType opCodeType { OpCodeType::SecondNibble };
    static constexpr auto startCode { STARTCODE };
    static constexpr auto endCode { ENDCODE };
    static constexpr auto operandCount { sizeof...(OPERANDS) };
    using Tuple = std::tuple<OPERANDS...>;

    const char* name;
};

template <Byte STARTCODE, Byte ENDCODE, typename ENUM_T, typename... OPERANDS> struct SecondNibbleViaEnumOpSpec {
    static constexpr OpCodeType opCodeType { OpCodeType::SecondNibbleViaEnum };
    static constexpr auto startCode { STARTCODE };
    static constexpr auto endCode { ENDCODE };
    static constexpr auto operandCount { sizeof...(OPERANDS) };
    using Tuple = std::tuple<OPERANDS...>;
    using Enum = ENUM_T;

    const char* name;
};

struct NamedByte {
    Byte value;
    constexpr operator Byte() const { return value; }
};

template <typename Enum_T> struct OperandEnumWrapper {
    static_assert(std::is_convertible_v<std::underlying_type_t<Enum_T>, Byte>);
    Enum_T value;
    constexpr operator Byte() const { return static_cast<Byte>(value); }
};

} // namespace details

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
};

enum struct OpSpecialClassEnum : Byte {
    Object,
    Symbol,
    Nil,
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
    NumSpecialClasses
};

enum struct OpPseudoVarEnum : Byte {
    Process,
    Method,
    FunctionDef,
    Function,
    Thread,
    NumPseudoVars,
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
};

enum struct OpSpecialSelectors : Byte {
    New,
    Init,
    At,
    Put,
    Next,
    Reset,
    Value,
    CopyToEnd, // used by multi assign
    Add, // used by dynamic list
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
    NumSpecialSelectors
};

enum struct OpSpecialValue : Byte { True, False, Nil, Inf };

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
    NumUnarySelectors
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
    NumBinarySelectors
};

enum struct OpTrinaryMath : Byte {
    Divz,
    Clip,
    Wrap,
    Fold,
    RampMult,
    Mix,
};
struct Operands {
    struct Unknown : details::NamedByte {
        static constexpr const char* name = "Unknown";
        constexpr static Unknown fromByte(Byte b) { return { b }; }
    };
    struct Index : details::NamedByte {
        static constexpr const char* name = "Index";
        constexpr static Index fromByte(Byte b) { return { b }; }
    };
    struct FrameOffset : details::NamedByte {
        static constexpr const char* name = "FrameOffset";
        constexpr static FrameOffset fromByte(Byte b) { return { b }; }
    };
    struct Class : details::NamedByte {
        static constexpr const char* name = "Class";
        constexpr static Class fromByte(Byte b) { return { b }; }
    };
    struct ArgumentCount : details::NamedByte {
        static constexpr const char* name = "ArgumentCount";
        constexpr static ArgumentCount fromByte(Byte b) { return { b }; }
    };
    struct KwArgumentCount : details::NamedByte {
        static constexpr const char* name = "KwArgumentCount";
        constexpr static KwArgumentCount fromByte(Byte b) { return { b }; }
    };
    struct LowBitsOf12BitNumber : details::NamedByte {
        static constexpr const char* name = "LowBitsOf12BitNumber";
        constexpr static LowBitsOf12BitNumber fromByte(Byte b) { return { b }; }
    };
    template <size_t TOTAL, size_t PART> struct NumericByte : details::NamedByte {
        static constexpr const char* name = "NumericByte";
        constexpr static NumericByte<TOTAL, PART> fromByte(Byte b) { return { b }; }
    };
    struct SpecialClass : details::OperandEnumWrapper<OpSpecialClassEnum> {
        static constexpr const char* name = "SpecialClass";
        constexpr static SpecialClass fromByte(Byte b) { return { static_cast<OpSpecialClassEnum>(b) }; }
    };
    struct PseudoVar : details::OperandEnumWrapper<OpPseudoVarEnum> {
        static constexpr const char* name = "PseudoVar";
        constexpr static PseudoVar fromByte(Byte b) { return { static_cast<OpPseudoVarEnum>(b) }; }
    };

    struct SpecialSelectors : details::OperandEnumWrapper<OpSpecialSelectors> {
        static constexpr const char* name = "SpecialSelectors";
        constexpr static SpecialSelectors fromByte(Byte b) { return { static_cast<OpSpecialSelectors>(b) }; }
    };

    struct UnaryMath : details::OperandEnumWrapper<OpUnaryMath> {
        static constexpr const char* name = "UnaryMath";
        constexpr static UnaryMath fromByte(Byte b) { return { static_cast<OpUnaryMath>(b) }; }
    };
    struct BinaryMath : details::OperandEnumWrapper<OpBinaryMath> {
        static constexpr const char* name = "BinaryMath";
        constexpr static BinaryMath fromByte(Byte b) { return { static_cast<OpBinaryMath>(b) }; }
    };
    struct TrinaryMath : details::OperandEnumWrapper<OpTrinaryMath> {
        static constexpr const char* name = "TrinaryMath";
        constexpr static TrinaryMath fromByte(Byte b) { return { static_cast<OpTrinaryMath>(b) }; }
    };

    //
    static_assert(Byte { 1 } == Index { 1 });
    static_assert(Byte { 0 } != Class { 1 });
};


/*
They way to interpret the SimpleOpSpec is as follows...

```c++
SimpleOpSpec<**BYTE CODE**, **OPERANDS**...> **NAME OF BYTE CODE** { "**NAME**" };
```

Where:
   **BYTE CODE** is the numerical value of the code, this is written in hex.
   **OPERANDS** are zero of more types that describe what the operands of the byte code are.


By doing things this way, the way to compiling bytecodes can be typed checked and have descriptive names. It could look
something like...

```c++
struct OpCode {
    SimpleOpSpec<0x00, Operands::ArgumentCount, Operands::KwArgumentCount> MySendMsg { "MySendMsg" };
};

byteCodeArray.compileByteCode<OpCode::MySendMsg>(
    Operands::ArgumentCount{2},
    Operands::KwArgumentCount{4}
);
```
*/


struct OpCode {
    /// Push a class to the top of the stack.
    /// 'Special' classes are handled by a different op code (PushSpecialClass).
    static constexpr details::SimpleOpSpec<0x00, Operands::Class> PushClassX { "PushClassX" };

    /// Push an instance variable of the parent class to the top of the stack.
    /// The second byte determines the index of the variable to push.
    /// This operation is the counterpart to 07 StoreInstVarX.
    /// Only used in class method code.
    static constexpr details::SimpleOpSpec<0x01, Operands::Index> PushInstVarX { "PushInstVarX" };

    /// Push a temporary variable to the top of the stack.
    /// The second instruction byte indicates how many frames up the stack the variable lives.
    /// For example, a value of 1 here means the variable belongs to the function block just outside the current one.
    /// The third instruction byte indicates the index of the variable within that frame.
    /// This instruction is used when 3x PushTempVar cannot be used; because the variable belongs to an enclosing
    /// frame.
    static constexpr details::SimpleOpSpec<0x02, Operands::FrameOffset, Operands::Index> PushTempVarX {
        "PushTempVarX"
    };

    /// Push a temporary variable from the current frame to the top of the stack.
    /// The second instruction byte indicates the index of the variable within that frame.
    /// This instruction is used when 3x PushTempVar cannot be used because the variable index is above 16.
    static constexpr details::SimpleOpSpec<0x03, Operands::Index> PushTempZeroVarX { "PushTempZeroVarX" };

    /// Push a stored literal from the current frame to the top of the stack.
    /// The second instruction byte indicates the index of the literal within that frame.
    /// This instruction is used when the literal is of type FunctionDef.
    static constexpr details::SimpleOpSpec<0x04, Operands::Index> PushLiteralX { "PushLiteralX" };

    /// Push a class variable of a class to the top of the stack.
    /// The second byte determines the class to use and the third byte determines the index of the variable to push.
    /// Only used in class method code.
    static constexpr details::SimpleOpSpec<0x05, Operands::Class, Operands::Index> PushClassVarX { "PushClassVarX" };

    /// Push a 'special' (intrinsic) class onto the stack.
    /// The second instruction byte indicates the index of the intrinsic class within the internal array
    /// `gSpecialClasses`.
    static constexpr details::SimpleOpSpec<0x06, Operands::SpecialClass> PushSpecialClass { "PushSpecialClass" };


    /// Store the top of the stack to an instance variable of the parent class.
    /// The second byte determines the index of the variable.
    /// The stack is not altered. This operation is the counterpart to 01 PushInstVarX.
    /// Only used in class method code.
    static constexpr details::SimpleOpSpec<0x07, Operands::Index> StoreInstVarX { "StoreInstVarX" };


    /// Store the top of the stack in a variable, without popping the stack. "
    /// The second instruction byte indicates the distance between the current frame and the target variable's frame. "
    /// The third instruction byte indicates the index of the variable within that frame. "
    static constexpr details::SimpleOpSpec<0x08, Operands::FrameOffset, Operands::Index> StoreTempVarX {
        "StoreTempVarX"
    };

    /// Store the top of the stack in a class variable, without popping the stack.
    /// The second and third instruction bytes indicate the index of the variable within the VM's classvars field.
    /// Only used in class method code.
    static constexpr details::SimpleOpSpec<0x09, Operands::NumericByte<2, 1>, Operands::NumericByte<2, 0>>
        StoreClassVarX {
            "StoreClassVarX",
        };


    /// Send a message.
    /// The second instruction byte indicates the number of normal arguments (TODO: document normal vs key args).
    /// The third instruction byte indicates the number of keyword arguments.
    /// The fourth instruction byte indicates ??? TODO (I think it's the selector index when the selector index
    /// is known at compile-time).
    /// The stack will be modified so that all the keyword and normal arguments are consumed; the top of the
    /// stack
    /// will then be the result of the call. \n
    /// { meow(x, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14) }.def.dumpByteCodes \n
    /// { meow(x, a: 5) }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x0A, Operands::ArgumentCount, Operands::KwArgumentCount, Operands::Index>
        SendMsgX { "SendMsgX" };

    /// "Same as SendMsgX, but only when called on `super`
    /// `super.meow(...)`.;
    static constexpr details::SimpleOpSpec<0x0B, Operands::ArgumentCount, Operands::KwArgumentCount, Operands::Index>
        SendSuperMsgX { "SendSuperMsgX" };


    /// Send a 'special message' to an object.
    /// The second and third instruction bytes respectively indicate the numbers of normal and keyword arguments.
    /// The fourth instruction byte indicates the index of the selector name within the internal array of special
    /// messages.
    /// TODO Used when C1 cannot be used, for example because keyword arguments are used in the call. \n
    /// { 5.postln(x: a) }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x0C, Operands::ArgumentCount, Operands::KwArgumentCount, Operands::Index>
        SendSpecialMsgX { "SendSpecialMsgX" };

    /// Send a special unary arithmetic message to the object on the top of the stack.
    /// If the object is a primitive type, the message is executed in an optimized manner.
    /// Otherwise, it becomes a normal message send.
    /// The second instruction byte indicates the value of the unary arithmetic message opcode, as defined in Opcodes.h.
    /// \n { 5.0.reciprocal }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x0D, Operands::UnaryMath> SendSpecialUnaryArithMsgX {
        "SendSpecialUnaryArithMsgX"
    };

    /// Send a special binary arithmetic message to the two objects on the top of the stack.
    /// If the receiver is a primitive type, the message is executed in an optimized manner.
    /// Otherwise, it becomes a normal message send.
    /// The second instruction byte indicates the value of the binary arithmetic message opcode, as defined in
    /// Opcodes.h. \n { 5.0.pow(5) }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x0E, Operands::BinaryMath> SendSpecialBinaryArithMsgX {
        "SendSpecialBinaryArithMsgX"
    };

    /// Pushes a special this–type object to the top of the stack.
    /// The second instruction byte indicates the object to push.
    /// One of thisProcess, thisThread, thisMethod, thisFunctionDef, thisFunction.
    /// In the case of thisFunction, a new (anonymous) closure is allocated. [TODO - how does this work?]
    static constexpr details::SimpleOpSpec<0x0F, Operands::PseudoVar> SpecialOpcode { "SpecialOpcode" };


    /// Push an instance variable from the current class to the top of the stack.
    /// The index of the variable is given by the second nibble of the instruction.
    /// For example, 10 pushes the first-declared variable.
    /// If the index is greater than 15, 01 PushInstVarX is used.
    /// Server.findMethod('maxNumClients').dumpByteCodes
    static constexpr details::SecondNibbleOpSpec<0x10, 0x20> PushInstVar { "PushInstVar" };

    static constexpr details::SimpleOpSpec<0x20, Operands::Unknown, Operands::Unknown> JumpIfTrue { "JumpIfTrue" };

    /// Push a variable from an enclosing frame onto the stack.
    /// The second nibble of the first instruction byte gives the distance in frames between the current frame and the
    /// desired target. The second instruction byte determines the index of the variable within that frame. For
    /// instance, 25 02 indicates the third variable of the fifth frame outside this frame.
    /// { var a, b, c; { { { { { c.floop }.def.dumpByteCodes }.value }.value }.value }.value }.value
    static constexpr details::SecondNibbleOpSpec<0x21, 0x28, Operands::Index> PushTempVar { "PushTempVar" };

    /// Push a constant from the current frame onto the stack. The second instruction byte indicates the index of the
    /// constant.
    /// This message is used instead of 4X PushLiteral when the index is >= 16.
    /// { a = [\a, \b, \c, \d, \e, \f, \g, \h, \i, \j, \k, \l, \m, \n, \o, \p, \q] }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x28, Operands::Index> PushConstant8 { "PushConstant8" };

    /// Push a constant from the current frame onto the stack.
    /// The second and third instruction bytes interpreted as a 16-bit integer indicate the index of the constant.
    /// This message is used instead of 28 PushConstant when the index is >= 256.
    static constexpr details::SimpleOpSpec<0x29, Operands::NumericByte<16, 1>, Operands::NumericByte<16, 0>>
        PushConstant16 { "PushConstant16" };


    /// Push a constant from the current frame onto the stack.
    /// The second, third and fourth instruction bytes interpreted as a 24-bit integer indicate the index of the
    /// constant. This message is used instead of 29 PushConstant when the index is >= 2^16.
    static constexpr details::SimpleOpSpec<0x2A, Operands::NumericByte<24, 2>, Operands::NumericByte<24, 1>,
                                           Operands::NumericByte<24, 0>>
        PushConstant24 { "PushConstant24" };

    /// Push a constant from the current frame onto the stack.
    /// The second to fifth instruction bytes interpreted as a 32-bit integer indicate the index of the constant.
    /// This message is used instead of 2A PushConstant when the index is >= 2^24.
    static constexpr details::SimpleOpSpec<0x2B, Operands::NumericByte<32, 3>, Operands::NumericByte<32, 2>,
                                           Operands::NumericByte<32, 1>, Operands::NumericByte<32, 0>>
        PushConstant32 { "PushConstant32" };

    /// Push an 8 bit integer (signed) onto the stack from the operand.
    static constexpr details::SimpleOpSpec<0x2C, Operands::Index> PushInteger8 { "PushInteger8" };

    /// Push an 16 bit integer (signed) onto the stack from the operand.
    static constexpr details::SimpleOpSpec<0x2D, Operands::NumericByte<16, 1>, Operands::NumericByte<16, 0>>
        PushInteger16 { "PushInteger16" };


    /// Push an 24 bit integer (signed) onto the stack from the operand.
    static constexpr details::SimpleOpSpec<0x2E, Operands::NumericByte<24, 2>, Operands::NumericByte<24, 1>,
                                           Operands::NumericByte<24, 0>>
        PushInteger24 { "PushInteger24" };

    /// Push an 32 bit integer (signed) onto the stack from the operand.
    static constexpr details::SimpleOpSpec<0x2F, Operands::NumericByte<32, 3>, Operands::NumericByte<32, 2>,
                                           Operands::NumericByte<32, 1>, Operands::NumericByte<32, 0>>
        PushInteger32 { "PushInteger32" };

    /// Push a variable from the current frame to the top of the stack.
    /// The index of the variable is given by the second nibble of the instruction.
    // For example, 0x30 pushes the first-declared variable, 0x31 the second.
    // The "Zero" in the name indicates that this variable lives in the top (zeroeth) frame.
    static constexpr details::SecondNibbleOpSpec<0x30, 0x40> PushTempZeroVar { "PushTempZeroVar" };

    /// Push a constant from the current frame to the top of the stack.
    /// The index of the constant is given by the second nibble of the instruction.
    // For example, 0x40 pushes the first-declared constant, 0x41 the second.
    /// { 5.5 + 8.5 }.def.dumpByteCodes
    static constexpr details::SecondNibbleOpSpec<0x40, 0x50> PushLiteral { "PushLiteral" };

    /// Push a class variable from the current frame to the top of the stack.
    /// The index of the variable is given by the second nibble of the first instruction byte and the second instruction
    /// byte interpreted as a 12-bit integer. The indexed array is the global collection of class variables.
    /// Server.findMethod('calculateViewBounds').dumpByteCodes
    static constexpr details::SecondNibbleOpSpec<0x50, 0x60> PushClassVar { "PushClassVar" };

    /// Pushes 'this' to the top of the stack.
    static constexpr details::SimpleOpSpec<0x60> PushSpecialValueThis { "PushSpecialValueThis" };


    /// If the top of the stack is an integer, this instruction decrements it by one.
    /// Otherwise, the integer value one is pushed to the top of the stack and prSubNum is called.
    /// { |x| x - 1; }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x61> PushOneAndSubtract { "PushOneAndSubtract" };

    /// Pushes a commonly used numerical value to the top of the stack.
    static constexpr details::SecondNibbleViaEnumOpSpec<0x62, 0x6B, OpSpecialNumbers> PushSpecialNumber {
        "PushSpecialNumber "
    };

    /// If the top of the stack is an integer, this instruction increments it by one.
    /// Otherwise, the integer value one is pushed to the top of the stack and prAddNum is called.
    static constexpr details::SimpleOpSpec<0x6B> PushOneAndAddOne { "PushOneAndAddOne" };


    /// Pushes a commonly used value to the top of the stack.
    /// True, False, Nil, or Inf.
    static constexpr details::SecondNibbleViaEnumOpSpec<0x6c, 0x70, OpSpecialValue> PushSpecialValue {
        "PushSpecialValue"
    };

    /// Pops the top of the stack and stores it in an instance variable of the current receiver.
    /// The second nibble of the byte indicates the index of the variable.
    /// TODO: has different behavior depending on whether owner of instance variable is immutable (?)
    /// Only used within class code.
    /// Server.findMethod('addr_').dumpByteCodes
    static constexpr details::SecondNibbleOpSpec<0x70, 0x80> StoreInstVar { "StoreInstVar" };

    /// Pops the top of the stack and stores it in a local instance variable.
    /// The second nibble of the first byte indicates the distance in frames between the current frame and the resident
    /// frame of the store variable. The second instruction byte indicates the index of the variable within its frame.
    /// TODO discuss this vs StoreTempVarX
    /// { var a, b, c, d, e, f, g, h, i; d = 5; d }.def.dumpByteCodes
    /// { var a, b, c, d, e, f, g, h, i; { d = 5; d }.def.dumpByteCodes }.value
    static constexpr details::SecondNibbleOpSpec<0x80, 0x88, Operands::Index> StoreTempVar { "StoreTempVar" };


    /// Pushes an instance variable of the current receiver to the top of the stack and calls a special method on it.
    /// The second instruction byte indicates the index of the instance variable in the class.
    /// The third instruction byte indicates the index of the method selector.
    /// Only used within class code.
    /// RingBuffer.findMethod('add').dumpByteCodes
    static constexpr details::SimpleOpSpec<0x88, Operands::Index, Operands::Index> PushInstVarAndSpendSpecialMsg {
        "PushInstVarAndSpendSpecialMsg"
    };

    /// Push all arguments to the top of the stack in order given, and call a method.
    /// The second instruction byte indicates the index of the selector name in the current frame.
    /// { |a, b, c| a.meow(b, c) }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x89, Operands::Index> PushAllArgsAndSendMsg { "PushAllArgsAndSendMsg" };


    /// Push all arguments except the first to the top of the stack in order given, and call a method.
    /// The second instruction byte indicates the index of the selector name in the current frame.
    /// TODO when exactly used? Used in class code to avoid pushing this. (?) TODO double check usage - is this really
    /// an implicit param? Meta_File.findMethod('open').dumpByteCodes
    static constexpr details::SimpleOpSpec<0x8A, Operands::Index> PushAllButFirstArgAndSendMsg {
        "PushAllButFirstArgAndSendMsg"
    };

    /// Push all arguments to the top of the stack in order given, and call a special method.
    /// The second instruction byte indicates the index of the selector name in the list of special selectors
    /// (gSpecialSelectors). { |a, b, c| a.meow(b, c) }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x8B, Operands::Index> PushAllArgsAndSendSpecialMsg {
        "PushAllArgsAndSendSpecialMsg"
    };


    /// Push all arguments except the first to the top of the stack in order given, and call a special method.
    /// The second instruction byte indicates the index of the selector name in the list of special selectors
    /// (gSpecialSelectors).
    /// TODO when exactly used? Used in class code to avoid pushing this. (?) TODO double check usage - is this really
    /// an implicit param? Meta_File.findMethod('open').dumpByteCodes
    static constexpr details::SimpleOpSpec<0x8C, Operands::Index> PushAllButFirstArgAndSendSpecialMsg {
        "PushAllButFirstArgAndSendSpecialMsg"
    };

    /// Push all arguments except the first two (TODO - really?), and call a method.
    /// The second instruction byte indicates the index of the selector name in the current frame.
    /// { |a, b, c| this.join(a, b, c) }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x8D, Operands::Index> PushAllButFirstTwoArgsAndSendMsg {
        "PushAllButFirstTwoArgsAndSendMsg"
    };

    /// Push all arguments except the first two (TODO - really?), and call a special method.
    /// The second instruction byte indicates the index of the selector name in the list of special selectors
    /// (gSpecialSelectors).
    /// TODO when exactly used? Used in class code to avoid pushing this. (?)
    /// { |a, b, c| this.add(a, b, c) }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0x8E, Operands::Index> PushAllButFirstTwoArgsAndSendSpecialMsg {
        "PushAllButFirstTwoArgsAndSendSpecialMsg"
    };

    // 0x8F is the loop magic

    /// Pop and store the top of the stack in a class variable of the current class.
    /// The index of the class variable is found by interpreting the second nibble of the first byte and the entire
    /// second byte as a 12-bit integer. Only used in class code. Meta_String.findMethod('initClass').dumpByteCodes
    static constexpr details::SecondNibbleOpSpec<0x90, 0xA0, Operands::LowBitsOf12BitNumber> StoreClassVar {
        "StoreClassVar"
    };

    /// Push this to the top of the stack and call a method taking one argument.
    /// The second instruction byte determines the index of the selector within the block.
    /// { this.hark }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0xA0, Operands::Index> SendMsgThisOpt { "SendMsgThisOpt" };

    /// Call a method taking a number of arguments.
    /// The second nibble of the first instruction byte determines the number of arguments passed to the method.
    /// The second instruction byte determines the index of the selector within the block.
    /// { a.hark(b); c.herp(d, e) }.def.dumpByteCodes
    static constexpr details::SecondNibbleOpSpec<0xA1, 0xB0, Operands::Index> SendMsg { "SendMsg" };

    /// Sets the global tail call state to 2. TODO what does this mean?
    static constexpr details::SimpleOpSpec<0xB0> TailCallReturnFromFunction { "TailCallReturnFromFunction" };

    /// Call a method on super taking a number of arguments.
    /// The second nibble of the first instruction bytes determines the number of arguments passed to the method.
    /// The second instruction byte determines the index of the selector within the block.
    /// Mostly used in class code, as in an interpreted context it refers to Interpreter.
    static constexpr details::SecondNibbleOpSpec<0xB1, 0xC0, Operands::Index> SendSuperMsg { "SendSuperMsg" };

    /// Push this to the top of the stack and call a special method taking one argument.
    /// The second instruction byte determines the index of the special method within gSpecialSelectors.
    /// { this.init }.def.dumpByteCodes
    static constexpr details::SimpleOpSpec<0xC0, Operands::Index> SendSuperMsgThisOpt { "SendSuperMsgThisOpt" };


    /// Call a special method taking a number of arguments.
    /// The second nibble of the first instruction byte determines the number of arguments passed to the method.
    /// The second instruction byte determines the index of the special method within gSpecialSelectors.
    /// { 5.init(8, 9, 10) }.def.dumpByteCodes
    static constexpr details::SecondNibbleOpSpec<0xC1, 0xD0, Operands::SpecialSelectors> SendSpecialMsg {
        "SendSpecialMsg"
    };

    /// Perform a unary operation in-place on the top of the stack.
    static constexpr details::SecondNibbleViaEnumOpSpec<0xD0, 0xE0, OpUnaryMathNibble> SpendSpecialUnaryArithMsg {
        "SpendSpecialUnaryArithMsg"
    };

    /// Perform a binary operation in-place on the top of the stack.
    static constexpr details::SecondNibbleViaEnumOpSpec<0xE0, 0xF0, OpBinaryMathNibble> SpendSpecialBinaryArithMsg {
        "SpendSpecialBinaryArithMsg"
    };

    /// Pop the top of the stack and discard it.
    static constexpr details::SimpleOpSpec<0xF0> Drop { "Drop" };

    /// Duplicate the top of the stack.
    static constexpr details::SimpleOpSpec<0xF1> Dup { "Dup" };

    /// Function Return
    /// Used when returning from a closure. Return from the current block.
    /// Sets the current VM frame to the calling frame and updates the VM's IP, current block, and method accordingly.
    static constexpr details::SimpleOpSpec<0xF2> BlockReturn { "BlockReturn" };

    /// Used when returning from a class method. Return from the current method.
    /// Sets the current
    /// TODO document
    static constexpr details::SimpleOpSpec<0xF3> Return { "Return" };

    /// Push this to the top of the stack and return from the current method, as in 0xF3 Return.
    static constexpr details::SimpleOpSpec<0xF4> ReturnSelf { "ReturnSelf" };
    static constexpr details::SimpleOpSpec<0xF5> ReturnTrue { "ReturnTrue" };
    static constexpr details::SimpleOpSpec<0xF6> ReturnFalse { "ReturnFalse" };
    static constexpr details::SimpleOpSpec<0xF7> ReturnNil { "ReturnNil" };

    /// If the top of the stack is True, pop and discard it.
    /// If the top of the stack is False, pop it and jump ahead.
    /// The distance by which to jump is the value of the
    /// second and third instruction bytes interpreted as a 16-bit integer. If the top of the stack is neither True nor
    /// False, call mustBeBoolean on it without popping the value.
    static constexpr details::SimpleOpSpec<0xF8, Operands::NumericByte<16, 1>, Operands::NumericByte<16, 0>>
        JumpIfFalse { "JumpIfFalse" };

    /// If the top of the stack is True, pop and discard it.
    /// If the top of the stack is False, pop it, push nil, and jump ahead. The distance by which to jump is the value
    /// of the second and third instruction bytes interpreted as a 16-bit integer. If the top of the stack is neither
    /// True nor False, call mustBeBoolean on it without popping the value.
    static constexpr details::SimpleOpSpec<0xF9, Operands::NumericByte<16, 1>, Operands::NumericByte<16, 0>>
        JumpIfFalsePushNil { "JumpIfFalsePushNil" };

    /// If the top of the stack is True, pop and discard it.
    /// If the top of the stack is False, jump ahead without popping the value (i.e., leave False on the top of the
    /// stack). The distance by which to jump is the value of the second and third instruction bytes interpreted as a
    /// 16-bit integer. If the top of the stack is neither True nor False, call mustBeBoolean on it without popping the
    /// value.
    static constexpr details::SimpleOpSpec<0xFA, Operands::NumericByte<16, 1>, Operands::NumericByte<16, 0>>
        JumpIfFalsePushFalse { "JumpIfFalsePushFalse" };

    /// If the top of the stack is True, jump ahead and set the top of the stack to True. The distance by which to jump
    /// is the value of the second and third instruction bytes interpreted as a 16-bit integer. If the top of the stack
    /// is False, pop and discard it. If the top of the stack is neither True nor False, call mustBeBoolean on it
    /// without popping the value.
    static constexpr details::SimpleOpSpec<0xFB, Operands::NumericByte<16, 1>, Operands::NumericByte<16, 0>>
        JumpIfFalsePushTrue { "JumpIfFalsePushTrue" };

    /// Jump forward the number of instructions equal to the second and third instruction bytes interpreted as a 16-bit
    /// integer.
    static constexpr details::SimpleOpSpec<0xFC, Operands::NumericByte<16, 1>, Operands::NumericByte<16, 0>> JumpFwd {
        "JumpFwd"
    };

    /// Jump backward the number of instructions equal to the second and third instruction bytes interpreted as a 16-bit
    /// integer. Also pop and discard the top of the stack. From the source code: > also drops the stack. This saves an
    /// opcode in the while loop > which is the only place this opcode is used.
    static constexpr details::SimpleOpSpec<0xFD, Operands::NumericByte<16, 1>, Operands::NumericByte<16, 0>> JumpBack {
        "JumpBack"
    };

    /// Pop the top three objects on the stack and perform a binary operation on the last two using the top of the stack
    /// as an adverb. The adverb is ignored if the types of the two operands allow the operation to be performed
    /// primitively (i.e., Signal, float, and integer for most math operations). Place the result on the top of the
    /// stack. The second instruction byte indicates the index of the binary operation.
    static constexpr details::SimpleOpSpec<0xFE, Operands::TrinaryMath> SpecialBinaryOpWithAdverb {
        "SpecialBinaryOpWithAdverb"
    };

    /// Sets the tail call state to 1. TODO what does this mean?
    static constexpr details::SimpleOpSpec<0xFF> TailCallReturnFromMethod { "TailCallReturnFromMethod" };
};


/* opcodes */
enum {
    opExtended, //  0
    opPushInstVar,
    opPushTempVar,
    opPushTempZeroVar,
    opPushLiteral,
    opPushClassVar, //  5
    opPushSpecialValue,
    opStoreInstVar,
    opStoreTempVar,
    opStoreClassVar,
    opSendMsg, // 10
    opSendSuper,
    opSendSpecialMsg,
    opSendSpecialUnaryArithMsg,
    opSendSpecialBinaryArithMsg,
    opSpecialOpcode, // 15


    opNumOpcodes
};

/* special opcodes */
enum {
    opcDrop, // 0
    opcDup,
    opcFunctionReturn,
    opcReturn,
    opcReturnSelf,
    opcReturnTrue, //  5
    opcReturnFalse,
    opcReturnNil,
    opcJumpIfFalse, // IF 3 args
    opcJumpIfFalsePushNil, // IF 2 args
    opcJumpIfFalsePushFalse, // AND:		(10)
    opcJumpIfTruePushTrue, // OR:
    opcJumpFwd,
    opcJumpBak,
    opcSpecialBinaryOpWithAdverb,
    opcSuperSpecial, // 15

    opcNewList,

    opcNumSpecialOpcodes
    /*
        opcSlotAt,
        opcByteAt,			// 15
        opcShortAt,
        opcInt32At,
        opcColorAt,
        opcFloatAt,
        opcDoubleAt
    */
};

/* special unary math operators */

enum {
    opNeg,
    opNot,
    opIsNil,
    opNotNil,
    opBitNot,
    opAbs,
    opAsFloat,
    opAsInteger,
    opCeil, // 5
    opFloor,
    opFrac,
    opSign,
    opSquared,
    opCubed, // 10
    opSqrt,
    opExp,
    opRecip,
    opMIDICPS,
    opCPSMIDI, // 15

    opMIDIRatio,
    opRatioMIDI,
    opDbAmp,
    opAmpDb,
    opOctCPS, // 20
    opCPSOct,
    opLog,
    opLog2,
    opLog10,
    opSin, // 25
    opCos,
    opTan,
    opArcSin,
    opArcCos,
    opArcTan,
    opSinH,
    opCosH, // 30
    opTanH,
    opRand,
    opRand2,
    opLinRand,
    opBiLinRand,

    //	opExpRand,
    //	opBiExpRand,
    opSum3Rand,
    //	opGammaRand,
    //	opGaussRand,
    //	opPoiRand,

    opDistort,
    opSoftClip,
    opCoin,

    opDigitValue,
    opSilence,
    opThru,
    opRectWindow,
    opHanWindow,
    opWelchWindow,
    opTriWindow,

    opRamp,
    opSCurve,

    opNumUnarySelectors
};

// #define IS_UNARY_BOOL_OP(op) ((op) >= opCoin && (op) <= opOdd)
#define IS_BINARY_BOOL_OP(op) ((op) >= opEQ && (op) <= opGE)


/* special binary math operators */
enum {
    opAdd,
    opSub,
    opMul,
    opIDiv,
    opFDiv,
    opMod,
    opEQ,
    opNE,
    opLT,
    opGT,
    opLE,
    opGE,
    // opIdentical,
    // opNotIdentical,

    opMin,
    opMax,
    opBitAnd,
    opBitOr,
    opBitXor,
    opLCM,
    opGCD,
    opRound,
    opRoundUp,
    opTrunc,
    opAtan2,
    opHypot,
    opHypotx,
    opPow,
    opShiftLeft,
    opShiftRight,
    opUnsignedShift,
    opFill,
    opRing1, // a * (b + 1) == a * b + a
    opRing2, // a * b + a + b
    opRing3, // a*a*b
    opRing4, // a*a*b - a*b*b
    opDifSqr, // a*a - b*b
    opSumSqr, // a*a + b*b
    opSqrSum, // (a + b)^2
    opSqrDif, // (a - b)^2
    opAbsDif, // |a - b|
    opThresh,
    opAMClip,
    opScaleNeg,
    opClip2,
    opExcess,
    opFold2,
    opWrap2,
    opFirstArg,
    opRandRange,
    opExpRandRange,

    opNumBinarySelectors
};

/* other special math operators */
enum {
    /* 3 operands */
    opDivz,
    opClip,
    opWrap,
    opFold,
    opRampMult,
    opMix,
    /* 4 operands */
    opPoly3,
    /* 5 operands */
    opMapRange
};
enum {
    opmNew,
    opmInit,
    opmAt,
    opmPut,
    opmNext,
    opmReset,
    opmValue,
    opmCopyToEnd, // used by multi assign
    opmAdd, // used by dynamic list
    // opmIsNil,
    // opmNotNil,
    opmSize,
    opmClass,
    opmIf,
    opmWhile,
    opmFor,
    opmAnd,
    opmOr,
    opmCase,
    opmSwitch,
    opmIdentical,
    opmNotIdentical,
    opmPrint,
    opmRemove,
    opmIndexOf,
    opmWrapAt,
    opmClipAt,
    opmFoldAt,
    opmWrapPut,
    opmClipPut,
    opmFoldPut,
    opmDo,
    opmCollect,
    opmSelect,
    opmReject,
    opmAny,
    opmEvery,
    opmFind,
    opmChoose,
    opmValueList,
    opmAddFirst,
    opmPrimitiveFailed,
    opmSubclassResponsibility,
    opmShouldNotImplement,
    opmNotYetImplemented,
    opmDoesNotUnderstand,

    opmAtSign,
    opmWrapAtSign,
    opmClipAtSign,
    opmFoldAtSign,

    opmNewClear,
    opmNewCopyArgs,
    opmMultiNew,
    opmMultiNewList,
    opmAR,
    opmKR,
    opmIR,

    opmCopy,
    opmPerformList,
    opmIsKindOf,
    opmPostln,
    opmAsString,

    opmEnvirGet,
    opmEnvirPut,

    opmHalt,
    opmForBy,
    opmForSeries,
    opmReverseDo,
    opmLoop,

    opmNonBooleanError,

    opmPlusPlus,
    opmLTLT,
    opmQuestionMark,
    opmDoubleQuestionMark,
    opmExclamationQuestionMark,

    opmYield,
    opmName,
    opmMulAdd,

    opmSeries,

    opmNumSpecialSelectors
};

enum {
    opsvSelf, // 0
    opsvMinusOne,
    opsvNegOne,
    opsvZero,
    opsvOne,
    opsvTwo, // 5
    opsvFHalf,
    opsvFNegOne,
    opsvFZero,
    opsvFOne,
    opsvFTwo, // 10
    opsvPlusOne,
    opsvTrue,
    opsvFalse,
    opsvNil,
    opsvInf, // 15

    opsvNumSpecialValues
};


enum {
    opgProcess,
    opgMethod,
    opgFunctionDef,
    opgFunction,
    opgThread,
    // opgSampleRate,
    // opgAudioClock,
    // opgLogicalClock,

    opgNumPseudoVars
};

/* selector types */
enum {
    selNormal,
    selSpecial,
    selUnary,
    selBinary,
    selIf,
    selWhile,
    selAnd,
    selOr,
    selCase,
    selSwitch,
    selLoop,
    selSuperNew,
    selQuestionMark,
    selDoubleQuestionMark,
    selExclamationQuestionMark,

    selNumSelectorTypes
};


/*
    special classes:
    Object, List, Number, Int, Float, Signal, Complex, Point
*/


enum {
    op_class_object,
    op_class_symbol,
    op_class_nil,
    op_class_boolean,
    op_class_true,
    op_class_false,
    op_class_magnitude,
    op_class_char,
    op_class_number,
    op_class_complex,
    op_class_simple_number,
    op_class_int,
    op_class_float,
    op_class_method,
    op_class_fundef,
    op_class_stream,
    op_class_func,
    op_class_frame,
    op_class_process,
    op_class_main,
    op_class_class,
    op_class_string,
    op_class_collection,
    op_class_sequenceable_collection,
    op_class_arrayed_collection,
    op_class_array,
    op_class_int8array,
    op_class_int16array,
    op_class_int32array,
    op_class_floatarray,
    op_class_signal,
    op_class_doublearray,
    op_class_symbolarray,
    op_class_list,
    op_class_linkedlist,
    op_class_bag,
    op_class_set,
    op_class_identityset,
    op_class_dictionary,
    op_class_identitydictionary,
    op_class_sortedlist,
    op_class_synth,
    op_class_ref,
    op_class_environment,
    op_class_event,
    op_class_wavetable,
    op_class_env,

    op_class_routine,
    op_class_color,
    op_class_rect,

    op_NumSpecialClasses
};
