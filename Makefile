build: init lexer.o
	g++ -o target/lexer target/tmp/lexer.o

lexer.o: 
	g++ -o target/tmp/lexer.o src/lexer/lexer.cpp -c -W -Wall -ansi -pedantic

init:
	mkdir -p target/tmp

clean:
	rm -rf target

test_1: 
	./target/lexer testes/teste1.mgol

test_2:
	./target/lexer testes/teste2.mgol