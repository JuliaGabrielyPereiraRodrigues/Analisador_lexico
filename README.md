
<div align="center">

# 🔎 Analisador Léxico e Sintático

**Um projeto acadêmico em C para explorar as etapas iniciais de um compilador.**

[![Linguagem C](https://img.shields.io/badge/Linguagem-C-00599C?style=for-the-badge&logo=c&logoColor=white)](https://en.cppreference.com/w/c)
![Compiladores](https://img.shields.io/badge/Área-Compiladores-334155?style=for-the-badge)
![Projeto acadêmico](https://img.shields.io/badge/Tipo-Projeto%20Acadêmico-6D28D9?style=for-the-badge)

[Visão geral](#-visão-geral) · [Funcionalidades](#-funcionalidades) · [Como executar](#-como-executar) · [Estrutura](#-estrutura-do-repositório)

</div>

---

## 📖 Visão geral

Este repositório contém um **analisador léxico com rotinas de análise sintática**, desenvolvido individualmente na disciplina de **Compiladores**. A implementação processa um arquivo de entrada escrito em uma linguagem didática com construções semelhantes às de Pascal, identifica tokens e tenta reconhecer a estrutura dos comandos.

O projeto também inclui uma **tabela de símbolos** e um **módulo separado de funções para verificações semânticas** (`semantico.c`). Esse módulo não é chamado pelo programa principal na versão atual.

## ✨ Funcionalidades

- **Análise léxica:** reconhecimento de palavras reservadas, identificadores, números e símbolos.
- **Localização dos tokens:** registro de linha e coluna na saída léxica.
- **Análise sintática:** rotinas para declaração de variáveis, atribuições, expressões, condicionais `if/then/else` e laços `while/do`.
- **Tabela de símbolos:** armazenamento de identificadores encontrados e suas posições.
- **Relatórios em texto:** geração de arquivos de tokens, análise sintática, símbolos e erros léxicos.
- **Estudos de semântica:** funções independentes para conferir declaração de variáveis e compatibilidade de tipos em `semantico.c`.

> **Escopo:** trata-se de uma implementação didática, não de um compilador completo. As rotinas semânticas ainda não estão integradas ao fluxo principal.

## 🛠️ Tecnologias

| Tecnologia | Utilização |
| :--- | :--- |
| **C** | Implementação dos analisadores e da tabela de símbolos |
| **Biblioteca padrão de C** | Leitura de arquivos, manipulação de strings e classificação de caracteres |
| **GCC** | Opção de compilação local |

## 🚀 Como executar

### Pré-requisitos

- Um compilador C, como **GCC**.
- Um terminal (PowerShell, Prompt de Comando, Bash etc.).

### 1. Clone o repositório

```bash
git clone https://github.com/JuliaGabrielyPereiraRodrigues/Analisador_lexico.git
cd Analisador_lexico/Analisador
```

### 2. Compile o analisador principal

No Linux/macOS:

```bash
gcc -std=c11 analisador.c -o analisador
```

No Windows (com GCC instalado):

```powershell
gcc -std=c11 analisador.c -o analisador.exe
```

### 3. Execute com o arquivo de exemplo

Linux/macOS:

```bash
./analisador test.txt
```

Windows (PowerShell):

```powershell
.\analisador.exe test.txt
```

O programa recebe o **caminho de um arquivo-fonte como argumento**. Execute-o dentro da pasta `Analisador`, pois os relatórios são gravados no diretório de trabalho atual.

## 🧪 Exemplo de entrada

O arquivo `Analisador/test.txt` contém um programa de teste, incluindo declarações, atribuições e uma condicional:

```pascal
program exemplo;

var
  x : integer;
  y : real;

begin
  x := 5;
  y := 10.2;
  if x < y then
    x := x + 1;
  else
    y := y - 2.5;
end.
```

### Arquivos de saída

| Arquivo | Conteúdo |
| :--- | :--- |
| `saida_lex.txt` | Tokens identificados, com linha e coluna |
| `saida_sintatico.txt` | Registros do reconhecimento de estruturas sintáticas |
| `tabela_simbolos.txt` | Identificadores registrados na tabela de símbolos |
| `erros.txt` | Ocorrências de símbolos léxicos não reconhecidos |

Exemplo de linha de `saida_lex.txt`:

```text
Token: PAL_RES (program), Linha: 1, Coluna: 7
```

> **Observação:** os arquivos de saída são sobrescritos a cada execução. A análise sintática também imprime mensagens detalhadas no terminal.

## 📁 Estrutura do repositório

```text
Analisador_lexico/
├── Analisador/
│   ├── analisador.c          # Análise léxica, análise sintática e execução principal
│   ├── semantico.c           # Funções semânticas separadas
│   ├── test.txt              # Programa de teste
│   ├── saida_lex.txt         # Exemplo de relatório léxico
│   ├── saida_sintatico.txt   # Exemplo de relatório sintático
│   ├── tabela_simbolos.txt   # Exemplo de tabela de símbolos
│   ├── erros.txt             # Relatório de erros
│   └── analisador.exe        # Executável incluído no repositório original
└── README.md
```

## 📌 Aprendizados e próximos passos

O projeto exercita a **tokenização**, o **reconhecimento de estruturas gramaticais**, o **gerenciamento de identificadores** e a **separação das etapas de análise**.

Possíveis evoluções:

- Integrar as verificações de `semantico.c` ao analisador principal.
- Adicionar testes automatizados para entradas válidas e inválidas.
- Melhorar o tratamento de erros e limites dos buffers de tokens.
- Separar os módulos léxico e sintático em arquivos próprios.
- Evitar versionar executáveis e relatórios gerados automaticamente.

## 👩‍💻 Autoria

Desenvolvido por **[Julia Gabriely](https://github.com/JuliaGabrielyPereiraRodrigues)** como projeto acadêmico da disciplina de Compiladores.

<div align="center">

---

**[Voltar ao topo ↑](#-analisador-léxico-e-sintático)**

</div>
