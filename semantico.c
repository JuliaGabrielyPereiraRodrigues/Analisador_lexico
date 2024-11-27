#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VAR 100

typedef struct {
    char nome[50];
    char tipo[10];
    int declarada;
} Variavel;

Variavel tabela_simbolos[MAX_VAR];
int total_variaveis = 0;

void registrar_variavel(const char *nome, const char *tipo) {
    for (int i = 0; i < total_variaveis; i++) {
        if (strcmp(tabela_simbolos[i].nome, nome) == 0) {
            printf("Erro Semântico: Variável '%s' já declarada.\n", nome);
            exit(1);
        }
    }
    if (total_variaveis >= MAX_VAR) {
        printf("Erro Semântico: Tabela de símbolos cheia.\n");
        exit(1);
    }
    strcpy(tabela_simbolos[total_variaveis].nome, nome);
    strcpy(tabela_simbolos[total_variaveis].tipo, tipo);
    tabela_simbolos[total_variaveis].declarada = 1;
    total_variaveis++;
}

int buscar_variavel(const char *nome) {
    for (int i = 0; i < total_variaveis; i++) {
        if (strcmp(tabela_simbolos[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

void verificar_variavel_declarada(const char *nome) {
    int indice = buscar_variavel(nome);
    if (indice == -1) {
        printf("Erro Semântico: Variável '%s' não declarada.\n", nome);
        exit(1);
    }
}

void verificar_tipo(const char *var_tipo, const char *expr_tipo) {
    if (strcmp(var_tipo, "integer") == 0 && strcmp(expr_tipo, "real") == 0) {
        printf("Erro Semântico: Não é permitido atribuir 'real' a 'integer'.\n");
        exit(1);
    }
}

void analisar_declaracao(const char *nome, const char *tipo) {
    registrar_variavel(nome, tipo);
}

void analisar_atribuicao(const char *nome, const char *tipo_expressao) {
    verificar_variavel_declarada(nome);
    int indice = buscar_variavel(nome);
    verificar_tipo(tabela_simbolos[indice].tipo, tipo_expressao);
}

const char *analisar_expressao(const char *operando1, const char *operando2, const char *operador) {
    if (strcmp(operador, "+") == 0 || strcmp(operador, "-") == 0 ||
        strcmp(operador, "*") == 0 || strcmp(operador, "/") == 0) {
        if (strcmp(operando1, "integer") == 0 && strcmp(operando2, "integer") == 0) {
            return "integer";
        }
        if (strcmp(operando1, "real") == 0 || strcmp(operando2, "real") == 0) {
            return "real";
        }
    }
    printf("Erro Semântico: Operador '%s' inválido para os tipos fornecidos.\n", operador);
    exit(1);
    return "";
}
