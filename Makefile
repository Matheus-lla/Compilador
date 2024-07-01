build: clean init 
	g++ -o target/parser src/parser.cpp src/lexer/lexer.cpp

init:
	mkdir -p target/tmp

clean:
	rm -rf target

test.all: test.comentario test.id test.lit test.num test.professora 

test.comentario: build
	./target/parser testes/teste_comentario.mgol

test.descricaoT1: build
	./target/parser testes/teste_descricaoT1.mgol

test.id: build
	./target/parser testes/teste_id.mgol

test.lit: build
	./target/parser testes/teste_lit.mgol

test.num: build
	./target/parser testes/teste_num.mgol

test.professora: build 
	./target/parser testes/teste_professora.mgol