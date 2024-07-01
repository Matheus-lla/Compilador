build: clean init 
	g++ -o target/parser src/parser.cpp src/lexer/lexer.cpp

init:
	mkdir -p target/tmp

clean:
	rm -rf target

test.all: test_comentario test_id test_lit test_num test_professora 

test.comentario:
	./target/parser testes/teste_comentario.mgol

test.descricaoT1:
	./target/parser testes/teste_descricaoT1.mgol

test.id:
	./target/parser testes/teste_id.mgol

test.lit:
	./target/parser testes/teste_lit.mgol

test.num:
	./target/parser testes/teste_num.mgol

test.professora: 
	./target/parser testes/teste_professora.mgol