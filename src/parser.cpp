#include "lexer/lexer.h"
#include "parser.h"
#include <unordered_map>
#include <stack>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>



void init_symbols_table(std::unordered_map<std::string, TOKEN>* SYMBOLS_TABLE);
int get_token_class_value(TOKEN token);
int get_reduce_rule_size(int reduce);
int get_reduce_rule_A(int reduce);
void print_grammar_rule(int reduce);
void generate_header(FILE *file);
void generate_tail(FILE *file);
void execute_semantic_rule(FILE *file, int rule, TOKEN token, 
        std::stack<TOKEN>& SEMANTIC_STACK, 
        std::unordered_map<std::string, TOKEN>& SYMBOLS_TABLE,
        std::stack<std::string>& print_buffer,
        std::queue<TOKEN>& TEMP_VAR_TO_PRINT,
        long file_pos);
TOKEN findLastType(std::stack<TOKEN>& SEMANTIC_STACK, std::string type);
bool is_real(std::string buffer);
void create_obj(bool has_error, std::queue<TOKEN>& TEMP_VAR_TO_PRINT);

std::set<std::string> SYNC_TOKENS = {"PT_V", "FC_P", "fim", "fimse", "fimrepita"};

std::vector<TOKEN> tokens_to_insert = {
    make_token("PT_V", 2), make_token("FC_P", 4), make_token("VIR", 0), make_palavra_reservada("varinicio"),
    make_palavra_reservada("inicio"), make_palavra_reservada("varfim"),
};

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
};

bool has_error = false;

int main(int argc, char* argv[]){
    std::unordered_map<std::string, TOKEN> SYMBOLS_TABLE;
    std::stack<int> PARSER_STACK;
    std::stack<TOKEN> SEMANTIC_STACK;
    std::stack<std::string> print_buffer;
    std::queue<TOKEN> TEMP_VAR_TO_PRINT;

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
    FILE *file_tmp;
    long file_pos;
    char ch;
    TOKEN token;
    TOKEN token_aux;
    std::string lexema;

    init_symbols_table(&SYMBOLS_TABLE);
    PARSER_STACK.push(0);
    SEMANTIC_STACK.push(make_palavra_reservada("-"));

    if(argc < 2) {
        printf("\n\nChamada sem argumento!!\n\n");
        exit(1);
    }
    file = fopen(argv[1], "r");
    file_tmp = fopen("./target/tmp.c", "w");

    generate_header(file_tmp);

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

        if(token.token_class != TOKEN_CLASS[8] && token.token_class != TOKEN_CLASS[12]) {
            stack_top = PARSER_STACK.top();
            token_class_value = get_token_class_value(token);

            NEXT_STATE = PARSER_TRANSITION_TABLE[stack_top][token_class_value];
             if (NEXT_STATE == -1) {
                printf("\nERRO!!!! - stack_top: %d - token_class_value: %d\ntoken: |%s| |%s| |%s| - linha: %d - coluna: %d\n\n",
                       stack_top, token_class_value, token.lexema.c_str(), token.token_class.c_str(), token.type.c_str(), token.linha, token.coluna);
                error_detected = true;
                has_error = true;

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
                        if (NEXT_STATE != -1) {
                            printf("Substituindo token: |%s| por |%s| para correção\n", token.lexema.c_str(), expected_token.lexema.c_str());
                            token = expected_token;
                            token_replaced = true;
                            break;
                        }
                    }
                }

                if (!token_replaced) {
                    bool token_fixed = false;
                    for (const auto& fix_token : tokens_to_insert) {
                        token_class_value = get_token_class_value(fix_token);
                        NEXT_STATE = PARSER_TRANSITION_TABLE[stack_top][token_class_value];
                        if (NEXT_STATE != -1) {
                            printf("Inserindo token: |%s| para correção\n", fix_token.lexema.c_str());
                            token = fix_token;
                            token_fixed = true;
                            break;
                        }
                    }

                    if (!token_fixed) {
                        while (true) {
                            if (SYNC_TOKENS.find(token.token_class) != SYNC_TOKENS.end() || token.token_class == TOKEN_CLASS[12]) {
                                if (token.token_class == TOKEN_CLASS[12]) {
                                    eof_detected = true;
                                    get_next_token = false;
                                }
                                break;
                            }
                            token = SCANNER(file);
                        }

                        if (eof_detected) {
                            break;
                        }

                        get_next_token = true;

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
                STATE = NEXT_STATE;
            }
            else if (NEXT_STATE < 200) {
                t = NEXT_STATE % 100;
                PARSER_STACK.push(t);
                SEMANTIC_STACK.push(token);
                STATE = t;
                get_next_token = true;

                continue;
            }
            else if (NEXT_STATE < 300) {
                reduce = NEXT_STATE % 200;

                rule_size = get_reduce_rule_size(reduce);
                rule_A = get_reduce_rule_A(reduce);

                for (int i = 0; i < rule_size; i++) {
                    PARSER_STACK.pop();
                }
                t = PARSER_STACK.top();
                
                PARSER_STACK.push(PARSER_TRANSITION_TABLE[t][rule_A]);

                print_grammar_rule(reduce);
                execute_semantic_rule(file_tmp, reduce, token, SEMANTIC_STACK, SYMBOLS_TABLE, print_buffer, TEMP_VAR_TO_PRINT, file_pos);

                get_next_token = false;
            }
            else {
                execute_semantic_rule(file_tmp, 0, token, SEMANTIC_STACK, SYMBOLS_TABLE, print_buffer, TEMP_VAR_TO_PRINT, file_pos);
                printf("\n\naccept!!!\n\n");
                break;
            }
        }
        else {
            get_next_token = true;
        }
        
    }

    generate_tail(file_tmp);
    fclose(file);
    fclose(file_tmp);
    create_obj(has_error, TEMP_VAR_TO_PRINT);
    std::remove("./target/tmp.c");

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
        printf("LV -> varfim pt_v\n");
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
        has_error = true;
        break;
    }
}

