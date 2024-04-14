#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ctype.h>
#include <stdbool.h>

const int TRANSITION_TABLE[24][24] = {
/*    ,  ;  (  )  +  -  /  *  <  =  >  "  \  {  }  L  $  D  _  .  e  E \n   estado final 1 para aceitacão
      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22   23 */
    { 1, 2, 3, 4, 5, 5, 5, 5, 6, 8, 9,10,-1,13,-1,16,17,18,-1,-1,16,16, 0,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {10,10,10,10,10,10,10,10,10,10,10,12,11,10,10,10,10,10,10,10,10,10,-1,    0},
    {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,-1,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {13,13,13,13,13,13,13,13,13,13,13,13,15,13,14,13,13,13,13,13,13,13,-1,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,-1,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,16,-1,16,16,-1,16,16,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,19,21,21,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,-1,-1,-1,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,21,21,-1,    1},
    {-1,-1,-1,-1,23,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,    0},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,    1},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,    0}
};

const int FINAL_STATE = 23;

const char TOKEN_CLASS[12][11] = {
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
    {"NUM"}
};

typedef struct TOKEN {
    char *lexema;
    char token_class[11];
} TOKEN;


bool is_letter(char ch);
bool is_number(char ch);
int transition(int STATE, int SYMBOL);
bool is_final(int STATE);
void make_token(char *buffer, int STATE, bool valid);
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
    int cont = 0;

    if(argc < 2){
        printf("Parametros invalidos!");
        return 1;
    }

    file = fopen(argv[1], "r");

    while((ch = fgetc(file)) != EOF){
        if(restore_pos){
            fseek(file, file_position, SEEK_SET);
            ch = fgetc(file);
            restore_pos = false;
        }

        while ((' ' == ch || '\t' == ch || '\n' == ch || '\r' == ch || '\f' == ch || '\v' == ch) && STATE == 0 && restore_pos != true) {
            ch = fgetc(file);
        }

        if(ch == '\\' and cont < 10){
            cont++;
            if (cont == 9)
            {
                break;
            }
            
        }
        
        file_position = ftell(file) - 1;
        
        SYMBOL = get_symbol(ch);
        if(SYMBOL != -2){
            NEXT_STATE = transition(STATE, SYMBOL);
        } else NEXT_STATE = -2;

        if(NEXT_STATE == -1){
            restore_pos = true;

            buffer[i] = '\0';

            // printf("\ntoken -1 & ch |%c|\n", ch);
            make_token(buffer, STATE, is_final(STATE));
            STATE = 0;
            i = 0;
        }
        else if (NEXT_STATE == -2){
            buffer[i++] = ch;
            buffer[i] = '\0';

            // printf("\ntoken -2 & ch |%d| & state: %d\n", ch, STATE);
            make_token(buffer, STATE, false);
            STATE = 0;
            i = 0;
        }
        else{
            buffer[i++] = ch;
            STATE = NEXT_STATE;
        }
        
        // printf("character: |%c| || state: %d\n", ch, STATE);
    }

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

void make_token(char *buffer, int STATE, bool valid){
    if(!valid){
        printf("token: ERRO & |%s|\n", buffer);
    }
    else{
        switch (STATE){
        case 1:
            printf("token: VIR & |%s|\n", buffer);
            break;
        case 2:
            printf("token: PT_V & |%s|\n", buffer);
            break;
        case 3:
            printf("token: AB_P & |%s|\n", buffer);
            break;
        case 4:
            printf("token: FC_P & |%s|\n", buffer);
            break;
        case 5:
            printf("token: OPM & |%s|\n", buffer);
            break;
        case 6:
            printf("token: OPR & |%s|\n", buffer);
            break;
        case 7:
            printf("token: RCB & |%s|\n", buffer);
            break;
        case 8:
            printf("token: OPR & |%s|\n", buffer);
            break;
        case 9:
            printf("token: OPR & |%s|\n", buffer);
            break;
        case 12:
            printf("token: Lit & |%s|\n", buffer);
            break;
        case 14:
            printf("token: Comentario & |%s|\n", buffer);
            break;
        case 16:
            printf("token: ID & |%s|\n", buffer);
            break;
        case 17:
            printf("token: EOF & |%s|\n", buffer);
            break;
        case 18:
            printf("token: Num & |%s|\n", buffer);
            break;
        case 20:
            printf("token: Num & |%s|\n", buffer);
            break;
        case 22:
            printf("token: Num & |%s|\n", buffer);
            break;
        default:
            printf("token: ERRO & |%s|\n", buffer);
            break;
        }   
    }
   
}

    // {"VIR"},
    // {"PT_V"},
    // {"AB_P"},
    // {"FC_P"},
    // {"OPM"},
    // {"OPR"},
    // {"RCB"},
    // {"LIT"},
    // {"COMENTARIO"},
    // {"ID"},
    // {"EOF_CLASS"},
    // {"NUM"}
    
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
            return 15;
        case 'E':
            return 15;
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
        default:
            return -2;
        }
    }
}