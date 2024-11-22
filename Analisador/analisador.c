#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char lexema[100];
    int tipo;
    int linha;
    int coluna;
} Token;

#define ID 1
#define NUM_INT 2
#define NUM_FLOAT 3
#define TAMANHO_TABELA_SIMBOLOS 100
#define MAX_TOKENS 1000  


typedef enum {
    TOKEN_ID,
    TOKEN_PAL_RES,
    TOKEN_NUM_INT,
    TOKEN_NUM_FLOAT,
    TOKEN_OP_NE,
    TOKEN_OP_LE,
    TOKEN_OP_LT,
    TOKEN_OP_GE,
    TOKEN_OP_GT,
    TOKEN_OP_ATRIB,
    TOKEN_OP_ASS,
    TOKEN_OP_DIV,
    TOKEN_OP_MULT,
    TOKEN_OP_AD,
    TOKEN_OP_MIN,
    TOKEN_SMB_PONTO,
    TOKEN_SMB_COM,
    TOKEN_SMB_SEM,
    TOKEN_SMB_OPA,
    TOKEN_SMB_CPA
} TipoToken;

typedef struct {
    char nome[20];
    char lexema[100];
    int linha;
    int coluna;
    int indice;
} Symbol;

Symbol tabela_simbolos[TAMANHO_TABELA_SIMBOLOS];
int simbolo_count = 0;

FILE *saida_lex, *tabela_simbolos_file, *erros_file, *saida_sintatico;

void reporta_erro(int linha, int coluna, const char *mensagem, const char *valor);
void parse_programa(Token *tokens, int *pos);
void parse_comandos(Token *tokens, int *pos);
void parse_decl_var(Token *tokens, int *pos);
void verifica_atribuicao_semantica(Token *tokens, int *pos);
void parse_atribuicao(Token *tokens, int *pos);
void parse_if(Token *tokens, int *pos);
void parse_while(Token *tokens, int *pos);
void parse_expressao(Token *tokens, int *pos);
void avanca_para_ponto_seguro(Token *tokens, int *pos);
void parse_termo(Token *tokens, int *pos);
void parse_fator(Token *tokens, int *pos);
void adicionar_tabela_simbolos(const char *nome, const char *lexema, int linha, int coluna);
void salvar_tabela_simbolos();
int eh_palavra_reservada(const char* lexema);
void analisar_lexico(FILE *input_file, Token *tokens, int *token_count);

void reporta_erro(int linha, int coluna, const char *mensagem, const char *valor) {
    fprintf(erros_file, "Erro! (%s: %s) - Linha: %d, Coluna: %d\n", mensagem, valor, linha, coluna);
    printf("Erro! (%s: %s) - Linha: %d, Coluna: %d\n", mensagem, valor, linha, coluna);
}

void parse_programa(Token *tokens, int *pos) {
    printf("Iniciando analise do programa...\n");

    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "program") == 0) {
        printf("Token 'program' encontrado. Avançando...\n");
        (*pos)++;  

        if (tokens[*pos].tipo == TOKEN_ID) {
            printf("Identificador encontrado: %s\n", tokens[*pos].lexema);  
            (*pos)++; 

            if (tokens[*pos].tipo == TOKEN_SMB_SEM) {
                printf("Token ';' encontrado. Declaração do programa correta.\n");
                (*pos)++; 
                fprintf(saida_sintatico, "Estrutura de programa reconhecida!\n");

                
                parse_decl_var(tokens, pos); 
           
                if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "begin") == 0) {
                    printf("Iniciando análise do bloco 'begin'.\n");
                    (*pos)++;  
                    parse_comandos(tokens, pos);  
                    return;
                }
            } else {
            
                reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando ';' apos identificador", tokens[*pos].lexema);
                fprintf(saida_sintatico, "Analisador sintatico falhou após 'program'.\n");
            }
        } else {
           
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava um identificador apos 'program'", tokens[*pos].lexema);
            fprintf(saida_sintatico, "Erro: Esperava um identificador apos 'program'.\n");
        }
    } else {
    
        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava palavra reservada 'program'", tokens[*pos].lexema);
        fprintf(saida_sintatico, "Erro: Esperava palavra reservada 'program'.\n");
    }
}

