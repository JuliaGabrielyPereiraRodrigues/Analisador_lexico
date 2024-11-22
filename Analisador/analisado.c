#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

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
    printf("Iniciando análise do programa...\n");

    // Verifica se o primeiro token é a palavra reservada "program"
    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "program") == 0) {
        printf("Token 'program' encontrado. Avançando...\n");
        (*pos)++;  // Avança para o próximo token, que deve ser o identificador

        // Verifica se o próximo token é um identificador
        if (tokens[*pos].tipo == TOKEN_ID) {
            printf("Identificador encontrado: %s\n", tokens[*pos].lexema);  // Aqui vemos o identificador
            (*pos)++;  // Avança para o próximo token após o identificador

            // Verifica se o próximo token é ";"
            if (tokens[*pos].tipo == TOKEN_SMB_SEM) {
                printf("Token ';' encontrado. Declaração do programa correta.\n");
                (*pos)++;  // Avança para o próximo token após ";"
                fprintf(saida_sintatico, "Estrutura de programa reconhecida!\n");

                // A partir daqui, chama a próxima função para analisar as declarações de variáveis
                parse_decl_var(tokens, pos); // Analisando declaração de variáveis

                // Após declarar as variáveis, avança para o próximo bloco de comandos
                if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "begin") == 0) {
                    printf("Iniciando análise do bloco 'begin'.\n");
                    (*pos)++;  // Avança para o próximo token após "begin"
                    parse_comandos(tokens, pos);  // Função que analisa os comandos do programa
                    return;
                }
            } else {
                // Se não encontrar ponto e vírgula
                reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando ';' após identificador", tokens[*pos].lexema);
                fprintf(saida_sintatico, "Analisador sintático falhou após 'program'.\n");
            }
        } else {
            // Se não encontrar identificador após 'program'
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava um identificador após 'program'", tokens[*pos].lexema);
            fprintf(saida_sintatico, "Erro: Esperava um identificador após 'program'.\n");
        }
    } else {
        // Se não encontrar 'program'
        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava palavra reservada 'program'", tokens[*pos].lexema);
        fprintf(saida_sintatico, "Erro: Esperava palavra reservada 'program'.\n");
    }
}


void parse_decl_var(Token *tokens, int *pos) {
    printf("Iniciando análise de declarações de variáveis...\n");

    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "var") == 0) {
        (*pos)++;
        while (tokens[*pos].tipo == TOKEN_ID) { // Enquanto houver identificadores
            (*pos)++;
            if (tokens[*pos].tipo == TOKEN_OP_ASS) { // Espera ':'
                (*pos)++;
                if (tokens[*pos].tipo == TOKEN_PAL_RES &&
                    (strcmp(tokens[*pos].lexema, "integer") == 0 || strcmp(tokens[*pos].lexema, "real") == 0)) {
                    (*pos)++;
                    if (tokens[*pos].tipo == TOKEN_SMB_SEM) { // Espera ';'
                        (*pos)++;
                        fprintf(saida_sintatico, "Declaração de variável reconhecida!\n");
                    } else {
                        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando ';' após tipo", tokens[*pos].lexema);
                    }
                } else {
                    reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Tipo inválido na declaração", tokens[*pos].lexema);
                }
            } else {
                reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava ':' na declaração", tokens[*pos].lexema);
            }
        }
    } else {
        fprintf(saida_sintatico, "Nenhuma declaração de variável encontrada.\n");
    }
}



