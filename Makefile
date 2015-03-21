CXX = /usr/bin/g++
CXXFLAGS = -c -Wall -g -std=c++11 -O3
LDFLAGS = -lm -lcrypto -lssl
SOURCES = $(wildcard *.cpp)
OBJECTS = ${SOURCES:.cpp=.o}
TITLE = spritz
ARCHIVE = $(TITLE).tar.gz

.PHONY : all clean valgrind archive

all: $(TITLE)

$(TITLE) : $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS)

%.o : %.c 
	$(CXX) $(CXXFLAGS) -o $@ $< -MMD -MF ./$@.d

clean :
	rm -f -R *.o *.gch $(TITLE) *.d *.dSYM

valgrind : $(TITLE)
	valgrind --tool=memcheck --leak-check=yes ./$< $(P0) $(P1)

archive :
	tar cfz $(ARCHIVE) *.cpp *.h *.pdf $(TITLE)

-include $(wildcard ./*.d)