void parse_comandos(Token *tokens, int *pos) {
    printf("Iniciando analise dos comandos...\n");

    while (tokens[*pos].tipo != TOKEN_PAL_RES || strcmp(tokens[*pos].lexema, "end") != 0) {
   
        while (tokens[*pos].tipo == TOKEN_SMB_SEM) {
            (*pos)++;
        }

        if (tokens[*pos].tipo == TOKEN_ID) {
            parse_atribuicao(tokens, pos);
        } else if (tokens[*pos].tipo == TOKEN_PAL_RES) {
            if (strcmp(tokens[*pos].lexema, "if") == 0) {
                parse_if(tokens, pos);
            } else if (strcmp(tokens[*pos].lexema, "while") == 0) {
                parse_while(tokens, pos);
            } else if (strcmp(tokens[*pos].lexema, "begin") == 0) {
                (*pos)++;
                parse_comandos(tokens, pos);
            } else if (strcmp(tokens[*pos].lexema, "end") == 0) {
                break;
            }
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Comando invalido", tokens[*pos].lexema);
            avanca_para_ponto_seguro(tokens, pos);
        }
    }

    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "end") == 0) {
        (*pos)++; 

        if (tokens[*pos].tipo == TOKEN_SMB_PONTO) {
            (*pos)++;
            fprintf(saida_sintatico, "Fim do programa reconhecido com sucesso!\n");
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava '.' após 'end'", tokens[*pos].lexema);
        }
    } else {
        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando 'end' no final do programa", tokens[*pos].lexema);
    }
}


void parse_decl_var(Token *tokens, int *pos) {
    printf("Iniciando analise de declaracoes de variaveis...\n");

    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "var") == 0) {
        (*pos)++;
        while (tokens[*pos].tipo == TOKEN_ID) { 
            (*pos)++;
            if (tokens[*pos].tipo == TOKEN_OP_ASS) { 
                (*pos)++;
                if (tokens[*pos].tipo == TOKEN_PAL_RES &&
                    (strcmp(tokens[*pos].lexema, "integer") == 0 || strcmp(tokens[*pos].lexema, "real") == 0)) {
                    (*pos)++;
                    if (tokens[*pos].tipo == TOKEN_SMB_SEM) { 
                        (*pos)++;
                        fprintf(saida_sintatico, "Declaracao de variavel reconhecida!\n");
                    } else {
                        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando ';' apos tipo", tokens[*pos].lexema);
                    }
                } else {
                    reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Tipo invalido na declaracao", tokens[*pos].lexema);
                }
            } else {
                reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava ':' na declaracao", tokens[*pos].lexema);
            }
        }
    } else {
        fprintf(saida_sintatico, "Nenhuma declaracao de variavel encontrada.\n");
    }
}



void parse_atribuicao(Token *tokens, int *pos) {
    printf("Iniciando analise de atribuicoes...\n");

    if (tokens[*pos].tipo == TOKEN_ID) {
        printf("Identificador encontrado: %s\n", tokens[*pos].lexema);
        (*pos)++; 

        if (tokens[*pos].tipo == TOKEN_OP_ATRIB) {  
            printf("Operador ':=' encontrado\n");
            (*pos)++; 

            parse_expressao(tokens, pos); 

            if (tokens[*pos].tipo == TOKEN_SMB_SEM) { 
                printf("Ponto e virgula ';' encontrado apos a atribuicao.\n");
                (*pos)++;
                fprintf(saida_sintatico, "Atribuicao reconhecida!\n");
                return;
            } else {
                reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando ';' na atribuicao", tokens[*pos].lexema);
            }
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava ':=' na atribuicao", tokens[*pos].lexema);
        }
    } else {
        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava um identificador na atribuicao", tokens[*pos].lexema);
    }
}

void parse_expressao(Token *tokens, int *pos) {
    printf("Iniciando analise de expressao...\n");
    parse_termo(tokens, pos); 

    while (tokens[*pos].tipo == TOKEN_OP_AD || tokens[*pos].tipo == TOKEN_OP_MIN) {
        printf("Operador de baixa prioridade encontrado: %s\n", tokens[*pos].lexema);
        (*pos)++;
        parse_termo(tokens, pos);
    }

    if (tokens[*pos].tipo == TOKEN_OP_LT || tokens[*pos].tipo == TOKEN_OP_GT || 
        tokens[*pos].tipo == TOKEN_OP_LE || tokens[*pos].tipo == TOKEN_OP_GE || 
        tokens[*pos].tipo == TOKEN_OP_NE || tokens[*pos].tipo == TOKEN_OP_ASS) {
        printf("Operador relacional encontrado: %s\n", tokens[*pos].lexema);
        (*pos)++;
        parse_termo(tokens, pos);
    }

    fprintf(saida_sintatico, "Expressao reconhecida!\n");
}

