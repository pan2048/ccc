include external/Makefile
include lib/Makefile
include model/Makefile
include lexer/Makefile
include parser/Makefile
include codegen/Makefile
include tool/Makefile

# Define the location of the include directory

# and the location to install the compiler binary
INCDIR=/tmp/include
BINDIR=/tmp

HSRCS= main.h incdir.h
SRCS= main.c $(EXTERNAL_FILES) $(LIB_FILES) $(MODEL_FILES) $(LEXER_FILES) $(PARSER_FILES) $(CODEGEN_X64_FILES) $(TOOL_FILES)

cwj: $(SRCS) $(HSRCS)
	cc -o bin/cwj -g -Wall $(SRCS)

incdir.h:
	echo "#define INCDIR \"$(INCDIR)\"" > incdir.h

install: cwj
	mkdir -p $(INCDIR)
	rsync -a include/. $(INCDIR)
	cp bin/cwj $(BINDIR)
	chmod +x $(BINDIR)/cwj

clean:
	rm -f bin/* cwj cwj[0-9] cwjarm *.o */*.o */*/*.o *.s out a.out incdir.h

test: install tests/runtests
	(cd tests; chmod +x runtests; ./runtests)

# Run the tests with the
# compiler that compiled itself
test0: install tests/runtests0 cwj0
	(cd tests; chmod +x runtests0; ./runtests0)

armtest: cwjarm tests/runtests
	(cd tests; chmod +x runtests; ./runtests)

quad: cwj2
	size bin/cwj[012]

cwj2: cwj1 $(SRCS) $(HSRCS)
	./bin/cwj1 -o bin/cwj2 $(SRCS)

cwj1: cwj0 $(SRCS) $(HSRCS)
	./bin/cwj0 -o bin/cwj1 $(SRCS)

cwj0: install $(SRCS) $(HSRCS)
	./bin/cwj -o bin/cwj0 $(SRCS)

