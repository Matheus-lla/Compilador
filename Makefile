build: clean init lexer.o
	g++ -o target/lexer target/tmp/lexer.o

lexer.o: 
	g++ -o target/tmp/lexer.o src/libs/implementations/lexer.cpp -c -W -Wall -ansi -pedantic

init:
	mkdir -p target/tmp

clean:
	rm -rf target

run:
	./target/lexer < testes/teste1.mgol