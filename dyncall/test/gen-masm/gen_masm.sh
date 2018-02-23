gcc -E -P -DGEN_MASM call_x86.S >call_x86.masm
gcc -E -P -DGEN_MASM call_x64.S >call_x64.masm

