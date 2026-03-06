#pragma once
#include "Opcodes.h"
#include <ostream>
#include <tuple>
#include <utility>


namespace Opcode {

namespace details {
// from cpp ref
[[noreturn]] inline void opcode_unreachable() {
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}
}

#define OpcodeCase(NAME) case NAME.code

#define OpcodeCase16(NAME)                                                                                             \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
    case NAME.codeOffset<8>():                                                                                         \
    case NAME.codeOffset<9>():                                                                                         \
    case NAME.codeOffset<10>():                                                                                        \
    case NAME.codeOffset<11>():                                                                                        \
    case NAME.codeOffset<12>():                                                                                        \
    case NAME.codeOffset<13>():                                                                                        \
    case NAME.codeOffset<14>():                                                                                        \
    case NAME.codeOffset<15>()

#define OpcodeCase15(NAME)                                                                                             \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
    case NAME.codeOffset<8>():                                                                                         \
    case NAME.codeOffset<9>():                                                                                         \
    case NAME.codeOffset<10>():                                                                                        \
    case NAME.codeOffset<11>():                                                                                        \
    case NAME.codeOffset<12>():                                                                                        \
    case NAME.codeOffset<13>():                                                                                        \
    case NAME.codeOffset<14>()

#define OpcodeCase14(NAME)                                                                                             \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
    case NAME.codeOffset<8>():                                                                                         \
    case NAME.codeOffset<9>():                                                                                         \
    case NAME.codeOffset<10>():                                                                                        \
    case NAME.codeOffset<11>():                                                                                        \
    case NAME.codeOffset<12>():                                                                                        \
    case NAME.codeOffset<13>()

#define OpcodeCase9(NAME)                                                                                              \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
    case NAME.codeOffset<8>()

#define OpcodeCase8(NAME)                                                                                              \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>()

#define OpcodeCase7(NAME)                                                                                              \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>()

#define OpcodeCase4(NAME)                                                                                              \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>()


template <typename Action> void forSingleBytecode(Action& action, unsigned char*& ip) {
    // Rules regarding instruction pointer increment.
    // While the main interpreter loop always advances the ip, here, we let the opcodes do that.
    switch (ip[0]) {
        OpcodeCase(PushClassX): PushClassX.destructure(action, ip);
        return;
        OpcodeCase(PushInstVarX): PushInstVarX.destructure(action, ip);
        return;
        OpcodeCase(PushTempVarX): PushTempVarX.destructure(action, ip);
        return;
        OpcodeCase(PushTempZeroVarX): PushTempZeroVarX.destructure(action, ip);
        return;
        OpcodeCase(PushLiteralX): PushLiteralX.destructure(action, ip);
        return;
        OpcodeCase(PushClassVarX): PushClassVarX.destructure(action, ip);
        return;
        OpcodeCase(PushSpecialClass): PushSpecialClass.destructure(action, ip);
        return;
        OpcodeCase(StoreInstVarX): StoreInstVarX.destructure(action, ip);
        return;
        OpcodeCase(StoreTempVarX): StoreTempVarX.destructure(action, ip);
        return;
        OpcodeCase(StoreClassVarX): StoreClassVarX.destructure(action, ip);
        return;
        OpcodeCase(SendMsgX): SendMsgX.destructure(action, ip);
        return;
        OpcodeCase(SendSuperMsgX): SendSuperMsgX.destructure(action, ip);
        return;
        OpcodeCase(SendSpecialMsgX): SendSpecialMsgX.destructure(action, ip);
        return;
        OpcodeCase(SendSpecialUnaryArithMsgX): SendSpecialUnaryArithMsgX.destructure(action, ip);
        return;
        OpcodeCase(SendSpecialBinaryArithMsgX): SendSpecialBinaryArithMsgX.destructure(action, ip);
        return;
        OpcodeCase(SpecialOpcode): SpecialOpcode.destructure(action, ip);
        return;
        OpcodeCase16(PushInstVar): PushInstVar.destructure(action, ip);
        return;
        OpcodeCase(JumpIfTrue): JumpIfTrue.destructure(action, ip);
        return;
        OpcodeCase7(PushTempVar): PushTempVar.destructure(action, ip);
        return;
        OpcodeCase(PushConstant8): PushConstant8.destructure(action, ip);
        return;
        OpcodeCase(PushConstant16): PushConstant16.destructure(action, ip);
        return;
        OpcodeCase(PushConstant24): PushConstant24.destructure(action, ip);
        return;
        OpcodeCase(PushConstant32): PushConstant32.destructure(action, ip);
        return;
        OpcodeCase(PushInteger8): PushInteger8.destructure(action, ip);
        return;
        OpcodeCase(PushInteger16): PushInteger16.destructure(action, ip);
        return;
        OpcodeCase(PushInteger24): PushInteger24.destructure(action, ip);
        return;
        OpcodeCase(PushInteger32): PushInteger32.destructure(action, ip);
        return;
        OpcodeCase16(PushTempZeroVar): PushTempZeroVar.destructure(action, ip);
        return;
        OpcodeCase16(PushLiteral): PushLiteral.destructure(action, ip);
        return;
        OpcodeCase16(PushClassVar): PushClassVar.destructure(action, ip);
        return;
        OpcodeCase(PushSpecialValueThis): PushSpecialValueThis.destructure(action, ip);
        return;
        OpcodeCase(PushOneAndSubtract): PushOneAndSubtract.destructure(action, ip);
        return;
        OpcodeCase9(PushSpecialNumber): PushSpecialNumber.destructure(action, ip);
        return;
        OpcodeCase(PushOneAndAddOne): PushOneAndAddOne.destructure(action, ip);
        return;
        OpcodeCase4(PushSpecialValue): PushSpecialValue.destructure(action, ip);
        return;
        OpcodeCase16(StoreInstVar): StoreInstVar.destructure(action, ip);
        return;
        OpcodeCase8(StoreTempVar): StoreTempVar.destructure(action, ip);
        return;
        OpcodeCase(PushInstVarAndSendSpecialMsg): PushInstVarAndSendSpecialMsg.destructure(action, ip);
        return;
        OpcodeCase(PushAllArgsAndSendMsg): PushAllArgsAndSendMsg.destructure(action, ip);
        return;
        OpcodeCase(PushAllButFirstArgAndSendMsg): PushAllButFirstArgAndSendMsg.destructure(action, ip);
        return;
        OpcodeCase(PushAllArgsAndSendSpecialMsg): PushAllArgsAndSendSpecialMsg.destructure(action, ip);
        return;
        OpcodeCase(PushAllButFirstArgAndSendSpecialMsg): PushAllButFirstArgAndSendSpecialMsg.destructure(action, ip);
        return;
        OpcodeCase(PushAllButFirstTwoArgsAndSendMsg): PushAllButFirstTwoArgsAndSendMsg.destructure(action, ip);
        return;
        OpcodeCase(PushAllButFirstTwoArgsAndSendSpecialMsg):
            PushAllButFirstTwoArgsAndSendSpecialMsg.destructure(action, ip);
        return;

    case ExtendedCodes.code: {
        // look forward to the next value after the prefix
        switch (ip[1]) {
            OpcodeCase(Extended::IntegerDo.LoopOrReturn): Extended::IntegerDo.LoopOrReturn.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerDo.DropAndJumpBackToLoop):
                Extended::IntegerDo.DropAndJumpBackToLoop.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerReverseDo.Init): Extended::IntegerReverseDo.Init.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerReverseDo.LoopOrReturn):
                Extended::IntegerReverseDo.LoopOrReturn.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerReverseDo.DropAndJumpBackToLoop):
                Extended::IntegerReverseDo.DropAndJumpBackToLoop.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerFor.Init): Extended::IntegerFor.Init.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerFor.LoopOrReturn): Extended::IntegerFor.LoopOrReturn.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerFor.DropAndJumpBackToLoop):
                Extended::IntegerFor.DropAndJumpBackToLoop.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerForBy.Init): Extended::IntegerForBy.Init.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerForBy.LoopOrReturn):
                Extended::IntegerForBy.LoopOrReturn.destructure(action, ip);
            return;
            OpcodeCase(Extended::IntegerForBy.DropAndJumpBackToLoop):
                Extended::IntegerForBy.DropAndJumpBackToLoop.destructure(action, ip);
            return;
            OpcodeCase(Extended::ArrayedCollectionDo.LoopOrReturn):
                Extended::ArrayedCollectionDo.LoopOrReturn.destructure(action, ip);
            return;
            // This is a duplication, remove it by duplicating the logic in the main interpreter loop.
            // PrintOntoConsumeIP(Extended::ArrayedCollectionDo.DropAndJumpBackToLoop, stream, ip);
            OpcodeCase(Extended::ArrayedCollectionReversedDo.Init):
                Extended::ArrayedCollectionReversedDo.Init.destructure(action, ip);
            return;
            OpcodeCase(Extended::ArrayedCollectionReversedDo.LoopOrReturn):
                Extended::ArrayedCollectionReversedDo.LoopOrReturn.destructure(action, ip);
            return;
            // This is a duplication, remove it by duplicating the logic in the main interpreter loop.
            // PrintOntoConsumeIP(Extended::ArrayedCollectionReversedDo.DropAndJumpBackToLoop, stream, ip);
            OpcodeCase(Extended::DictionaryKeyValuesArrayDo.LoopOrReturn):
                Extended::DictionaryKeyValuesArrayDo.LoopOrReturn.destructure(action, ip);
            return;
            OpcodeCase(Extended::DictionaryKeyValuesArrayDo.DropAndJumpBackToLoop):
                Extended::DictionaryKeyValuesArrayDo.DropAndJumpBackToLoop.destructure(action, ip);
            return;
            OpcodeCase(Extended::FloatDo.LoopOrReturn): Extended::FloatDo.LoopOrReturn.destructure(action, ip);
            return;
            OpcodeCase(Extended::FloatDo.DropAndJumpBackToLoop):
                Extended::FloatDo.DropAndJumpBackToLoop.destructure(action, ip);
            return;
            OpcodeCase(Extended::QuestionMark.IsNil): Extended::QuestionMark.IsNil.destructure(action, ip);
            return;
            OpcodeCase(Extended::DoubleQuestionMark.Jump): Extended::DoubleQuestionMark.Jump.destructure(action, ip);
            return;
            OpcodeCase(Extended::IfNotNilJump.Jump): Extended::IfNotNilJump.Jump.destructure(action, ip);
            return;
            OpcodeCase(Extended::IfNilJump.Jump): Extended::IfNilJump.Jump.destructure(action, ip);
            return;
            OpcodeCase(Extended::IfNotNilJumpPushNilElsePop.Jump):
                Extended::IfNotNilJumpPushNilElsePop.Jump.destructure(action, ip);
            return;
            OpcodeCase(Extended::IfNilThenJumpElsePopNil.Jump):
                Extended::IfNilThenJumpElsePopNil.Jump.destructure(action, ip);
            return;
            OpcodeCase(Extended::Switch.LookupAndJump): Extended::Switch.LookupAndJump.destructure(action, ip);
            return;
            OpcodeCase(Extended::FloatDoReverse.Init): Extended::FloatDoReverse.Init.destructure(action, ip);
            return;
            OpcodeCase(Extended::FloatDoReverse.LoopOrReturn):
                Extended::FloatDoReverse.LoopOrReturn.destructure(action, ip);
            return;
            OpcodeCase(Extended::FloatDoReverse.DropAndJumpBackToLoop):
                Extended::FloatDoReverse.DropAndJumpBackToLoop.destructure(action, ip);
            return;
            OpcodeCase(Extended::NumberForSeries.Init): Extended::NumberForSeries.Init.destructure(action, ip);
            return;
            OpcodeCase(Extended::NumberForSeries.LoopOrReturn):
                Extended::NumberForSeries.LoopOrReturn.destructure(action, ip);
            return;
            OpcodeCase(Extended::NumberForSeries.DropAndJumpBackToLoop):
                Extended::NumberForSeries.DropAndJumpBackToLoop.destructure(action, ip);
            return;
        default:
            assert(false);
            return; // details::opcode_unreachable();
        }
    };
        OpcodeCase16(StoreClassVar): StoreClassVar.destructure(action, ip);
        return;
        OpcodeCase(SendMsgThisOpt): SendMsgThisOpt.destructure(action, ip);
        return;
        OpcodeCase15(SendMsg): SendMsg.destructure(action, ip);
        return;
        OpcodeCase(TailCallReturnFromFunction): TailCallReturnFromFunction.destructure(action, ip);
        return;
        OpcodeCase(SendSuperMsgThisOpt): SendSuperMsgThisOpt.destructure(action, ip);
        return;
        OpcodeCase14(SendSuperMsg): SendSuperMsg.destructure(action, ip);
        return;
        OpcodeCase(SendSpecialMsgThisOpt): SendSpecialMsgThisOpt.destructure(action, ip);
        return;
        OpcodeCase15(SendSpecialMsg): SendSpecialMsg.destructure(action, ip);
        return;
        OpcodeCase16(SendSpecialUnaryArithMsg): SendSpecialUnaryArithMsg.destructure(action, ip);
        return;
        OpcodeCase16(SendSpecialBinaryArithMsg): SendSpecialBinaryArithMsg.destructure(action, ip);
        return;
        OpcodeCase(Drop): Drop.destructure(action, ip);
        return;
        OpcodeCase(Dup): Dup.destructure(action, ip);
        return;
        OpcodeCase(BlockReturn): BlockReturn.destructure(action, ip);
        return;
        OpcodeCase(Return): Return.destructure(action, ip);
        return;
        OpcodeCase(ReturnSelf): ReturnSelf.destructure(action, ip);
        return;
        OpcodeCase(ReturnTrue): ReturnTrue.destructure(action, ip);
        return;
        OpcodeCase(ReturnFalse): ReturnFalse.destructure(action, ip);
        return;
        OpcodeCase(ReturnNil): ReturnNil.destructure(action, ip);
        return;
        OpcodeCase(JumpIfFalse): JumpIfFalse.destructure(action, ip);
        return;
        OpcodeCase(JumpIfFalsePushNil): JumpIfFalsePushNil.destructure(action, ip);
        return;
        OpcodeCase(JumpIfFalsePushFalse): JumpIfFalsePushFalse.destructure(action, ip);
        return;
        OpcodeCase(JumpIfTruePushTrue): JumpIfTruePushTrue.destructure(action, ip);
        return;
        OpcodeCase(JumpFwd): JumpFwd.destructure(action, ip);
        return;
        OpcodeCase(JumpBack): JumpBack.destructure(action, ip);
        return;
        OpcodeCase(SpecialBinaryOpWithAdverb): SpecialBinaryOpWithAdverb.destructure(action, ip);
        return;
        OpcodeCase(TailCallReturnFromMethod): TailCallReturnFromMethod.destructure(action, ip);
        return;
    default:
        assert(false);
        return; // details::opcode_unreachable();
    }
};

