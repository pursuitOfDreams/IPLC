CXX  = g++-8
EXE = iplC
CXXDEBUG = -g -Wall
CXXSTD = -std=c++11


.PHONY: all
all: parser lexer 
	$(CXX) $(CXXDEBUG) -o iplC main.cpp symbolTab.cpp ast.cpp parser.o scanner.o 2> error1.txt
	
parser: parser.yy scanner.hh 
	bison -d -v $<
	$(CXX) $(CXXDEBUG) -c parser.tab.cc -o parser.o 2> error.txt

lexer: scanner.l scanner.hh parser.tab.hh parser.tab.cc	
	flex++ --outfile=scanner.yy.cc  $<
	$(CXX)  $(CXXDEBUG) -c scanner.yy.cc -o scanner.o

clean:
	rm -f *.o
	rm -f *.output
	rm -f *.cc
	rm -f parser.tab.hh
	rm -f stack.hh
	rm -f position.hh
	rm -f location.hh
	rm -f iplC
	rm -f error.txt
	rm -f error1.txt
