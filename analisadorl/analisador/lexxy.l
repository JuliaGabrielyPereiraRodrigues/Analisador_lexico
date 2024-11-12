%{
#include <stdio.h>
#include <string.h>

#define TAMANHO_TABELA_SIMBOLOS 100

typedef struct {
    char nome[20];
    char lexema[100];
    int linha;
    int coluna;
    int indice;
} Symbol;

Symbol tabela_simbolos[TAMANHO_TABELA_SIMBOLOS];
int simbolo_count = 0;

FILE *saida_lex, *tabela_simbolos_file, *erros_file;

void reporta_erro(int linha, int coluna, const char *mensagem, const char *valor) {
    fprintf(erros_file, "Erro! (%s: %s) - Linha: %d, Coluna: %d\n", mensagem, valor, linha, coluna);
}

int adicionar_tabela_simbolos(const char *nome, const char *lexema, int linha, int coluna) {
    for (int i = 0; i < simbolo_count; i++) {
        if (strcmp(tabela_simbolos[i].lexema, lexema) == 0) {
            return i; 
        }
    }

    if (simbolo_count < TAMANHO_TABELA_SIMBOLOS) {
        strncpy(tabela_simbolos[simbolo_count].nome, nome, sizeof(tabela_simbolos[simbolo_count].nome) - 1);
        strncpy(tabela_simbolos[simbolo_count].lexema, lexema, sizeof(tabela_simbolos[simbolo_count].lexema) - 1);
        tabela_simbolos[simbolo_count].linha = linha;
        tabela_simbolos[simbolo_count].coluna = coluna;
        tabela_simbolos[simbolo_count].indice = simbolo_count;
        simbolo_count++;
        return simbolo_count - 1;
    } else {
        reporta_erro(linha, coluna, "Tabela de símbolos cheia", "");
        return -1;
    }
}

int linha = 1, coluna = 1;
%}

%%

[ \t]+          ;

\n              { linha++; coluna = 1; }

"program"      { fprintf(saida_lex, "Token: PAL_RES (program), Linha: %d, Coluna: %d\n", linha, coluna); }
"var"          { fprintf(saida_lex, "Token: PAL_RES (var), Linha: %d, Coluna: %d\n", linha, coluna); }
"integer"      { fprintf(saida_lex, "Token: PAL_RES (integer), Linha: %d, Coluna: %d\n", linha, coluna); }
"real"         { fprintf(saida_lex, "Token: PAL_RES (real), Linha: %d, Coluna: %d\n", linha, coluna); }
"begin"        { fprintf(saida_lex, "Token: PAL_RES (begin), Linha: %d, Coluna: %d\n", linha, coluna); }
"end"          { fprintf(saida_lex, "Token: PAL_RES (end), Linha: %d, Coluna: %d\n", linha, coluna); }
"if"           { fprintf(saida_lex, "Token: PAL_RES (if), Linha: %d, Coluna: %d\n", linha, coluna); }
"then"         { fprintf(saida_lex, "Token: PAL_RES (then), Linha: %d, Coluna: %d\n", linha, coluna); }
"else"         { fprintf(saida_lex, "Token: PAL_RES (else), Linha: %d, Coluna: %d\n", linha, coluna); }
"while"        { fprintf(saida_lex, "Token: PAL_RES (while), Linha: %d, Coluna: %d\n", linha, coluna); }
"do"           { fprintf(saida_lex, "Token: PAL_RES (do), Linha: %d, Coluna: %d\n", linha, coluna); }

[0-9]+         { fprintf(saida_lex, "Token: NUM_INT (%s), Linha: %d, Coluna: %d\n", yytext, linha, coluna); }

[0-9]+\.[0-9]+ { fprintf(saida_lex, "Token: NUM_FLOAT (%s), Linha: %d, Coluna: %d\n", yytext, linha, coluna); }

[a-zA-Z_][a-zA-Z0-9_]* {
    int indice = adicionar_tabela_simbolos("ID", yytext, linha, coluna);
   fprintf(saida_lex, "Token: ID (%s), Linha: %d, Coluna: %d\n", yytext, linha, coluna);
}

