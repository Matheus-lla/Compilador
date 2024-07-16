#include "lexer/lexer.h"
#include "parser.h"
#include <unordered_map>
#include <stack>
#include <set>
#include <iostream>
#include <vector>


void init_symbols_table(std::unordered_map<std::string, TOKEN>* SYMBOLS_TABLE);
int get_token_class_value(TOKEN token);
int get_reduce_rule_size(int reduce);
int get_reduce_rule_A(int reduce);
void print_grammar_rule(int reduce);

std::set<std::string> SYNC_TOKENS = {"PT_V", "FC_P", "fim", "fimse", "fimrepita"};

std::vector<TOKEN> tokens_to_insert = {
    make_token("PT_V", 2), make_token("FC_P", 4), make_token("VIR", 0), make_palavra_reservada("varinicio"),
    make_palavra_reservada("inicio"), make_palavra_reservada("varfim"),
};

// Conjunto de tokens esperados para substituição, conforme contexto do analisador
std::unordered_map<int, std::vector<TOKEN>> EXPECTED_TOKENS = {
    {0, {make_palavra_reservada("inicio")}},
    {2, {make_palavra_reservada("varinicio")}},
    {4, {make_palavra_reservada("varfim")}},
    {24, {make_token("PT_V", 2)}},
    {26, {make_token("PT_V", 2)}},
    {27, {make_token("PT_V", 2)}},
    {28, {make_token("PT_V", 2)}},
    {29, {make_token("PT_V", 2)}},
    {30, {make_token("PT_V", 2)}}
    // Adicione mais estados e tokens esperados conforme a gramática
};