void parse_atribuicao(Token *tokens, int *pos) {
    printf("Iniciando análise de atribuições...\n");

    if (tokens[*pos].tipo == TOKEN_ID) {
        printf("Identificador encontrado: %s\n", tokens[*pos].lexema);
        (*pos)++;  // Avança para o próximo token após o identificador

        if (tokens[*pos].tipo == TOKEN_OP_ATRIB) {  // Verifica ":="
            printf("Operador ':=' encontrado\n");
            (*pos)++;  // Avança para o próximo token após ":="

            parse_expressao(tokens, pos);  // Analisa a expressão atribuída

            if (tokens[*pos].tipo == TOKEN_SMB_SEM) {  // Verifica ";"
                printf("Ponto e vírgula ';' encontrado após a atribuição.\n");
                (*pos)++;
                fprintf(saida_sintatico, "Atribuição reconhecida!\n");
                return;
            } else {
                reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando ';' na atribuição", tokens[*pos].lexema);
            }
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava ':=' na atribuição", tokens[*pos].lexema);
        }
    } else {
        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava um identificador na atribuição", tokens[*pos].lexema);
    }
}

void parse_expressao(Token *tokens, int *pos) {
    printf("Iniciando análise de expressão...\n");
    parse_termo(tokens, pos);  // Primeiro termo da expressão
    
    // Verifica operadores relacionais e aritméticos
    while (tokens[*pos].tipo == TOKEN_OP_AD || 
           tokens[*pos].tipo == TOKEN_OP_MIN ||
           tokens[*pos].tipo == TOKEN_OP_LT ||
           tokens[*pos].tipo == TOKEN_OP_GT ||
           tokens[*pos].tipo == TOKEN_OP_LE ||
           tokens[*pos].tipo == TOKEN_OP_GE ||
           tokens[*pos].tipo == TOKEN_OP_NE) {
        
        printf("Operador encontrado: %s\n", tokens[*pos].lexema);
        (*pos)++;
        parse_termo(tokens, pos);
    }
    fprintf(saida_sintatico, "Expressão reconhecida!\n");
}

void parse_if(Token *tokens, int *pos) {
    printf("Iniciando análise do IF...\n");

    // Verifica 'if'
    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "if") == 0) {
        (*pos)++;  // Avança depois do 'if'
        
        // Analisa a condição
        parse_expressao(tokens, pos);
        
        // Verifica 'then'
        if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "then") == 0) {
            (*pos)++;  // Avança depois do 'then'
            
            // Analisa o bloco do then
            if (tokens[*pos].tipo == TOKEN_ID) {
                parse_atribuicao(tokens, pos);
                
                // Verifica se há else
                if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "else") == 0) {
                    (*pos)++;  // Avança depois do 'else'
                    
                    // Analisa o bloco do else
                    if (tokens[*pos].tipo == TOKEN_ID) {
                        parse_atribuicao(tokens, pos);
                    }
                }
                
                fprintf(saida_sintatico, "Estrutura IF-THEN-ELSE reconhecida!\n");
            } else {
                reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, 
                           "Esperava comando após 'then'", tokens[*pos].lexema);
            }
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, 
                        "Esperava 'then' após a condição do IF", tokens[*pos].lexema);
        }
    }
}

void parse_comandos(Token *tokens, int *pos) {
    printf("Iniciando análise dos comandos...\n");

    while (tokens[*pos].tipo != TOKEN_PAL_RES || strcmp(tokens[*pos].lexema, "end") != 0) {
        // Ignora ponto e vírgula extras
        while (tokens[*pos].tipo == TOKEN_SMB_SEM) {
            (*pos)++;
        }

        if (tokens[*pos].tipo == TOKEN_ID) {
            parse_atribuicao(tokens, pos);
        }
        else if (tokens[*pos].tipo == TOKEN_PAL_RES) {
            if (strcmp(tokens[*pos].lexema, "if") == 0) {
                parse_if(tokens, pos);
            }
            else if (strcmp(tokens[*pos].lexema, "while") == 0) {
                parse_while(tokens, pos);
            }
            else if (strcmp(tokens[*pos].lexema, "begin") == 0) {
                (*pos)++;
                continue;
            }
            else if (strcmp(tokens[*pos].lexema, "end") == 0) {
                break;
            }
        }
        else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Comando inválido", tokens[*pos].lexema);
            (*pos)++;
        }
    }

    // Verifica o final do bloco
    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "end") == 0) {
        (*pos)++;  // Avança após o 'end'
        
        // Verifica se tem ponto e vírgula ou ponto final
        if (tokens[*pos].tipo == TOKEN_SMB_SEM || tokens[*pos].tipo == TOKEN_SMB_PONTO) {
            (*pos)++;
            fprintf(saida_sintatico, "Fim do bloco de comandos reconhecido!\n");
        }
    }
}

