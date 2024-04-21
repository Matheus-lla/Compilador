build: clean init 
	g++ -o target/main src/main.cpp src/lexer/lexer.cpp

init:
	if not exist "target" mkdir target
	if not exist "target\tmp" mkdir target\tmp

clean:
	if exist "target" rd /s /q target

test.all: test_comentario test_id test_lit test_num test_professora 

test.comentario:
	./target/main testes/teste_comentario.mgol

test.id:
	./target/main testes/teste_id.mgol

test.lit:
	./target/main testes/teste_lit.mgol

test.num:
	./target/main testes/teste_num.mgol

test.professora: 
	./target/main testes/teste_professora.mgol