"+"            { fprintf(saida_lex, "Token: OP_AD, Linha: %d, Coluna: %d\n", linha, coluna); }
"-"            { fprintf(saida_lex, "Token: OP_MIN, Linha: %d, Coluna: %d\n", linha, coluna); }
"="            { fprintf(saida_lex, "Token: OP_EQ, Linha: %d, Coluna: %d\n", linha, coluna); }
";"            { fprintf(saida_lex, "Token: SMB_SEM, Linha: %d, Coluna: %d\n", linha, coluna); }
":"            { fprintf(saida_lex, "Token: OP_ASS, Linha: %d, Coluna: %d\n", linha, coluna); }
"{"            { fprintf(saida_lex, "Token: SMB_OBC, Linha: %d, Coluna: %d\n", linha, coluna); }
"}"            { fprintf(saida_lex, "Token: SMB_CBC, Linha: %d, Coluna: %d\n", linha, coluna); }
"("            { fprintf(saida_lex, "Token: SMB_OPA, Linha: %d, Coluna: %d\n", linha, coluna); }
")"            { fprintf(saida_lex, "Token: SMB_CPA, Linha: %d, Coluna: %d\n", linha, coluna); }
","            { fprintf(saida_lex, "Token: SMB_COM, Linha: %d, Coluna: %d\n", linha, coluna); }
"."            { fprintf(saida_lex, "Token: SMB_PONTO, Linha: %d, Coluna: %d\n", linha, coluna); }

"<>"           { fprintf(saida_lex, "Token: OP_NE, Linha: %d, Coluna: %d\n", linha, coluna); }
"<="           { fprintf(saida_lex, "Token: OP_LE, Linha: %d, Coluna: %d\n", linha, coluna); }
">="           { fprintf(saida_lex, "Token: OP_GE, Linha: %d, Coluna: %d\n", linha, coluna); }
":="           { fprintf(saida_lex, "Token: OP_ATRIB, Linha: %d, Coluna: %d\n", linha, coluna); }
">"            { fprintf(saida_lex, "Token: OP_GT, Linha: %d, Coluna: %d\n", linha, coluna); }
"<"            { fprintf(saida_lex, "Token: OP_LT, Linha: %d, Coluna: %d\n", linha, coluna); }
"/"            { fprintf(saida_lex, "Token: OP_DIV, Linha: %d, Coluna: %d\n", linha, coluna); }
"*"            { fprintf(saida_lex, "Token: OP_MUL, Linha: %d, Coluna: %d\n", linha, coluna); }

[0-9]+[a-zA-Z_]           { reporta_erro(linha, coluna, "Identificador inválido após número", yytext); }

[0-9]*\.[0-9]+[a-zA-Z_]    { reporta_erro(linha, coluna, "Número inválido seguido de identificador", yytext); }

[0-9]+\.[0-9]+\.[a-zA-Z_]  { reporta_erro(linha, coluna, "Número inválido com múltiplos pontos", yytext); }

[0-9]*\.[0-9]+\.[0-9]+     { reporta_erro(linha, coluna, "Número inválido com múltiplos pontos", yytext); }

[0-9]*\.[0-9]*\.[a-zA-Z_]  { reporta_erro(linha, coluna, "Número inválido seguido de identificador", yytext); }

[0-9]*\.[0-9]*\.[0-9]      { reporta_erro(linha, coluna, "Número inválido com múltiplos pontos", yytext); }

[0-9]*\.+[a-zA-Z_]         { reporta_erro(linha, coluna, "Número inválido seguido de identificador", yytext); }

%%

int yywrap() {
    return 1;
}

int main(int argc, char **argv) {
    FILE *input_file;

    if (argc > 1) {
        input_file = fopen(argv[1], "r");
        if (!input_file) {
            perror("Erro ao abrir o arquivo de entrada");
            return 1;
        }

        saida_lex = fopen("saida.lex", "w");
        if (!saida_lex) {
            perror("Erro ao criar o arquivo de saída de tokens");
            return 1;
        }

        tabela_simbolos_file = fopen("tabela_simbolos.txt", "w");
        if (!tabela_simbolos_file) {
            perror("Erro ao criar o arquivo de tabela de símbolos");
            return 1;
        }

        erros_file = fopen("erros.txt", "w");
        if (!erros_file) {
            perror("Erro ao criar o arquivo de erros");
            return 1;
        }

        yyin = input_file;
        yylex();

        fprintf(tabela_simbolos_file, "\nTabela de símbolos:\n");
        for (int i = 0; i < simbolo_count; i++) {
            fprintf(tabela_simbolos_file, "Indice: %d, Nome: %s, Lexema: %s, Linha: %d, Coluna: %d\n",
                tabela_simbolos[i].indice,
                tabela_simbolos[i].nome,
                tabela_simbolos[i].lexema,
                tabela_simbolos[i].linha,
                tabela_simbolos[i].coluna);
        }

        fclose(input_file);
        fclose(saida_lex);
        fclose(tabela_simbolos_file);
        fclose(erros_file);

    } else {
        printf("Modo interativo:\n");
        yyin = stdin;
        yylex();
    }

    return 0;
}