template <typename Action> void forAllBytecodes(Action& action, unsigned char*& ip, const unsigned char* const ip_end) {
    while (ip < ip_end) {
        forSingleBytecode(action, ip);
    }
};


struct WriterAction {
    std::ostream& stream;

    template <typename... Operands> void operator()(const char* name, Operands&&... operands) {
        this->operator()(std::tuple { name }, std::forward<Operands>(operands)...);
    }

    template <typename... Tups, typename... Operands>
    void operator()(std::tuple<const char*, Tups...> codeInfo, Operands&&... operands) {
        std::apply([&](auto&&... infos) { ((stream << infos << " "), ...); }, codeInfo);

        if constexpr (sizeof...(Operands) != 0) {
            std::apply(
                [&](auto&&... ops) {
                    size_t operand_counter { 0 };
                    stream << '[';
                    const auto app = [&](auto op) {
                        size_t operand_value_counter { 0 };

                        std::apply(
                            [&](auto... values) {
                                if constexpr (sizeof...(values) != 0) {
                                    stream << "(";
                                    ((stream << values << (operand_value_counter != sizeof...(values) ? ", " : "")),
                                     ...);
                                    stream << ")";
                                }
                            },
                            op.asTuple());

                        operand_counter += 1;
                        if (operand_counter != sizeof...(Operands))
                            stream << ", ";
                    };
                    (app(ops), ...);
                    stream << ']';
                },
                std::tuple { std::forward<Operands>(operands)... });
        }
        stream << "\n";
    }
};

inline void printAllBytecodes(std::ostream& stream, unsigned char*& ip, const unsigned char* const ip_end) {
    WriterAction action { stream };
    forAllBytecodes(action, ip, ip_end);
}

}
