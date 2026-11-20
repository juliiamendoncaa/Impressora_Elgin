# Controle de Impressora Térmica Elgin i8/i9

Projeto escolar em linguagem C para controlar uma impressora térmica Elgin i8 ou i9 por meio da biblioteca `E1_Impressora01.dll`.

O programa possui login, menu de opções e comandos para imprimir textos, QR Code, código de barras e documentos SAT, além de acionar a gaveta de dinheiro e o sinal sonoro.

## Requisitos e ambiente
- Linguagem C e Windows.
- Dev-C++ configurado com compilador de 64 bits (ambiente definido pela equipe).
- Impressora Elgin i9 (ou i8, conforme configuração).
- DLL `E1_Impressora01.dll` x64, compatível com o compilador.
- Arquivos de teste `XMLSAT.xml` e `CANC_SAT.xml`.

## Login de teste
- Usuário: `admin`
- Senha: `1234`
- Máximo de três tentativas.

## Opções do menu

| Opção | Ação |
|---:|---|
| 1 | Configurar conexão |
| 2 | Abrir conexão |
| 3 | Fechar conexão |
| 4 | Imprimir texto |
| 5 | Imprimir QR Code |
| 6 | Imprimir código de barras |
| 7 | Imprimir XML SAT |
| 8 | Imprimir XML de cancelamento SAT |
| 9 | Abrir gaveta (comando Elgin) |
| 10 | Abrir gaveta (outro comando) |
| 11 | Emitir sinal sonoro |
| 0 | Sair |

As operações que usam a impressora verificam se a conexão está aberta antes de pedir dados ou enviar comandos.

## Arquivos na pasta de execução

```text
Projeto_Impressora_Elgin/
├── main.c
├── E1_Impressora01.dll
├── XMLSAT.xml
└── CANC_SAT.xml
```

A DLL x64 e os dois XMLs devem estar na pasta de execução. O arquivo DLL incluído no ZIP enviado foi identificado como x64. Não publique a DLL ou o ZIP da biblioteca no repositório público sem autorização para redistribuição.

## Compilar e executar

### Dev-C++
1. Abra ou crie um projeto de Console em C.
2. Use o arquivo `main.c` deste repositório.
3. Confirme que o compilador selecionado é de 64 bits.
4. Coloque a DLL x64 e os dois XMLs na pasta de execução do programa.
5. Compile e execute no Windows.
6. Para USB, configure: tipo `1`, modelo `i9`, conexão `USB`, parâmetro `0`.

### Terminal MinGW 64 bits
Na pasta que contém os arquivos:

```bash
gcc main.c -o impressora.exe
./impressora.exe
```

Não envie o executável gerado ao GitHub.

## Referência de implementação

A organização geral do fluxo e dos comandos também foi estudada a partir do projeto [sistema-impressora-elgin-c, de Lucas Libano](https://github.com/LucasLibano/sistema-impressora-elgin-c). A implementação deste grupo foi adaptada ao enunciado do professor e ao código-base fornecido; não é uma cópia literal. As diferenças e correções relevantes estão em [docs/REFERENCIAS.md](docs/REFERENCIAS.md).

## Testes

Consulte [docs/PLANO_DE_TESTES.md](docs/PLANO_DE_TESTES.md). Registre separadamente compilação e teste com a impressora física. Não marque testes como aprovados antes de executá-los.

## Documentação
- [Estrutura do projeto](docs/ESTRUTURA_DO_PROJETO.md)
- [Arquivos necessários](docs/ARQUIVOS_NECESSARIOS.md)
- [Como usar o repositório](docs/COMO_USAR_O_REPOSITORIO.md)
- [Plano de testes](docs/PLANO_DE_TESTES.md)
- [Referências e decisões de implementação](docs/REFERENCIAS.md)
- Enunciado: `Trabalho_C_Impressora.pdf` (adicionar em `docs/` se a divulgação for permitida).

## Autores
- Guilherme da Silva Damas
- Julia Mendonça da Silva
- Matheus Henrique Lima da Silva
- Mayara D'Assunção Silva

## Estado dos testes
A versão atual do código ainda precisa ser compilada no Dev-C++ de 64 bits e testada com a impressora física. Esses testes não devem ser considerados aprovados antes de sua execução e do registro dos resultados no plano de testes.