void generate_header(FILE *file) {
    long file_pos;
    fprintf(file, "#include<stdio.h>\n");
    fprintf(file, "typedef char literal[256];\n");
    fprintf(file, "void main(void) {\n");
}

void generate_tail(FILE *file){
    fprintf(file, "\n}\n");
}

void execute_semantic_rule(FILE *file, int rule, 
        TOKEN token,
        std::stack<TOKEN>& SEMANTIC_STACK,
        std::unordered_map<std::string, TOKEN>& SYMBOLS_TABLE,
        std::stack<std::string>& print_buffer,
        std::queue<TOKEN>& TEMP_VAR_TO_PRINT,
        long file_pos) {
    TOKEN stack_top_value;
    TOKEN last_type;
    TOKEN opm_to_print;
    static int temp_var_count = -1;

    switch (rule) {
    case 6:
        SEMANTIC_STACK.pop();
        last_type = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("D", "D", last_type.type.c_str()));
        while (!print_buffer.empty()) {
            std::string buffer = print_buffer.top();
            print_buffer.pop();
            fprintf(file, "%s", buffer.c_str());
        }
        
        break;
    case 7:
        last_type = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        stack_top_value = SEMANTIC_STACK.top();
        if (auto result_from_table = SYMBOLS_TABLE.find(stack_top_value.lexema); result_from_table != SYMBOLS_TABLE.end()) {
            result_from_table->second.type = last_type.type;
        }
        print_buffer.push(("\t"+last_type.type+" "+SEMANTIC_STACK.top().lexema+";\n"));
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("L", "L", last_type.type.c_str()));
        break;
    case 8:
        last_type = findLastType(SEMANTIC_STACK, "TIPO");
        stack_top_value = SEMANTIC_STACK.top();
        if (auto result_from_table = SYMBOLS_TABLE.find(stack_top_value.lexema); result_from_table != SYMBOLS_TABLE.end()) {
            result_from_table->second.type = last_type.type;
        }
        print_buffer.push(("\t"+last_type.type+" "+SEMANTIC_STACK.top().lexema+";\n"));
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("L", "L", last_type.type.c_str()));
        break;
    case 9:
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("TIPO", "TIPO", "int"));
        break;
    case 10:
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("TIPO", "TIPO", "double"));
        break;
    case 11:
        stack_top_value  = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("TIPO", "TIPO", "literal"));
        break;
    case 13:
        SEMANTIC_STACK.pop();
        stack_top_value  = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("ES", "ES", "ES"));
        if (auto result_from_table = SYMBOLS_TABLE.find(stack_top_value.lexema); result_from_table != SYMBOLS_TABLE.end()) {
            if(!result_from_table->second.type.compare("Nulo")) {
                printf("Erro: Variável não declarada - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                has_error = true;
            }
            else if (!result_from_table->second.type.compare("int")) {
                fprintf(file, "\tscanf(\"%%d\", &%s);\n", result_from_table->second.lexema.c_str());
            }
            else if (!result_from_table->second.type.compare("double")) {
                fprintf(file, "\tscanf(\"%%lf\", &%s);\n", result_from_table->second.lexema.c_str());
            }
            else if (!result_from_table->second.type.compare("literal")) {
                fprintf(file, "\tscanf(\"%%s\", %s);\n", result_from_table->second.lexema.c_str());
            }
        }
        break;
    case 14:
        SEMANTIC_STACK.pop();
        stack_top_value  = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("ES", "ES", "ES"));
        if (!stack_top_value.type.compare("literal")) {
            fprintf(file, "\tprintf(%s);\n", stack_top_value.lexema.c_str());
        }
        else {
            fprintf(file, "\tprintf(\"%s\");\n", stack_top_value.lexema.c_str());
        }
        
        break;
    case 15:
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type(stack_top_value.lexema, "ARG", stack_top_value.type.c_str()));
        break;
    case 16:
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type(stack_top_value.lexema, "ARG", stack_top_value.type.c_str()));
        break;
    case 17:
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        if (auto result_from_table = SYMBOLS_TABLE.find(stack_top_value.lexema); result_from_table != SYMBOLS_TABLE.end()) {
            if(!result_from_table->second.type.compare("Nulo")) {
                printf("Erro: Variável não declarada - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                has_error = true;
            }
            else  {
                SEMANTIC_STACK.push(make_token_with_type(result_from_table->second.lexema, "ARG", result_from_table->second.type.c_str()));
            }
        }
        break;
    case 19:
        SEMANTIC_STACK.pop();
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        last_type = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("CMD", "CMD", "CMD"));
        if (auto result_from_table = SYMBOLS_TABLE.find(last_type.lexema); result_from_table != SYMBOLS_TABLE.end()) {
            if(!result_from_table->second.type.compare("Nulo")) {
                printf("Erro: Variável não declarada - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                has_error = true;
            }
            else  {
                if (!stack_top_value.type.compare(result_from_table->second.type.c_str())) {
                    if(stack_top_value.type.compare("literal") == 0) {
                        printf("Erro: atribuicao de literal - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                        has_error = true;
                    }
                    else {
                        fprintf(file, "\t%s = %s;\n", result_from_table->second.lexema.c_str(), stack_top_value.lexema.c_str());
                    }
                }
                else {
                    printf("Erro: Tipos deiferentes para atribuicao - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                    has_error = true;
                }
            }
        }
        break;
    case 20:
        temp_var_count++;

        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        opm_to_print = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        last_type = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        if (!stack_top_value.token_class.compare("ID")) {
            if(auto result_from_table = SYMBOLS_TABLE.find(stack_top_value.lexema); result_from_table != SYMBOLS_TABLE.end()) {
                stack_top_value = result_from_table->second;
            }
            else {
                printf("Erro: Variável não declarada - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                has_error = true;
            }
        }
        if (!last_type.token_class.compare("ID")) {
            if(auto result_from_table = SYMBOLS_TABLE.find(last_type.lexema); result_from_table != SYMBOLS_TABLE.end()) {
                last_type = result_from_table->second;
            }
            else {
                printf("Erro: Variável não declarada - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                has_error = true;
            }
        }

        if (last_type.type.compare(stack_top_value.type.c_str())) {
            printf("Erro: Operandos com tipos incompatíveis - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
            has_error = true;
        }
        else {
            SEMANTIC_STACK.push(make_token_with_type("T" + std::to_string(temp_var_count), "TEMP_VAR", stack_top_value.type.c_str()));
            TEMP_VAR_TO_PRINT.push(make_token_with_type("T" + std::to_string(temp_var_count), "TEMP_VAR", stack_top_value.type.c_str()));
            fprintf(file, "\tT%d = %s %s %s;\n", temp_var_count, last_type.lexema.c_str(), opm_to_print.lexema.c_str(), stack_top_value.lexema.c_str());
        }
        break;
    case 21: 
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type(stack_top_value.lexema.c_str(), stack_top_value.token_class.c_str(), stack_top_value.type.c_str()));
        break;
    case 22:

        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        if (auto result_from_table = SYMBOLS_TABLE.find(stack_top_value.lexema); result_from_table != SYMBOLS_TABLE.end()) {
            if (!result_from_table->second.type.compare("Nulo")) {
                printf("Erro: Variável não declarada - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                has_error = true;
            }
            else {
                SEMANTIC_STACK.push(make_token_with_type(result_from_table->second.lexema.c_str(), 
                                                        result_from_table->second.token_class.c_str(), 
                                                        result_from_table->second.type.c_str()));
            }
        }
        break;
    case 23:
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        if(is_real(stack_top_value.lexema)) {
            SEMANTIC_STACK.push(make_token_with_type(stack_top_value.lexema.c_str(), stack_top_value.token_class.c_str(), "double"));
        }
        else {
            SEMANTIC_STACK.push(make_token_with_type(stack_top_value.lexema.c_str(), stack_top_value.token_class.c_str(), "int"));
        }
        break;
    case 25:
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("COND", "COND", "COND"));
        fprintf(file, "\t}\n");
        break;
    case 26:
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("CAB", "CAB", "CAB"));
        fprintf(file, "\tif (%s) {\n", stack_top_value.lexema.c_str());
        break;
    case 27:
        temp_var_count++;

        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        opm_to_print = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        last_type = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        if (!stack_top_value.token_class.compare("ID")) {
            if(auto result_from_table = SYMBOLS_TABLE.find(stack_top_value.lexema); result_from_table != SYMBOLS_TABLE.end()) {
                stack_top_value = result_from_table->second;
            }
            else {
                printf("Erro: Variável não declarada - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                has_error = true;
            }
        }
        if (!last_type.token_class.compare("ID")) {
            if(auto result_from_table = SYMBOLS_TABLE.find(last_type.lexema); result_from_table != SYMBOLS_TABLE.end()) {
                last_type = result_from_table->second;
            }
            else {
                printf("Erro: Variável não declarada - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
                has_error = true;
            }
        }

        if (last_type.type.compare(stack_top_value.type.c_str())) {
            printf("Erro:  Operandos com tipos incompatíveis - linha: %d, coluna: %d\n", stack_top_value.linha, stack_top_value.coluna);
            has_error = true;
        }
        else {
            SEMANTIC_STACK.push(make_token_with_type("T" + std::to_string(temp_var_count), "TEMP_VAR", stack_top_value.type.c_str()));
            TEMP_VAR_TO_PRINT.push(make_token_with_type("T" + std::to_string(temp_var_count), "TEMP_VAR", stack_top_value.type.c_str()));
            fprintf(file, "\tT%d = %s %s %s;\n", temp_var_count, last_type.lexema.c_str(), opm_to_print.lexema.c_str(), stack_top_value.lexema.c_str());
        }
        break;
    case 33:
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("R", "R", "R"));
        fprintf(file, "\t}\n");
        break;
    case 34:
        SEMANTIC_STACK.pop();
        stack_top_value = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.pop();
        SEMANTIC_STACK.push(make_token_with_type("CABR", "CABR", "CABR"));
        fprintf(file, "\twhile (%s) {\n", stack_top_value.lexema.c_str());
        break;
    default: 
        if(rule > 39){
            printf("ERRO SEMANTICO!!!!\n");
            has_error = true;
        }
        break;
    }
}

TOKEN findLastType(std::stack<TOKEN> &SEMANTIC_STACK, std::string type) {
    std::vector<TOKEN> tempVector;
    TOKEN lastToken;
    bool foundT = false;

    while (!SEMANTIC_STACK.empty())
    {
        TOKEN token = SEMANTIC_STACK.top();
        SEMANTIC_STACK.pop();
        tempVector.push_back(token);

        if (!token.token_class.compare(type)) {
            lastToken = token;
            foundT = true;
        }
    }

    for (auto it = tempVector.rbegin(); it != tempVector.rend(); ++it) {
        SEMANTIC_STACK.push(*it);
    }

    if (foundT) {
        return lastToken;
    }
    else {
        throw std::runtime_error("Nenhum token do tipo T encontrado.");
    }
}

bool is_real(std::string str) {
    return str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos;
}
    
void create_obj(bool has_error, std::queue<TOKEN>& TEMP_VAR_TO_PRINT) {
    if (has_error) {
        printf("\n\nOBJ NAO GERADO PORQUE CONTEM ERRO!!\n\n");
    }
    else {
        FILE *file_obj;
        FILE *file_tmp;

        std::ifstream filein("./target/tmp.c");
        std::ofstream fileout("./target/output.c");

        std::string str_tmp; 
        while (std::getline(filein, str_tmp)) {
            if(!str_tmp.compare("void main(void) {")) {
                fileout << str_tmp;
                fileout << "\n/*----Variaveis temporarias----*/\n";
                while (!TEMP_VAR_TO_PRINT.empty()) {
                    TOKEN token = TEMP_VAR_TO_PRINT.front();
                    TEMP_VAR_TO_PRINT.pop();
                    fileout << "\t";
                    fileout << token.type.c_str();
                    fileout << " ";
                    fileout << token.lexema.c_str();
                    fileout << ";\n";
                }
                fileout << "/*-----------------------------*/\n";
            }
            else {
                str_tmp += "\n";
                fileout << str_tmp;
            }
        }
    }
}