// Modificação na função parse_while para melhor tratamento do bloco de comandos
void parse_while(Token *tokens, int *pos) {
    printf("Iniciando análise do WHILE...\n");

    if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "while") == 0) {
        (*pos)++;  // Avança após 'while'

        parse_expressao(tokens, pos);

        if (tokens[*pos].tipo == TOKEN_PAL_RES && strcmp(tokens[*pos].lexema, "do") == 0) {
            (*pos)++;  // Avança após 'do'

            parse_comandos(tokens, pos);  // Analisa o bloco de comandos do while

            fprintf(saida_sintatico, "Estrutura WHILE-DO reconhecida!\n");
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Esperava 'do' após a condição do WHILE", tokens[*pos].lexema);
        }
    }
}


void parse_termo(Token *tokens, int *pos) {
    printf("Iniciando análise de termo...\n");
    parse_fator(tokens, pos); // Primeiro fator do termo
    while (tokens[*pos].tipo == TOKEN_OP_MULT || tokens[*pos].tipo == TOKEN_OP_DIV) { // "*" ou "/"
        printf("Operador de termo encontrado: %s\n", tokens[*pos].lexema);
        (*pos)++;
        parse_fator(tokens, pos); // Próximo fator
    }
    fprintf(saida_sintatico, "Termo reconhecido!\n");
}

void parse_fator(Token *tokens, int *pos) {
    printf("Iniciando análise de fator...\n");
    if (tokens[*pos].tipo == TOKEN_NUM_INT || tokens[*pos].tipo == TOKEN_NUM_FLOAT || tokens[*pos].tipo == TOKEN_ID) {
        printf("Fator identificado: %s\n", tokens[*pos].lexema);
        (*pos)++;
        fprintf(saida_sintatico, "Fator reconhecido!\n");
    } else if (tokens[*pos].tipo == TOKEN_SMB_OPA) { // "("
        printf("Parêntese de abertura '(' encontrado. Analisando expressão dentro dos parênteses.\n");
        (*pos)++;
        parse_expressao(tokens, pos); // Expressão dentro dos parênteses
        if (tokens[*pos].tipo == TOKEN_SMB_CPA) { // ")"
            printf("Parêntese de fechamento ')' encontrado.\n");
            (*pos)++;
        } else {
            reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Faltando ')' após expressão", tokens[*pos].lexema);
        }
    } else {
        reporta_erro(tokens[*pos].linha, tokens[*pos].coluna, "Fator inválido", tokens[*pos].lexema);
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
        printf("Erro: Tabela de símbolos cheia!\n");
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

    printf("Iniciando análise léxica...\n");
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

            printf("Número identificado: '%s'\n", numero);
            tokens[*token_count].tipo = (tem_ponto) ? TOKEN_NUM_FLOAT : TOKEN_NUM_INT;
            strncpy(tokens[*token_count].lexema, numero, sizeof(tokens[*token_count].lexema) - 1);
            tokens[*token_count].linha = linha;
            tokens[*token_count].coluna = coluna;
            fprintf(saida_lex, "Token: NUMERO (%s), Linha: %d, Coluna: %d\n", numero, linha, coluna);
            (*token_count)++;
        } else {
            printf("Analisando símbolo: '%c'\n", c);
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
    printf("Análise léxica concluída. Tokens gerados: %d\n", *token_count);
}


int main(int argc, char **argv) {
    setlocale(LC_ALL, "pt_BR.UTF-8");  
    
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