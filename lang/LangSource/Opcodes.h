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
#include "OpcodesTypes.h"


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
   - These use the second nibble of the bytecode as a number or enum

Most opcodes are simple.

*/


namespace Opcode {


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
static constexpr details::SimpleOpSpec<0x02, Operands::FrameOffset, Operands::Index> PushTempVarX { "PushTempVarX" };

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
static constexpr details::SimpleOpSpec<0x08, Operands::FrameOffset, Operands::Index> StoreTempVarX { "StoreTempVarX" };

/// Store the top of the stack in a class variable, without popping the stack.
/// The second and third instruction bytes indicate the index of the variable within the VM's classvars field.
/// Only used in class method code.
static constexpr details::SimpleOpSpec<0x09, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>>
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

static constexpr details::SimpleOpSpec<0x20, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>> JumpIfTrue {
    "JumpIfTrue"
};

/// Note: because the frameoffset of 0 is handled by a separate opcode, you need to subtract one from the real
/// value. Push a variable from an enclosing frame onto the stack. The second nibble of the first instruction byte
/// gives the distance in frames between the current frame and the desired target. The second instruction byte
/// determines the index of the variable within that frame. For instance, 25 02 indicates the third variable of the
/// fifth frame outside this frame. { var a, b, c; { { { { { c.floop }.def.dumpByteCodes }.value }.value }.value
/// }.value }.value
static constexpr details::SecondNibbleOpSpec<0x21, 0x28, Operands::Index> PushTempVar { "PushTempVar" };

/// Push a constant from the current frame onto the stack. The second instruction byte indicates the index of the
/// constant.
/// This message is used instead of 4X PushLiteral when the index is >= 16.
/// { a = [\a, \b, \c, \d, \e, \f, \g, \h, \i, \j, \k, \l, \m, \n, \o, \p, \q] }.def.dumpByteCodes
static constexpr details::SimpleOpSpec<0x28, Operands::UnsignedInt<8, 0>> PushConstant8 { "PushConstant8" };

/// Push a constant from the current frame onto the stack.
/// The second and third instruction bytes interpreted as a 16-bit integer indicate the index of the constant.
/// This message is used instead of 28 PushConstant when the index is >= 256.
static constexpr details::SimpleOpSpec<0x29, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>>
    PushConstant16 { "PushConstant16" };


/// Push a constant from the current frame onto the stack.
/// The second, third and fourth instruction bytes interpreted as a 24-bit integer indicate the index of the
/// constant. This message is used instead of 29 PushConstant when the index is >= 2^16.
static constexpr details::SimpleOpSpec<0x2A, Operands::UnsignedInt<24, 2>, Operands::UnsignedInt<24, 1>,
                                       Operands::UnsignedInt<24, 0>>
    PushConstant24 { "PushConstant24" };

/// Push a constant from the current frame onto the stack.
/// The second to fifth instruction bytes interpreted as a 32-bit integer indicate the index of the constant.
/// This message is used instead of 2A PushConstant when the index is >= 2^24.
static constexpr details::SimpleOpSpec<0x2B, Operands::UnsignedInt<32, 3>, Operands::UnsignedInt<32, 2>,
                                       Operands::UnsignedInt<32, 1>, Operands::UnsignedInt<32, 0>>
    PushConstant32 { "PushConstant32" };

/// Push an 8 bit integer (signed) onto the stack from the operand.
static constexpr details::SimpleOpSpec<0x2C, Operands::Int<8, 0>> PushInteger8 { "PushInteger8" };

/// Push an 16 bit integer (signed) onto the stack from the operand.
static constexpr details::SimpleOpSpec<0x2D, Operands::Int<16, 1>, Operands::Int<16, 0>> PushInteger16 {
    "PushInteger16"
};


/// Push an 24 bit integer (signed) onto the stack from the operand.
static constexpr details::SimpleOpSpec<0x2E, Operands::Int<24, 2>, Operands::Int<24, 1>, Operands::Int<24, 0>>
    PushInteger24 { "PushInteger24" };

/// Push an 32 bit integer (signed) onto the stack from the operand.
static constexpr details::SimpleOpSpec<0x2F, Operands::Int<32, 3>, Operands::Int<32, 2>, Operands::Int<32, 1>,
                                       Operands::Int<32, 0>>
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
static constexpr details::SecondNibble12bitSpec<0x50, 0x60> PushClassVar { "PushClassVar " };

/// Pushes 'this' to the top of the stack.
static constexpr details::SimpleOpSpec<0x60> PushSpecialValueThis { "PushSpecialValueThis" };


/// If the top of the stack is an integer, this instruction decrements it by one.
/// Otherwise, the integer value one is pushed to the top of the stack and prSubNum is called.
/// { |x| x - 1; }.def.dumpByteCodes
static constexpr details::SimpleOpSpec<0x61> PushOneAndSubtract { "PushOneAndSubtract" };

/// Pushes a commonly used numerical value to the top of the stack.
static constexpr details::SecondNibbleViaEnumOpSpec<0x62, 0x6B, Operands::SpecialNumber> PushSpecialNumber {
    "PushSpecialNumber "
};

/// If the top of the stack is an integer, this instruction increments it by one.
/// Otherwise, the integer value one is pushed to the top of the stack and prAddNum is called.
static constexpr details::SimpleOpSpec<0x6B> PushOneAndAddOne { "PushOneAndAddOne" };


/// Pushes a commonly used value to the top of the stack.
/// True, False, Nil_, or Inf.
static constexpr details::SecondNibbleViaEnumOpSpec<0x6c, 0x70, Operands::SpecialValue> PushSpecialValue {
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
static constexpr details::SimpleOpSpec<0x88, Operands::Index, Operands::Index> PushInstVarAndSendSpecialMsg {
    "PushInstVarAndSendSpecialMsg"
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

/// Extended Opcodes are a set of opcodes within the normals opcodes.
/// They all have a prefix of 0\x8F.
static constexpr details::SimpleOpSpec<0x8F, Operands::Index> ExtendedCodes { "ExtendedCodes" };

/// Note, the extended opcodes are often used to implement loops;
/// they assume opcodes will be emitted in a particular sequence,
/// so the instruction pointer can jump around to implement the loop in the interpreter.
namespace Extended {
static constexpr Byte Prefix = ExtendedCodes.code;

/// The frame of this method holds the receiver (loop limit), the function argument, and an accumulator variable
/// initially set to 0.
static constexpr struct IntegerDo {
    /// The accumulator is tested against the receiver and, if greater than or equal to it, control
    /// exits the method. Otherwise, the function and two copies of the accumulator are pushed to the stack, and `value`
    /// is invoked on all three.
    details::SimpleOpSpec<0x00> LoopOrReturn { "IntDo-LoopOrReturn" };

    /// The top of the stack is popped, discarding the result of `value`.
    /// The IP is decremented by the size of this instruction, and the the size of IntegerDoLoopOrReturn.
    /// If using with other 'do' loop codes, ensure their size matches IntegerDoLoopOrReturn.
    details::SimpleOpSpec<0x01> DropAndJumpBackToLoop { "IntDo-DropAndJumpBackToLoop" };

    /// Jump from the drop instruction to the loop
    unsigned int jumpSize() const { return 1U + LoopOrReturn.byteSize + 1U + DropAndJumpBackToLoop.byteSize; }


    void emit() const {
        emitByte(Prefix);
        LoopOrReturn.emit();
        emitByte(Prefix);
        DropAndJumpBackToLoop.emit();
    }
} IntegerDo;


/// The frame of this method holds the receiver (loop limit), the function argument, and two accumulators (`i`, `j`).
/// `i` is used as the reverse accumulator, while `j` is the forward accumulator.
static constexpr struct IntegerReverseDo {
    /// `i` is initialized to the receiver minus 1.
    details::SimpleOpSpec<0x02> Init { "IntReverseDo-Init" };
    /// `i` is tested against 0 and, if less than it, control exits the method. Otherwise, the function, `i`, and `j`
    /// are pushed to the stack and `value` is invoked on all three.
    details::SimpleOpSpec<0x03> LoopOrReturn { "IntReverseDo-LoopOrReturn" };
    /// The top of the stack is popped, discarding the result of `value`. `i` is decremented,
    /// `j` is incremented, and the IP is set back to `8F 03`.
    details::SimpleOpSpec<0x04> DropAndJumpBackToLoop { "IntReverseDo-DropAndJumpBackToLoop" };

    /// Jump from the drop instruction to the loop, don't jump back to the init instruction.
    unsigned int jumpSize() const { return 1U + LoopOrReturn.byteSize + 1U + DropAndJumpBackToLoop.byteSize; }

    void emit() const {
        emitByte(Prefix);
        Init.emit();
        emitByte(Prefix);
        LoopOrReturn.emit();
        emitByte(Prefix);
        DropAndJumpBackToLoop.emit();
    }
} IntegerReverseDo;


/// Implements `Integer:-for`: `8F 05 8F 06 8F 10`. The frame of this method holds the receiver (loop start),
/// the loop limit, the function argument, the accumulator, the loop count accumulator, and `stepval`.
static constexpr struct IntegerFor {
    /// The type of the loop limit is inspected. If it is an int, nothing happens; if it is a
    /// float, it is coerced to an int value using a direct cast.
    /// Otherwise, an error is thrown.
    /// Next, `stepval` is set to 1 or -1 depending on the direction of iteration: if the loop limit is <=the loop
    /// start, `stepval := 1`, otherwise -1. Finally, the accumulator is set to the value of the receiver.
    details::SimpleOpSpec<0x05> Init { "IntFor-Init" };

    /// The accumulator is checked: if the accumulator is beyond the loop limit, given the
    /// direction of iteration, the method is exited. Otherwise, the function argument, the accumulator, and
    /// loop count accumulator are pushed to the stack and `value` is invoked on all three.
    details::SimpleOpSpec<0x06> LoopOrReturn { "IntFor-LoopOrReturn" };

    /// The top of the stack is popped, discarding the result of `value`. `i` is incremented by
    /// `stepval`, `j` is incremented, and the IP is set back to `LoopOrReturn
    /// TODO: Change this number to 7 and bump everything along.
    details::SimpleOpSpec<0x10> DropAndJumpBackToLoop { "IntFor-DropAndJumpBackToLoop" };

    /// Jump from the drop instruction to the loop, don't jump back to the init instruction.
    unsigned int jumpSize() const { return 1U + LoopOrReturn.byteSize + 1U + DropAndJumpBackToLoop.byteSize; }

    void emit() const {
        emitByte(Prefix);
        Init.emit();
        emitByte(Prefix);
        LoopOrReturn.emit();
        emitByte(Prefix);
        DropAndJumpBackToLoop.emit();
    }
} IntegerFor;

static constexpr struct IntegerForBy {
    /// Check arguments are integers (or cast if float) and check for inf, or zero step size.
    details::SimpleOpSpec<0x07> Init { "IntForBy-Init" };
    details::SimpleOpSpec<0x08> LoopOrReturn { "IntForBy-LoopOrReturn" };
    details::SimpleOpSpec<0x09> DropAndJumpBackToLoop { "IntForBy-DropAndJumpBackToLoop" };
    /// Jump from the drop instruction to the loop, don't jump back to the init instruction.
    unsigned int jumpSize() const { return 1U + LoopOrReturn.byteSize + 1U + DropAndJumpBackToLoop.byteSize; }
    void emit() const {
        emitByte(Prefix);
        Init.emit();
        emitByte(Prefix);
        LoopOrReturn.emit();
        emitByte(Prefix);
        DropAndJumpBackToLoop.emit();
    }

} IntegerForBy;


/// Implements `ArrayedCollection:-do`. The frame of this method holds the receiver, the
/// apply function, and the loop accumulator (initially 0).
static constexpr struct ArrayedCollectionDo {
    /// The accumulator is inspected. If it is less than the size of the array, then the apply
    /// function, indexed element of the receiver array, and accumulator are pushed to the stack and `value`
    /// is invoked on all three.
    details::SimpleOpSpec<0x0A> LoopOrReturn { "ArrayedCollectionDo-Init" };

    /// As previously, the result of `value` is dropped, the accumulator is incremented, and the IP
    /// is set to Init.
    /// NOTE: this is a duplicate of a previous code! It relies on the jumpSize being the same.
    /// This could be changed in the future so it has its own unique code.
    details::SimpleOpSpec<0x01> DropAndJumpBackToLoop { "ArrayedCollectionDo-DropAndJumpBackToLoop" };

    /// Jump from the drop instruction to the loop, don't jump back to the init instruction.
    unsigned int jumpSize() const { return 1U + LoopOrReturn.byteSize + 1U + DropAndJumpBackToLoop.byteSize; }

} ArrayedCollectionDo;
}

/// Pop and store the top of the stack in a class variable of the current class.
/// The index of the class variable is found by interpreting the second nibble of the first byte and the entire
/// second byte as a 12-bit integer. Only used in class code. Meta_String.findMethod('initClass').dumpByteCodes
static constexpr details::SecondNibble12bitSpec<0x90, 0xA0> StoreClassVar { "StoreClassVar" };

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

static constexpr details::SimpleOpSpec<0xB1, Operands::Index> SendSuperMsgThisOpt { "SendSuperMsgThisOpt" };
/// NOTE: because zero args is handled elsewhere, you must subtract one from the total number of args before setting
/// nibble. Call a method on super taking a number of arguments. The second nibble of the first instruction bytes
/// determines the number of arguments passed to the method. The second instruction byte determines the index of the
/// selector within the block. Mostly used in class code, as in an interpreted context it refers to Interpreter.
static constexpr details::SecondNibbleOpSpec<0xB2, 0xC0, Operands::Index> SendSuperMsg { "SendSuperMsg" };

/// Push this to the top of the stack and call a special method taking one argument.
/// The second instruction byte determines the index of the special method within gSpecialSelectors.
/// { this.init }.def.dumpByteCodes
static constexpr details::SimpleOpSpec<0xC0, Operands::Index> SendSpecialMsgThisOpt { "SendSpecialMsgThisOpt" };


/// NOTE: because zero args is handled elsewhere, you must subtract one from the total number of args before setting
/// Call a special method taking a number of arguments.
/// The second nibble of the first instruction byte determines the number of arguments passed to the method.
/// The second instruction byte determines the index of the special method within gSpecialSelectors.
/// { 5.init(8, 9, 10) }.def.dumpByteCodes
static constexpr details::SecondNibbleOpSpec<0xC1, 0xD0, Operands::SpecialSelectors> SendSpecialMsg {
    "SendSpecialMsg"
};

/// Perform a unary operation in-place on the top of the stack.
static constexpr details::SecondNibbleViaEnumOpSpec<0xD0, 0xE0, Operands::UnaryMathNibble> SendSpecialUnaryArithMsg {
    "SendSpecialUnaryArithMsg"
};

/// Perform a binary operation in-place on the top of the stack.
static constexpr details::SecondNibbleViaEnumOpSpec<0xE0, 0xF0, Operands::BinaryMathNibble> SendSpecialBinaryArithMsg {
    "SendSpecialBinaryArithMsg"
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
static constexpr details::SimpleOpSpec<0xF8, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>> JumpIfFalse {
    "JumpIfFalse"
};

/// If the top of the stack is True, pop and discard it.
/// If the top of the stack is False, pop it, push nil, and jump ahead. The distance by which to jump is the value
/// of the second and third instruction bytes interpreted as a 16-bit integer. If the top of the stack is neither
/// True nor False, call mustBeBoolean on it without popping the value.
static constexpr details::SimpleOpSpec<0xF9, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>>
    JumpIfFalsePushNil { "JumpIfFalsePushNil" };

/// If the top of the stack is True, pop and discard it.
/// If the top of the stack is False, jump ahead without popping the value (i.e., leave False on the top of the
/// stack). The distance by which to jump is the value of the second and third instruction bytes interpreted as a
/// 16-bit integer. If the top of the stack is neither True nor False, call mustBeBoolean on it without popping the
/// value.
static constexpr details::SimpleOpSpec<0xFA, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>>
    JumpIfFalsePushFalse { "JumpIfFalsePushFalse" };

/// If the top of the stack is True, jump ahead and set the top of the stack to True. The distance by which to jump
/// is the value of the second and third instruction bytes interpreted as a 16-bit integer. If the top of the stack
/// is False, pop and discard it. If the top of the stack is neither True nor False, call mustBeBoolean on it
/// without popping the value.
/// TODO: This does not appear to be used.
static constexpr details::SimpleOpSpec<0xFB, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>>
    JumpIfTruePushTrue { "JumpIfTruePushTrue" };

/// Jump forward the number of instructions equal to the second and third instruction bytes interpreted as a 16-bit
/// integer.
static constexpr details::SimpleOpSpec<0xFC, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>> JumpFwd {
    "JumpFwd"
};

/// Jump backward the number of instructions equal to the second and third instruction bytes interpreted as a 16-bit
/// integer. Also pop and discard the top of the stack. From the source code: > also drops the stack. This saves an
/// opcode in the while loop > which is the only place this opcode is used.
static constexpr details::SimpleOpSpec<0xFD, Operands::UnsignedInt<16, 1>, Operands::UnsignedInt<16, 0>> JumpBack {
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
