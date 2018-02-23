all: call_x86.masm call_x64.masm

%.masm: %.S
	${CPP} -E -P -DGEN_MASM $< -o $@