void parse_if(Token *tokens, int *pos) {
    printf("Iniciando analise do IF...\n");

    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "if") == 0) {
        (*pos)++;  
        
     
        parse_expressao(tokens, pos);
  
        if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "then") == 0) {
            (*pos)++; 
          
            if (tokens[*pos].tipo == TOKEN_ID) {
                parse_atribuicao(tokens, pos);
                
                if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "else") == 0) {
                    (*pos)++; 
                    
                    if (tokens[*pos].tipo == TOKEN_ID) {
                        parse_atribuicao(tokens, pos);
                    }
                }
                
                fprintf(saida_sintatico, "Estrutura IF-THEN-ELSE reconhecida!\n");
            } else {
                reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, 
                           "Esperava comando apos 'then'", tokens[*pos].lexema);
            }
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, 
                        "Esperava 'then' após a condicao do IF", tokens[*pos].lexema);
        }
    }
}

void avanca_para_ponto_seguro(Token *tokens, int *pos) {
    while (tokens[*pos].tipo != TOKEN_SMB_SEM && 
           tokens[*pos].tipo != TOKEN_PAL_RES && 
           tokens[*pos].tipo != TOKEN_SMB_PONTO &&
           strcmp(tokens[*pos].lexema, "begin") != 0 &&
           strcmp(tokens[*pos].lexema, "end") != 0 &&
           tokens[*pos].tipo != 0) {
        (*pos)++;
    }
    printf("Erro tratado. Avancando para o proximo ponto seguro...\n");
}


void parse_while(Token *tokens, int *pos) {
    printf("Iniciando analise do WHILE...\n");

    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "while") == 0) {
        (*pos)++;  

        parse_expressao(tokens, pos);

        if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "do") == 0) {
            (*pos)++;  

            parse_comandos(tokens, pos);  

            fprintf(saida_sintatico, "Estrutura WHILE-DO reconhecida!\n");
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava 'do' apos a condicao do WHILE", tokens[*pos].lexema);
        }
    }
}


void parse_termo(Token *tokens, int *pos) {
    printf("Iniciando analise de termo...\n");
    parse_fator(tokens, pos); 

    while (tokens[*pos].tipo == TOKEN_OP_MULT || tokens[*pos].tipo == TOKEN_OP_DIV) {
        printf("Operador de alta prioridade encontrado: %s\n", tokens[*pos].lexema);
        (*pos)++;
        parse_fator(tokens, pos);
    }

    fprintf(saida_sintatico, "Termo reconhecido!\n");
}


void parse_fator(Token *tokens, int *pos) {
    printf("Iniciando analise de fator...\n");
    if (tokens[*pos].tipo == TOKEN_NUM_INT || tokens[*pos].tipo == TOKEN_NUM_FLOAT || tokens[*pos].tipo == TOKEN_ID) {
        printf("Fator identificado: %s\n", tokens[*pos].lexema);
        (*pos)++;
        fprintf(saida_sintatico, "Fator reconhecido!\n");
    } else if (tokens[*pos].tipo == TOKEN_SMB_OPA) { 
        printf("Parentese de abertura '(' encontrado. Analisando expressao dentro dos parenteses.\n");
        (*pos)++;
        parse_expressao(tokens, pos);
        if (tokens[*pos].tipo == TOKEN_SMB_CPA) { 
            printf("Parentese de fechamento ')' encontrado.\n");
            (*pos)++;
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando ')' após expressao", tokens[*pos].lexema);
        }
    } else {
        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Fator invalido", tokens[*pos].lexema);
    }
}


