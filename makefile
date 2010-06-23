TARGET = kspec

all: $(TARGET)

$(TARGET) : main.o kspec_parser.o kspec_scanner.o
	g++ -I$(HOME)/usr/include -o $@ $^ -lfl

kspec_parser.c : kspec_parser.y
	bison -o $@ --defines=kspec_parser.h kspec_parser.y

kspec_scanner.c : kspec_scanner.l
	flex -o $@ --header-file=kspec_scanner.h kspec_scanner.l

clean:
	@rm -f *.o
	@rm -f $(TARGET)

spotless: clean
	@rm -f kspec_parser.c kspec_parser.h
	@rm -f kspec_scanner.c kspec_scanner.h

.PHONY: clean spotless
	