int main(int argc, char* argv[]){
    std::unordered_map<std::string, TOKEN> SYMBOLS_TABLE;
    std::stack<int> PARSER_STACK;

    int STATE = 0;
    int NEXT_STATE;
    int stack_top;
    int token_class_value;
    int action;
    int t;
    int reduce;
    int rule_size;
    int rule_A;

    bool get_next_token = true;
    bool error_detected = false;
    bool eof_detected = false;

    FILE *file;
    char ch;
    TOKEN token;
    TOKEN token_aux;
    std::string lexema;

    init_symbols_table(&SYMBOLS_TABLE);
    PARSER_STACK.push(0);

    if(argc < 2) {
        printf("\n\nChamada sem argumento!!\n\n");
        exit(1);
    }
    file = fopen(argv[1], "r");


    while (true){
        if(get_next_token) {
            token = SCANNER(file);
            get_next_token = false;
        }

        if (token.token_class == TOKEN_CLASS[9]){
            if (auto result_from_table = SYMBOLS_TABLE.find(token.lexema); result_from_table == SYMBOLS_TABLE.end()){
                SYMBOLS_TABLE.insert({token.lexema, token});
            }
            else{
                if (result_from_table->second.lexema == result_from_table->second.token_class 
                    and result_from_table->second.token_class == result_from_table->second.type){
                        token = result_from_table->second;
                }
            }
        }


        // printf("\nAQUI\ntoken: |%s| |%s| |%s|\n", token.lexema.c_str(), token.token_class.c_str(), token.type.c_str());
        if (token.token_class == TOKEN_CLASS[12])
        {
            break;
        }

        if(eof_detected){
            break;
        }
        
        if(token.token_class != TOKEN_CLASS[8] && token.token_class != TOKEN_CLASS[12]){
            stack_top = PARSER_STACK.top();
            token_class_value = get_token_class_value(token);

            NEXT_STATE = PARSER_TRANSITION_TABLE[stack_top][token_class_value];
            // printf("\nAQUI\ntoken: |%s| |%s| |%s|\nstack_top: %d\nnext_state: %d\nclass_value: %d\n", token.lexema.c_str(), token.token_class.c_str(), token.type.c_str(), stack_top, NEXT_STATE, token_class_value);
             if (NEXT_STATE == -1) {
                printf("\nERRO!!!! - stack_top: %d - token_class_value: %d\ntoken: |%s| |%s| |%s| - linha: %d - coluna: %d\n\n",
                       stack_top, token_class_value, token.lexema.c_str(), token.token_class.c_str(), token.type.c_str(), token.linha, token.coluna);
                error_detected = true;

                // Tentar substituir o token por um esperado
                bool token_replaced = false;
                if (EXPECTED_TOKENS.find(stack_top) != EXPECTED_TOKENS.end()) {
                    for (const auto& expected_token : EXPECTED_TOKENS[stack_top]) {
                        token_class_value = get_token_class_value(expected_token);
                        if(token_class_value == 3){
                            if(get_token_class_value(token) != 5 && get_token_class_value(token) != 17){
                                break;
                            }
                        }
                        NEXT_STATE = PARSER_TRANSITION_TABLE[stack_top][token_class_value];
                        printf("topo: %d - token: %s\n", stack_top, token.lexema.c_str());
                        if (NEXT_STATE != -1) {
                            printf("Substituindo token: |%s| por |%s| para correção\n", token.lexema.c_str(), expected_token.lexema.c_str());
                            token = expected_token;
                            token_replaced = true;
                            break;
                        }
                    }
                }

                if (!token_replaced) {
                    // Tentar inserir tokens
                    bool token_fixed = false;
                    for (const auto& fix_token : tokens_to_insert) {
                        token_class_value = get_token_class_value(fix_token);
                        NEXT_STATE = PARSER_TRANSITION_TABLE[stack_top][token_class_value];
                        printf("topo rep - %d - token: %s\n", stack_top, token.lexema.c_str());
                        if (NEXT_STATE != -1) {
                            printf("Inserindo token: |%s| para correção\n", fix_token.lexema.c_str());
                            token = fix_token;
                            token_fixed = true;
                            break;
                        }
                    }

                    if (!token_fixed) {
                        // Modo pânico: consome tokens até encontrar um token de sincronização
                        while (true) {
                            if (SYNC_TOKENS.find(token.token_class) != SYNC_TOKENS.end() || token.token_class == TOKEN_CLASS[12]) {
                                if (token.token_class == TOKEN_CLASS[12]) {
                                    eof_detected = true;
                                    get_next_token = false;
                                }
                                break;
                            }
                            token = SCANNER(file);
                            printf("Valor do token aqui dentro: %s\n", token.lexema.c_str());
                        }

                        if (eof_detected) {
                            break;
                        }

                        get_next_token = true;

                        // Desempilha até encontrar um estado que permita continuar
                        while (!PARSER_STACK.empty() && NEXT_STATE == -1) {
                            PARSER_STACK.pop();
                            if (!PARSER_STACK.empty()) {
                                stack_top = PARSER_STACK.top();
                                token_class_value = get_token_class_value(token);
                                NEXT_STATE = PARSER_TRANSITION_TABLE[stack_top][token_class_value];
                                if(NEXT_STATE != -1){
                                    get_next_token = false;
                                }
                            }
                        }

                        if (PARSER_STACK.empty()) {
                            printf("\nErro irrecuperável.\n");
                            break;
                        }

                        continue;
                    }
                }
            }
            else if (NEXT_STATE >= 0 && NEXT_STATE < 100) {
                printf("vim next1");
                // printf("\nGOTO\ntoken: |%s| |%s| |%s|\nstack_top: %d\nnext_state: %d\n", token.lexema.c_str(), token.token_class.c_str(), token.type.c_str(), stack_top, NEXT_STATE);
                STATE = NEXT_STATE;
            }
            else if (NEXT_STATE < 200) {
            printf("vim next2");
                t = NEXT_STATE % 100;
                // printf("\nSHIFT\ntoken: |%s| |%s| |%s|\nstack_top: %d\nt: %d\n", token.lexema.c_str(), token.token_class.c_str(), token.type.c_str(), stack_top, t);
                PARSER_STACK.push(t);
                STATE = t;
                get_next_token = true;

                // printf("shift t: %3d\n", t);
                continue;
            }
            else if (NEXT_STATE < 300) {
                printf("vim next3");
                // printf("\nREDUCE\ntoken: |%s| |%s| |%s|\nstack_top: %d\nnext_state: %d\n", token.lexema.c_str(), token.token_class.c_str(), token.type.c_str(), stack_top, NEXT_STATE);
                reduce = NEXT_STATE % 200;

                rule_size = get_reduce_rule_size(reduce);
                rule_A = get_reduce_rule_A(reduce);
                // printf("reduce: %3d\n", reduce);

                // printf("STACK\n");
                // for (std::stack<int> dump = PARSER_STACK; !dump.empty(); dump.pop())
                //     printf("%d ", dump.top());
                // printf("\n");

                for (int i = 0; i < rule_size; i++) {
                    PARSER_STACK.pop();
                }
                t = PARSER_STACK.top();
                
                PARSER_STACK.push(PARSER_TRANSITION_TABLE[t][rule_A]);

                // printf("after pop t: %3d - rule_A: %3d - ", t, rule_A);
                print_grammar_rule(reduce);
                // printf("t: %3d\n",PARSER_STACK.top());
                get_next_token = false;
            }
            else {
                printf("\n\naccept!!!\n\n");
                break;
            }
        }
    }

    return 0;
}

