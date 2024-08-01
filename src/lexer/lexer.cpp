#include "lexer.h"

TOKEN SCANNER(FILE *file){
    int STATE = 0;
    int SYMBOL;
    int NEXT_STATE;
    TOKEN token;
    
    char ch;
    std::string buffer;

    static int linha = 1;
    static int coluna = 1;

    while (!(feof(file))) {
        if ((ch = fgetc(file)) != EOF) {
            ch = skip_ws(ch, STATE, file, &linha, &coluna);

            SYMBOL = get_symbol(ch);
            NEXT_STATE = transition(STATE, SYMBOL);

            if(NEXT_STATE == -1){
                buffer += '\0';
                fseek(file, ftell(file)-1, SEEK_SET);
                coluna--;
                break;
            }
            else{ 
                if (NEXT_STATE == -2){
                    buffer += ch;
                    buffer += '\0';
                    break;
                }
                else{
                    buffer += ch;
                    STATE = NEXT_STATE;
                }            
            }

            coluna++;
        }
        else {
            
        }
        
    }
    token = make_token((char*) buffer.c_str(), STATE); 
    
    token.linha = linha;
    token.coluna = coluna - 1;

    if(feof(file) && STATE == 10) {
        printf("ERRO LEXICO - Comentario nao fechado, linha: %d, coluna: %d\n", linha, coluna + 2);
    } else {
        if (feof(file) && token.lexema.compare("") == 0) {
            token = make_token((char*) "$", 15);
        }
        else if(token.token_class == TOKEN_CLASS[12]) {
            printf("ERRO LEXICO - Caracter invalido na linguagem |%s|, linha: %d, coluna: %d\n", token.lexema.c_str(), linha, coluna + 2);
        }
    }

    
    
    return token;
}

char skip_ws(char ch, int STATE, FILE *file, int *linha, int *coluna){
    while ((' ' == ch || '\t' == ch || '\n' == ch || '\r' == ch || '\f' == ch || '\v' == ch) && STATE == 0) {
        if(ch == '\n'){
            (*linha) ++;
            (*coluna) = 1;
        }
        (*coluna)++;
        ch = fgetc(file);
    }
    
    return ch;
}

int get_symbol(char ch){
    if(is_number(ch)){
        return 17;
    }else if(is_letter(ch)){
        return 15;
    }else {
        switch (ch){
        case ',':
            return 0;
        case ';':
            return 1;
        case '(':
            return 2;
        case ')':
            return 3;
        case '+':
            return 4;
        case '-':
            return 5;
        case '/':
            return 6;
        case '*':
            return 7;
        case '<':
            return 8;
        case '=':
            return 9;
        case '>':
            return 10;
        case '"':
            return 11;
        case '\\':
            return 12;
        case '{':
            return 13;
        case '}':
            return 14;
        case '$':
            return 16;
        case '_':
            return 18;
        case '.':
            return 19;
        case 'e':
            return 20;
        case 'E':
            return 21;
        case '\n':
            return 22;
        case '\r':
            return 22;
        case ' ':
            return 22;
        case '\t':
            return 22;
        case '\v':
            return 22;
        case '\f':
            return 22;
        case ':':           
            return 23;
        case '!':
            return 24;
        case '?':
            return 25;
        case '[':
            return 26;
        case ']':
            return 28;
        case '\'':
            return 28;
        default:
            return -1;
        }
    }
}

bool is_letter(char ch){
    char ch_upper = toupper(ch);
    return (ch_upper >= 'A' and ch_upper <= 'Z' and ch_upper != 'E') ? true : false;
}

bool is_number(char ch){
    return (ch >= '0' and ch <= '9') ? true : false;
}

int transition(int STATE, int SYMBOL){
    if (SYMBOL == -1) {
        return (STATE == 0) ? -2: -1;
    }
    return TRANSITION_TABLE[STATE][SYMBOL];
}

TOKEN make_token(std::string buffer, int STATE){
    TOKEN token;
    token.type = "Nulo";

    switch (STATE){
        case 1:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[0];
            return token;
            break;
        case 2:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[1];
            return token;
            break;
        case 3:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[2];
            return token;
            break;
        case 4:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[3];
            return token;
            break;
        case 5:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[4];
            return token;
            break;
        case 6:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[5];
            return token;
            break;
        case 7:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[6];
            return token;
            break;
        case 8:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[5];
            return token;
            break;
        case 9:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[5];
            return token;
            break;
        case 11:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[7];
            token.type = "literal";
            return token;
            break;
        case 13:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[8];
            return token;
            break;
        case 14:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[9];
            return token;
            break;
        case 15:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[10];
            return token;
            break;
        case 16:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[11];
            return token;
            break;
        case 18:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[11];
            return token;
            break;
        case 20:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[11];
            return token;
            break;
        default:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[12];
            return token;
            break;
    }   
}

TOKEN make_token_with_type(std::string buffer, std::string token_class, std::string type) {
    TOKEN token;

    token.lexema = buffer;
    token.token_class = token_class;
    token.type = type;

    return token;
}

TOKEN make_palavra_reservada(std::string buffer){
    TOKEN token;

    token.lexema = buffer;
    token.token_class = buffer;
    token.type = buffer;

    return token;
}