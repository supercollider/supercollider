# sc_bytecode

The sclang VM's bytecodes, written as C++ types instead of magic numbers.

The VM itself still reads raw bytes. This library describes what those bytes mean, and gives the compiler a safe way to write them.

## Layout

| File | Contents |
| --- | --- |
| `ByteCodeArray.h` | `CompilingBytecodes`, the buffer the compiler emits into. |
| `Opcodes.h` | One constant per opcode, with its documentation. |
| `SpecialNames.h` | Names of the special classes, selectors, and operators, indexed by their enums. |
| `details/OpcodeDetails.h` | The opcode spec templates (`SimpleOpSpec` and friends). |
| `details/OpcodeOperands.h` | Operand types and the enums behind them. |

## Design

### Opcodes are typed constants

Each opcode is a `static constexpr` object in `Opcodes.h`. Its type holds the byte value and the operand types:

```cpp
/// Push a temporary variable to the top of the stack.
static constexpr details::SimpleOpSpec<0x02, Operands::FrameOffset, Operands::Index> PushTempVarX { "PushTempVarX" };
```

To emit it, you pass the operands by type, so the compiler rejects a swapped or missing operand:

```cpp
Opcode::PushTempVarX.emit(
    bytecodes,
    loc, 
    Operands::FrameOffset { 1 }, 
    Operands::Index { 3 }
);
```

### Three kinds of opcode

- **Simple** (`SimpleOpSpec`): one code byte, then a fixed number of one byte operands.
- **Second nibble**: the low nibble of the code byte carries a value, such as a variable index. These are `SecondNibbleOpSpec`, `SecondNibbleNonZeroOpSpec`, `SecondNibble12bitSpec` (the nibble is the top four bits of a 12-bit number), and `SecondNibbleViaEnumOpSpec` (the nibble is an enum).
- **Extended**: the `0x8F` prefix, then a simple opcode. They are built with `WithPrefix`, and mostly implement loops. They live in `Opcode::Extended`, where related codes are grouped in a struct with a combined `emit`.

### Operands

Operands are small types in `OpcodeOperands.h`. There are two families:

- `NamedByte` wraps a plain byte (`Index`, `SelectorIndex`, `FrameOffset`, ...). The distinct types stop you mixing them up.
- `OperandEnumWrapper` wraps an enum with a `COUNT` entry (`SpecialClass`, `UnaryMath`, `BinaryMath`, ...). Decoding a byte outside the enum fails an assert.

Multi-byte numbers use `UnsignedInt<TOTAL, PART>` and `Int<TOTAL, PART>`. Each one is a single byte of a larger value, so a 16-bit operand is two entries, high byte first.

### Emitting and reading

Every spec has an `emit` that writes into `CompilingBytecodes`, and a `pullOperandsFromInstructions` that reads the operands back from an instruction pointer. Both come from the same type list, so writing and reading cannot disagree.

`CompilingBytecodes` stores three parallel arrays: the bytes, a source range for each opcode, and each opcode's size in bytes. 

## Adding a new opcode

1. Pick the code byte. Check that it is free and not inside a nibble range used by another opcode.
2. Add a `static constexpr` spec to `Opcodes.h`, next to similar opcodes, with a `///` comment saying what it does to the stack.
   - Fixed operands: `SimpleOpSpec<0xNN, Operands::A, Operands::B>`.
   - A value in the low nibble: one of the `SecondNibble*` specs, with `startCode` and an exclusive `endCode`.
   - An extended opcode: `ExtendedPrefix<details::SimpleOpSpec<0xNN, ...>>` inside `Opcode::Extended`. If several codes must be emitted together, group them in a struct with a combined `emit`, like `IntegerDo`.
3. If it needs an operand that does not exist yet, add one to `OpcodeOperands.h`. Use `NamedByte<YourType>` for a byte, or `OperandEnumWrapper` for an enum. Give the enum a `COUNT` entry and a `name`.
4. If it adds a special class, selector, or operator, add the name to `SpecialNames.h` in the same position as the enum. These must match the tables the VM builds in `lang/`.
5. The VM must also run the new code. A new opcode needs an interpreter case in `lang/`, or it will do nothing.
6. Add a regression test for the output the compiler gives, as the other compiler changes do.

## Credit

The opcode documentation is based on Moss Heim's work (2018) deciphering what each bytecode does.