void adicionar_tabela_simbolos(const char *nome, const char *lexema, int linha, int coluna) {
    for (int i = 0; i < simbolo_count; i++) {
        if (strcmp(tabela_simbolos[i].lexema, lexema) == 0) {
            return;
        }
    }
    if (simbolo_count < TAMANHO_TABELA_SIMBOLOS) {
        strncpy(tabela_simbolos[simbolo_count].nome, nome, sizeof(tabela_simbolos[simbolo_count].nome) - 1);
        strncpy(tabela_simbolos[simbolo_count].lexema, lexema, sizeof(tabela_simbolos[simbolo_count].lexema) - 1);
        tabela_simbolos[simbolo_count].linha = linha;
        tabela_simbolos[simbolo_count].coluna = coluna;
        tabela_simbolos[simbolo_count].indice = simbolo_count;
        simbolo_count++;
    } else {
        printf("Erro: Tabela de simbolos cheia!\n");
    }
}

void salvar_tabela_simbolos() {
    for (int i = 0; i < simbolo_count; i++) {
        fprintf(tabela_simbolos_file, "Indice: %d, Nome: %s, Lexema: %s, Linha: %d, Coluna: %d\n",
                tabela_simbolos[i].indice, tabela_simbolos[i].nome, tabela_simbolos[i].lexema,
                tabela_simbolos[i].linha, tabela_simbolos[i].coluna);
    }
}

