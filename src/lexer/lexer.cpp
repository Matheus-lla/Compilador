#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ctype.h>
#include <stdbool.h>

const int TRANSITION_TABLE[24][30] = {
/*    ,  ;  (  )  +  -  /  *  <  =  >  "  \  {  }  L  $  D  _  .  e  E \n  :  !  ?  [  ]  '   estado final 1 para aceitacão
      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28    29 */
    { 1, 2, 3, 4, 5, 5, 5, 5, 6, 8, 9,10,-1,13,-1,16,17,18,-1,-1,16,16, 0,-1,-1,-1,-1,-1,-1,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {10,10,10,10,10,10,10,10,10,10,10,12,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,    0},
    {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {13,13,13,13,13,13,13,13,13,13,13,13,15,13,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,16,-1,16,16,-1,16,16,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,19,21,21,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,21,21,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,23,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    0}
};

const int FINAL_STATE = 29;

char TOKEN_CLASS[13][11] = {
    {"VIR"},
    {"PT_V"},
    {"AB_P"},
    {"FC_P"},
    {"OPM"},
    {"OPR"},
    {"RCB"},
    {"LIT"},
    {"COMENTARIO"},
    {"ID"},
    {"EOF_CLASS"},
    {"NUM"},
    {"ERRO"}
};

typedef struct TOKEN {
    char *lexema;
    char *token_class;
} TOKEN;


bool is_letter(char ch);
bool is_number(char ch);
int transition(int STATE, int SYMBOL);
bool is_final(int STATE);
TOKEN make_last_token(char ch);
TOKEN make_token(char *buffer, int STATE, bool valid);
int get_symbol(char ch);

int main(int argc,char *argv[]){
    FILE *file;
    long file_position;
    char ch;
    int STATE = 0;
    int NEXT_STATE = 0;
    int SYMBOL;
    bool restore_pos = false;
    char buffer[50] = {'\0'};
    int i = 0;
    TOKEN token;

    if(argc < 2){
        printf("Parametros invalidos!");
        return 1;
    }

    file = fopen(argv[1], "r");

    while((ch = fgetc(file)) != '$' && ch != EOF){
        if(restore_pos){
            fseek(file, file_position, SEEK_SET);
            ch = fgetc(file);
            restore_pos = false;
        }

        while ((' ' == ch || '\t' == ch || '\n' == ch || '\r' == ch || '\f' == ch || '\v' == ch) && STATE == 0 && restore_pos != true) {
            ch = fgetc(file);
        }

        file_position = ftell(file) - 1;
        
        SYMBOL = get_symbol(ch);
        if(SYMBOL != -2){
            NEXT_STATE = transition(STATE, SYMBOL);
        } else NEXT_STATE = -2;

        if(NEXT_STATE == -1){
            restore_pos = true;

            buffer[i] = '\0';

            token = make_token(buffer, STATE, is_final(STATE));
            printf("lexema: %s | class: %s\n", token.lexema, token.token_class);

            STATE = 0;
            i = 0;
        }
        else if (NEXT_STATE == -2){
            buffer[i++] = ch;
            buffer[i] = '\0';

            token = make_token(buffer, STATE, false);
            printf("lexema: %s | class: %s\n", token.lexema, token.token_class);

            STATE = 0;
            i = 0;
        }
        else{
            buffer[i++] = ch;
            STATE = NEXT_STATE;
        }
        
    }

    token = make_last_token(ch);
    printf("lexema: %s | class: %s\n", token.lexema, token.token_class);

    fclose(file);
    return 0;
}

bool is_letter(char ch){
    char ch_upper = toupper(ch);
    return (ch_upper >= 'A' and ch_upper <= 'Z' and ch_upper != 'E') ? true : false;
}

bool is_number(char ch){
    return (ch >= '0' and ch <= '9') ? true : false;
}

int transition(int STATE, int SYMBOL){
    return TRANSITION_TABLE[STATE][SYMBOL];
}

bool is_final(int STATE){
    return (TRANSITION_TABLE[STATE][FINAL_STATE]) ? true : false;
}

TOKEN make_last_token(char ch){
    char buffer[2];
    int STATE; 
    
    if (ch == '$') {
        buffer[0] = ch;
        buffer[1] = '\0';
        STATE = 17; 
    }
    else {
        buffer[0] = ch;
        buffer[1] = '\0';
        STATE = false;
    }
    
   return make_token(buffer, STATE, is_final(STATE));
}

TOKEN make_token(char *buffer, int STATE, bool valid){
    TOKEN token;
    
    if(!valid){
        token.lexema = buffer;
        token.token_class = TOKEN_CLASS[12];
        return token;
    }
    else{
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
        case 12:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[7];
            return token;
            break;
        case 14:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[8];
            return token;
            break;
        case 16:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[9];
            return token;
            break;
        case 17:
            token.lexema = buffer;
            token.token_class = TOKEN_CLASS[10];
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
        case 22:
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
            return -2;
        }
    }
}