void init_symbols_table(std::unordered_map<std::string, TOKEN>* SYMBOLS_TABLE){
    SYMBOLS_TABLE->insert({"inicio", make_palavra_reservada("inicio")});
    SYMBOLS_TABLE->insert({"varinicio", make_palavra_reservada("varinicio")});
    SYMBOLS_TABLE->insert({"varfim", make_palavra_reservada("varfim")});
    SYMBOLS_TABLE->insert({"escreva", make_palavra_reservada("escreva")});
    SYMBOLS_TABLE->insert({"leia", make_palavra_reservada("leia")});
    SYMBOLS_TABLE->insert({"se", make_palavra_reservada("se")});
    SYMBOLS_TABLE->insert({"entao", make_palavra_reservada("entao")});
    SYMBOLS_TABLE->insert({"fimse", make_palavra_reservada("fimse")});
    SYMBOLS_TABLE->insert({"repita", make_palavra_reservada("repita")});
    SYMBOLS_TABLE->insert({"fimrepita", make_palavra_reservada("fimrepita")});
    SYMBOLS_TABLE->insert({"fim", make_palavra_reservada("fim")});
    SYMBOLS_TABLE->insert({"inteiro", make_palavra_reservada("inteiro")});
    SYMBOLS_TABLE->insert({"literal", make_palavra_reservada("literal")});
    SYMBOLS_TABLE->insert({"real", make_palavra_reservada("real")});
}

int get_token_class_value(TOKEN token){
    if(token.token_class.compare("inicio") == 0) {
        return 0;
    }
    else if (token.token_class.compare("varinicio") == 0) {
        return 1;
    }
    else if (token.token_class.compare("varfim") == 0) {
        return 2;
    }
    else if (token.token_class.compare("PT_V") == 0) {
        return 3;
    }
    else if (token.token_class.compare("ID") == 0) {
        return 4;
    }
    else if (token.token_class.compare("VIR") == 0) {
        return 5;
    }
    else if (token.token_class.compare("inteiro") == 0) {
        return 6;
    }
    else if (token.token_class.compare("real") == 0) {
        return 7;
    }
    else if (token.token_class.compare("literal") == 0) {
        return 8;
    }
    else if (token.token_class.compare("leia") == 0) {
        return 9;
    }
    else if (token.token_class.compare("escreva") == 0) {
        return 10;
    }
    else if (token.token_class.compare("LIT") == 0) {
        return 11;
    }
    else if (token.token_class.compare("NUM") == 0) {
        return 12;
    }
    else if (token.token_class.compare("RCB") == 0) {
        return 13;
    }
    else if (token.token_class.compare("OPM") == 0) {
        return 14;
    }
    else if (token.token_class.compare("se") == 0) {
        return 15;
    }
    else if (token.token_class.compare("AB_P") == 0) {
        return 16;
    }
    else if (token.token_class.compare("FC_P") == 0) {
        return 17;
    }
    else if (token.token_class.compare("entao") == 0) {
        return 18;
    }
    else if (token.token_class.compare("OPR") == 0) {
        return 19;
    }
    else if (token.token_class.compare("fimse") == 0) {
        return 20;
    }
    else if (token.token_class.compare("repita") == 0) {
        return 21;
    }
    else if (token.token_class.compare("fimrepita") == 0) {
        return 22;
    }
    else if (token.token_class.compare("fim") == 0) {
        return 23;
    }
    else if (token.token_class.compare("EOF_CLASS") == 0) {
        return 24;
    }
    else {
        return -1;
    }
}

int get_reduce_rule_size(int reduce) {
    if (reduce == 1  ||
        reduce == 8  ||
        reduce == 9  ||
        reduce == 10 ||
        reduce == 11 ||
        reduce == 15 ||
        reduce == 16 ||
        reduce == 17 ||
        reduce == 21 ||
        reduce == 22 ||
        reduce == 23 ||
        reduce == 31 ||
        reduce == 38 ||
        reduce == 39 
        ) {
            return 1;
    }
    else if(reduce == 3  ||
            reduce == 4  ||
            reduce == 5  ||
            reduce == 12 ||
            reduce == 18 ||
            reduce == 24 ||
            reduce == 25 ||
            reduce == 28 ||
            reduce == 29 ||
            reduce == 30 ||
            reduce == 32 ||
            reduce == 33 ||
            reduce == 35 ||
            reduce == 36 ||
            reduce == 37
        ) {
            return 2;
    }
    else if(reduce == 2  ||
            reduce == 6  ||
            reduce == 7  ||
            reduce == 13 ||
            reduce == 14 ||
            reduce == 20 ||
            reduce == 27
        ) {
            return 3;
    }
    else if(reduce == 19 ||
            reduce == 34
        ) {
            return 4;
    }
    else {
        return 5;
    }
}