int eh_palavra_reservada(const char* lexema) {
    const char* palavras_reservadas[] = { "program", "var", "integer", "real", "begin", "end", "if", "then", "else", "while", "do"};
    for (int i = 0; i < sizeof(palavras_reservadas) / sizeof(palavras_reservadas[0]); i++) {
        if (strcmp(lexema, palavras_reservadas[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void analisar_lexico(FILE *input_file, Token *tokens, int *token_count) {
    int c;
    int linha = 1, coluna = 1;

    printf("Iniciando analise lexica...\n");
    while ((c = fgetc(input_file)) != EOF) {
        printf("Caractere lido: '%c' (Linha: %d, Coluna: %d)\n", c, linha, coluna);

        if (isspace(c)) {
            if (c == '\n') {
                linha++;
                coluna = 1;
                printf("Quebra de linha detectada. Linha agora: %d\n", linha);
            } else {
                coluna++;
            }
        } else if (isalpha(c)) {
            char lexema[100] = {c, '\0'};
            int lex_pos = 1;

            while ((c = fgetc(input_file)) != EOF && (isalnum(c) || c == '_')) {
                lexema[lex_pos++] = c;
                coluna++;
            }
            lexema[lex_pos] = '\0';
            ungetc(c, input_file);

            printf("Lexema identificado como palavra ou identificador: '%s'\n", lexema);
            if (eh_palavra_reservada(lexema)) {
                tokens[*token_count].tipo = TOKEN_PAL_RES;
                strncpy(tokens[*token_count].lexema, lexema, sizeof(tokens[*token_count].lexema) - 1);
                tokens[*token_count].linha = linha;
                tokens[*token_count].coluna = coluna;
                fprintf(saida_lex, "Token: PAL_RES (%s), Linha: %d, Coluna: %d\n", lexema, linha, coluna);
            } else {
                tokens[*token_count].tipo = TOKEN_ID;
                strncpy(tokens[*token_count].lexema, lexema, sizeof(tokens[*token_count].lexema) - 1);
                tokens[*token_count].linha = linha;
                tokens[*token_count].coluna = coluna;
                fprintf(saida_lex, "Token: ID (%s), Linha: %d, Coluna: %d\n", lexema, linha, coluna);
                adicionar_tabela_simbolos("ID", lexema, linha, coluna);
            }
            (*token_count)++;
        } else if (isdigit(c)) {
            char numero[100] = {c, '\0'};
            int num_pos = 1;
            int tem_ponto = 0;

            while ((c = fgetc(input_file)) != EOF && (isdigit(c) || c == '.')) {
                if (c == '.') {
                    if (tem_ponto) break;
                    tem_ponto = 1;
                }
                numero[num_pos++] = c;
                coluna++;
            }
            numero[num_pos] = '\0';
            ungetc(c, input_file);

            printf("Numero identificado: '%s'\n", numero);
            tokens[*token_count].tipo = (tem_ponto) ? TOKEN_NUM_FLOAT : TOKEN_NUM_INT;
            strncpy(tokens[*token_count].lexema, numero, sizeof(tokens[*token_count].lexema) - 1);
            tokens[*token_count].linha = linha;
            tokens[*token_count].coluna = coluna;
            fprintf(saida_lex, "Token: NUMERO (%s), Linha: %d, Coluna: %d\n", numero, linha, coluna);
            (*token_count)++;
        } else {
            printf("Analisando simbolo: '%c'\n", c);
            Token token = {.linha = linha, .coluna = coluna};
            switch (c) {
                case '<':
                    c = fgetc(input_file);
                    if (c == '>') {
                        token.tipo = TOKEN_OP_NE;
                        fprintf(saida_lex, "Token: OP_NE (<>), Linha: %d, Coluna: %d\n", linha, coluna);
                    } else if (c == '=') {
                        token.tipo = TOKEN_OP_LE;
                        fprintf(saida_lex, "Token: OP_LE (<=), Linha: %d, Coluna: %d\n", linha, coluna);
                    } else {
                        ungetc(c, input_file);
                        token.tipo = TOKEN_OP_LT;
                        fprintf(saida_lex, "Token: OP_LT (<), Linha: %d, Coluna: %d\n", linha, coluna);
                    }
                    break;

                case '>':
                    c = fgetc(input_file);
                    if (c == '=') {
                        token.tipo = TOKEN_OP_GE;
                        fprintf(saida_lex, "Token: OP_GE (>=), Linha: %d, Coluna: %d\n", linha, coluna);
                    } else {
                        ungetc(c, input_file);
                        token.tipo = TOKEN_OP_GT;
                        fprintf(saida_lex, "Token: OP_GT (>), Linha: %d, Coluna: %d\n", linha, coluna);
                    }
                    break;

                case ':':
                    c = fgetc(input_file);
                    if (c == '=') {
                        token.tipo = TOKEN_OP_ATRIB;
                        fprintf(saida_lex, "Token: OP_ATRIB (:=), Linha: %d, Coluna: %d\n", linha, coluna);
                    } else {
                        ungetc(c, input_file);
                        token.tipo = TOKEN_OP_ASS;
                        fprintf(saida_lex, "Token: OP_ASS (:), Linha: %d, Coluna: %d\n", linha, coluna);
                    }
                    break;

                case '+':
                    token.tipo = TOKEN_OP_AD;
                    fprintf(saida_lex, "Token: OP_AD (+), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                case '-':
                    token.tipo = TOKEN_OP_MIN;
                    fprintf(saida_lex, "Token: OP_MIN (-), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                case '*':
                    token.tipo = TOKEN_OP_MULT;
                    fprintf(saida_lex, "Token: OP_MULT (*), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                case '/':
                    token.tipo = TOKEN_OP_DIV;
                    fprintf(saida_lex, "Token: OP_DIV (/), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                case '.':
                    token.tipo = TOKEN_SMB_PONTO;
                    fprintf(saida_lex, "Token: SMB_PONTO (.), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                case ',':
                    token.tipo = TOKEN_SMB_COM;
                    fprintf(saida_lex, "Token: SMB_COM (,), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                case ';':
                    token.tipo = TOKEN_SMB_SEM;
                    fprintf(saida_lex, "Token: SMB_SEM (;), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                case '(':
                    token.tipo = TOKEN_SMB_OPA;
                    fprintf(saida_lex, "Token: SMB_OPA ((), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                case ')':
                    token.tipo = TOKEN_SMB_CPA;
                    fprintf(saida_lex, "Token: SMB_CPA ()), Linha: %d, Coluna: %d\n", linha, coluna);
                    break;

                default:
                    reporta_erro(linha, coluna, "Símbolo desconhecido", (char[]){c, '\0'});
                    continue;
            }
            tokens[*token_count] = token;
            (*token_count)++;
        }
        coluna++;
    }
    printf("Analise lexica concluida. Tokens gerados: %d\n", *token_count);
}


int main(int argc, char **argv) { 
    
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }
    
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Erro ao abrir arquivo fonte");
        return 1;
    }
    
    saida_lex = fopen("saida_lex.txt", "w");
    erros_file = fopen("erros.txt", "w");
    tabela_simbolos_file = fopen("tabela_simbolos.txt", "w");
    saida_sintatico = fopen("saida_sintatico.txt", "w");
    
    Token tokens[1000];
    int token_count = 0;
    
    analisar_lexico(input_file, tokens, &token_count);
    int pos = 0;
    parse_programa(tokens, &pos);
    salvar_tabela_simbolos();
    
    fclose(input_file);
    fclose(saida_lex);
    fclose(erros_file);
    fclose(tabela_simbolos_file);
    fclose(saida_sintatico);
    
    return 0;
}