#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ctype.h>
#include <stdbool.h>

#include "lexer.h"

const char* TOKEN_CLASS[] = {
    "VIR",
    "PT_V",
    "AB_P",
    "FC_P",
    "OPM",
    "OPR",
    "RCB",
    "LIT",
    "COMENTARIO",
    "ID",
    "EOF_CLASS",
    "NUM",
    "ERRO"
};

TOKEN SCANNER(FILE *file){
    static int linha = 0;
    static int coluna = 0;
    
    TOKEN token;
    int STATE = 0;
    int SYMBOL;
    int NEXT_STATE;
    char ch;
    std::string buffer;
    int i = 0;

    while (!(feof(file))) {
        if ((ch = fgetc(file)) != EOF) {
            while ((' ' == ch || '\t' == ch || '\n' == ch || '\r' == ch || '\f' == ch || '\v' == ch) && STATE == 0) {
                ch = fgetc(file);
            }

            SYMBOL = get_symbol(ch);
            // if(SYMBOL != -2){
            NEXT_STATE = transition(STATE, SYMBOL);
            // } else NEXT_STATE = -2;

            printf("aqui tem o char: |%c| $ next state: %d\n", ch, NEXT_STATE);

            if(NEXT_STATE == -1){
                buffer += '\0';

                printf("buffer: |%s|\n", buffer.c_str());
                token = make_token((char*) buffer.c_str(), STATE);
                fseek(file, ftell(file)-1, SEEK_SET);
                
                printf("lexema: %s | class: %s | sizeof: %lu\n", token.lexema.c_str(), token.token_class, sizeof(token.lexema));

                return token;
                if (token.token_class != TOKEN_CLASS[12]){
                    // restore_pos = true;
                }
                STATE = 0;
                i = 0;
            }
            // else if (NEXT_STATE == -2){
            //     buffer[i++] = ch;
            //     buffer[i] = '\0';

            //     token = make_token(buffer, STATE);
            //     fseek(file, ftell(file)-1, SEEK_SET);
                
            //     return token;
            //     printf("lexema: %s | class: %s\n", token.lexema, token.token_class);

            //     STATE = 0;
            //     i = 0;
            // }
            else{
                buffer += ch;
                STATE = NEXT_STATE;
            }            

            if (ch == '\n'){
                linha++;
                coluna = 0;
            }
            coluna++;
        }
    }

    return token;
}

// int main(int argc,char *argv[]){
//     FILE *file;
//     long file_position;
//     char ch;
//     int STATE = 0;
//     int NEXT_STATE = 0;
//     int SYMBOL;
//     bool restore_pos = false;
//     char buffer[256] = {'\0'};
//     int i = 0;
//     int j = 0;
//     TOKEN token;


//     while((ch = fgetc(file)) != EOF and j < 1000){
//         if(restore_pos){
//             fseek(file, file_position, SEEK_SET);
//             ch = fgetc(file);
//             restore_pos = false;
            
//         }

//         j++;

//         while ((' ' == ch || '\t' == ch || '\n' == ch || '\r' == ch || '\f' == ch || '\v' == ch) && STATE == 0 && restore_pos != true) {
//             ch = fgetc(file);
//         }

//         file_position = ftell(file) - 1;
        
//         SYMBOL = get_symbol(ch);
//         if(SYMBOL != -2){
//             NEXT_STATE = transition(STATE, SYMBOL);
//         } else NEXT_STATE = -2;

//         // printf("aqui tem o char: |%c| $ next state: %d\n", ch, NEXT_STATE);

//         if(NEXT_STATE == -1){
//             buffer[i] = '\0';

//             token = make_token(buffer, STATE);
//             printf("lexema: %s | class: %s\n", token.lexema, token.token_class);

//             if (token.token_class != TOKEN_CLASS[12]){
//                 restore_pos = true;
//             }
//             STATE = 0;
//             i = 0;
//         }
//         else if (NEXT_STATE == -2){
//             buffer[i++] = ch;
//             buffer[i] = '\0';

//             token = make_token(buffer, STATE);
//             printf("lexema: %s | class: %s\n", token.lexema, token.token_class);

//             STATE = 0;
//             i = 0;
//         }
//         else{
//             buffer[i++] = ch;
//             STATE = NEXT_STATE;
//         }
        
//     }

//     fclose(file);
//     return 0;
// }

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

TOKEN make_token(char *buffer, int STATE){
    TOKEN token;
    
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