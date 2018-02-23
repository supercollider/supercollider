CFLAGS = 
MODULES = main.o dispatch.o f.o node.o call.o
TARGETS = calls.s test
all: $(TARGETS)
test: $(MODULES)
	$(CC) $^ -o $@
%.s: %.c
	$(CC) -S $< -o $@
clean:
	$(RM) $(TARGETS) *.o