int get_reduce_rule_A(int reduce) {
    if (reduce == 2) {
        return 25;
    }
    else if(reduce == 3) {
        return 26;
    }
    else if(reduce == 12 ||
            reduce == 18 ||
            reduce == 24 ||
            reduce == 32 ||
            reduce == 39
        ) {
        return 27;
    }
    else if(reduce == 4  ||
            reduce == 5 
        ) {
        return 28;
    }
    else if(reduce == 6) {
        return 29;
    }
    else if(reduce == 9  ||
            reduce == 10 || 
            reduce == 11
        ) {
        return 30;
    }
    else if(reduce == 7  ||
            reduce == 8 
        ) {
        return 31;
    }
    else if(reduce == 13 ||
            reduce == 14 
        ) {
        return 32;
    }
    else if(reduce == 15 ||
            reduce == 16 || 
            reduce == 17 
        ) {
        return 33;
    }
    else if(reduce == 19) {
        return 34;
    }
    else if(reduce == 20 ||
            reduce == 21 
        ) {
        return 35;
    }
    else if(reduce == 22 ||
            reduce == 23 
        ) {
        return 36;
    }
    else if(reduce == 25) {
        return 37;
    }
    else if(reduce == 26) {
        return 38;
    }
    else if(reduce == 28 ||
            reduce == 29 ||
            reduce == 30 ||
            reduce == 31 
        ) {
        return 39;
    }
    else if(reduce == 27) {
        return 40;
    }
    else if(reduce == 33) {
        return 41;
    }
    else if(reduce == 34) {
        return 42;
    }
    else if(reduce == 35 ||
            reduce == 36 ||
            reduce == 37 ||
            reduce == 38
        ) {
        return 43;
    }
    else {
        return -1;
    }
}

void print_grammar_rule(int reduce) {
    switch (reduce) {
    case 1:
        printf("P' -> P\n");
        break;
    case 2:
        printf("P -> inicio V A\n");
        break;
    case 3:
        printf("V -> varinicio LV\n");
        break;
    case 4:
        printf("LV -> D LV\n");
        break;
    case 5:
        printf("  -> varfim pt_v\n");
        break;
    case 6:
        printf("D -> TIPO L pt_v\n");
        break;
    case 7:
        printf("L -> id vir L\n");
        break;
    case 8:
        printf("L -> id\n");
        break;
    case 9:
        printf("TIPO -> inteiro\n");
        break;
    case 10:
        printf("TIPO -> real\n");
        break;
    case 11:
        printf("TIPO -> literal\n");
        break;
    case 12:
        printf("A -> ES A\n");
        break;
    case 13:
        printf("ES -> leia id pt_v\n");
        break;
    case 14:
        printf("ES -> escreva ARG pt_v\n");
        break;
    case 15:
        printf("ARG -> lit\n");
        break;
    case 16:
        printf("ARG -> num\n");
        break;
    case 17:
        printf("ARG -> id\n");
        break;
    case 18:
        printf("A -> CMD A\n");
        break;
    case 19:
        printf("CMD -> id atr LD pt_v\n");
        break;
    case 20:
        printf("LD -> OPRD opm OPRD\n");
        break;
    case 21:
        printf("LD -> OPRD\n");
        break;
    case 22:
        printf("OPRD -> id\n");
        break;
    case 23:
        printf("OPRD -> num\n");
        break;
    case 24:
        printf("A -> COND A\n");
        break;
    case 25:
        printf("COND -> CAB CP\n");
        break;
    case 26:
        printf("CAB -> se ab_p EXP_R fc_p entao\n");
        break;
    case 27:
        printf("EXP_R -> OPRD opr OPRD\n");
        break;
    case 28:
        printf("CP -> ES CP\n");
        break;
    case 29:
        printf("CP -> CMD CP\n");
        break;
    case 30:
        printf("CP -> COND CP\n");
        break;
    case 31:
        printf("CP -> fimse\n");
        break;
    case 32:
        printf("A -> RA\n");
        break;
    case 33:
        printf("R -> CABR CPR\n");
        break;
    case 34:
        printf("CABR -> repita ab_p EXP_R fc_p\n");
        break;
    case 35:
        printf("CPR -> ES CPR\n");
        break;
    case 36:
        printf("CPR -> CMD CPR\n");
        break;
    case 37:
        printf("CPR -> COND CPR\n");
        break;
    case 38:
        printf("CPR -> fimrepita\n");
        break;
    case 39:
        printf("A -> fim\n");
        break;
    default:
        printf("ERRO SINTATICO!!!!\n");
        break;
    }
}