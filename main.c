#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

/*
 * Controle da impressora Elgin i8/i9 em C.
 * A DLL e os arquivos XML devem estar na pasta de execucao.
 * A arquitetura da DLL (x86/x64) deve ser igual a do compilador.
 */

/* Assinaturas dos comandos exportados pela DLL. */
typedef int (WINAPI *FnAbreConexaoImpressora)(int, const char *, const char *, int);
typedef int (WINAPI *FnFechaConexaoImpressora)(void);
typedef int (WINAPI *FnImpressaoTexto)(const char *, int, int, int);
typedef int (WINAPI *FnCorte)(int);
typedef int (WINAPI *FnImpressaoQRCode)(const char *, int, int);
typedef int (WINAPI *FnImpressaoCodigoBarras)(int, const char *, int, int, int);
typedef int (WINAPI *FnAvancaPapel)(int);
typedef int (WINAPI *FnStatusImpressora)(int);
typedef int (WINAPI *FnAbreGavetaElgin)(int, int, int);
typedef int (WINAPI *FnAbreGaveta)(int, int, int);
typedef int (WINAPI *FnSinalSonoro)(int, int, int);
typedef int (WINAPI *FnModoPagina)(void);
typedef int (WINAPI *FnLimpaBufferModoPagina)(void);
typedef int (WINAPI *FnImprimeModoPagina)(void);
typedef int (WINAPI *FnModoPadrao)(void);
typedef int (WINAPI *FnPosicaoImpressaoHorizontal)(int);
typedef int (WINAPI *FnPosicaoImpressaoVertical)(int);
typedef int (WINAPI *FnImprimeXMLSAT)(const char *, int);
typedef int (WINAPI *FnImprimeXMLCancelamentoSAT)(const char *, const char *, int);

static FnAbreConexaoImpressora AbreConexaoImpressora;
static FnFechaConexaoImpressora FechaConexaoImpressora;
static FnImpressaoTexto ImpressaoTexto;
static FnCorte Corte;
static FnImpressaoQRCode ImpressaoQRCode;
static FnImpressaoCodigoBarras ImpressaoCodigoBarras;
static FnAvancaPapel AvancaPapel;
static FnStatusImpressora StatusImpressora;
static FnAbreGavetaElgin AbreGavetaElgin;
static FnAbreGaveta AbreGaveta;
static FnSinalSonoro SinalSonoro;
static FnModoPagina ModoPagina;
static FnLimpaBufferModoPagina LimpaBufferModoPagina;
static FnImprimeModoPagina ImprimeModoPagina;
static FnModoPadrao ModoPadrao;
static FnPosicaoImpressaoHorizontal PosicaoImpressaoHorizontal;
static FnPosicaoImpressaoVertical PosicaoImpressaoVertical;
static FnImprimeXMLSAT ImprimeXMLSAT;
static FnImprimeXMLCancelamentoSAT ImprimeXMLCancelamentoSAT;
static HMODULE dll = NULL;

static int conexaoAberta = 0;
static int tipo = 1;
static char modelo[50] = "i9";
static char conexao[50] = "USB";
static int parametro = 0;
static int entradaEncerrada = 0;

/* Chave QR Code correspondente ao XML de cancelamento fornecido. */
static const char *ASS_QRCODE =
    "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/"
    "q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+"
    "TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+"
    "ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";

static int carregaFuncao(void *destino, const char *nome) {
    FARPROC funcao = GetProcAddress(dll, nome);
    if (funcao == NULL) {
        printf("Comando '%s' nao encontrado na DLL.\n", nome);
        return 0;
    }
    memcpy(destino, &funcao, sizeof(funcao));
    return 1;
}

static int carregarDLL(void) {
    dll = LoadLibraryA("E1_Impressora01.dll");
    if (dll == NULL) {
        printf("Nao foi possivel carregar E1_Impressora01.dll (erro %lu).\n", GetLastError());
        printf("Confira a pasta de execucao e a arquitetura da DLL.\n");
        return 0;
    }

    int ok = 1;
    ok &= carregaFuncao(&AbreConexaoImpressora, "AbreConexaoImpressora");
    ok &= carregaFuncao(&FechaConexaoImpressora, "FechaConexaoImpressora");
    ok &= carregaFuncao(&ImpressaoTexto, "ImpressaoTexto");
    ok &= carregaFuncao(&Corte, "Corte");
    ok &= carregaFuncao(&ImpressaoQRCode, "ImpressaoQRCode");
    ok &= carregaFuncao(&ImpressaoCodigoBarras, "ImpressaoCodigoBarras");
    ok &= carregaFuncao(&AvancaPapel, "AvancaPapel");
    ok &= carregaFuncao(&StatusImpressora, "StatusImpressora");
    ok &= carregaFuncao(&AbreGavetaElgin, "AbreGavetaElgin");
    ok &= carregaFuncao(&AbreGaveta, "AbreGaveta");
    ok &= carregaFuncao(&SinalSonoro, "SinalSonoro");
    ok &= carregaFuncao(&ModoPagina, "ModoPagina");
    ok &= carregaFuncao(&LimpaBufferModoPagina, "LimpaBufferModoPagina");
    ok &= carregaFuncao(&ImprimeModoPagina, "ImprimeModoPagina");
    ok &= carregaFuncao(&ModoPadrao, "ModoPadrao");
    ok &= carregaFuncao(&PosicaoImpressaoHorizontal, "PosicaoImpressaoHorizontal");
    ok &= carregaFuncao(&PosicaoImpressaoVertical, "PosicaoImpressaoVertical");
    ok &= carregaFuncao(&ImprimeXMLSAT, "ImprimeXMLSAT");
    ok &= carregaFuncao(&ImprimeXMLCancelamentoSAT, "ImprimeXMLCancelamentoSAT");
    return ok;
}

static void capturarEntrada(const char *mensagem, char *destino, int tamanho) {
    int caractere;
    size_t comprimento;
    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        entradaEncerrada = 1;
        return;
    }
    comprimento = strlen(destino);
    if (comprimento > 0 && destino[comprimento - 1] != '\n' && !feof(stdin)) {
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
            /* Descarta o restante da linha para nao contaminar a proxima leitura. */
        }
        printf("Aviso: entrada muito longa; o excesso foi descartado.\n");
    }
    destino[strcspn(destino, "\r\n")] = '\0';
}

static int lerInteiro(const char *mensagem) {
    char linha[64];
    char *fim;
    long valor;
    while (!entradaEncerrada) {
        errno = 0;
        capturarEntrada(mensagem, linha, sizeof(linha));
        if (entradaEncerrada) return 0;
        valor = strtol(linha, &fim, 10);
        while (*fim != '\0' && isspace((unsigned char)*fim)) fim++;
        if (fim != linha && *fim == '\0' &&
            errno != ERANGE && valor >= INT_MIN && valor <= INT_MAX) return (int)valor;
        printf("Digite um numero inteiro valido!\n");
    }
    return 0;
}

static char *lerArquivo(const char *nome) {
    FILE *arquivo = fopen(nome, "rb");
    if (arquivo == NULL) return NULL;

    if (fseek(arquivo, 0, SEEK_END) != 0) {
        fclose(arquivo);
        return NULL;
    }
    long tamanho = ftell(arquivo);
    if (tamanho < 0 || fseek(arquivo, 0, SEEK_SET) != 0) {
        fclose(arquivo);
        return NULL;
    }

    char *dados = (char *)malloc((size_t)tamanho + 1);
    if (dados == NULL) {
        fclose(arquivo);
        return NULL;
    }

    size_t lidos = fread(dados, 1, (size_t)tamanho, arquivo);
    if (ferror(arquivo)) {
        free(dados);
        fclose(arquivo);
        return NULL;
    }
    dados[lidos] = '\0';
    fclose(arquivo);
    return dados;
}

void configurarConexao(void) {
    char novoModelo[50];
    if (conexaoAberta) {
        printf("Conexao ja esta aberta. Feche-a antes de alterar os parametros.\n");
        return;
    }
    do {
        tipo = lerInteiro("Tipo de conexao (1 USB, 2 Serial, 3 Rede, 4 Bluetooth): ");
        if (entradaEncerrada) return;
        if (tipo < 1 || tipo > 4) printf("Tipo invalido. Escolha de 1 a 4.\n");
    } while (tipo < 1 || tipo > 4);
    do {
        capturarEntrada("Modelo da impressora (i8/i9): ", novoModelo, sizeof(novoModelo));
        if (entradaEncerrada) return;
        if (strcmp(novoModelo, "i8") != 0 && strcmp(novoModelo, "i9") != 0)
            printf("Modelo invalido. Informe i8 ou i9.\n");
    } while (strcmp(novoModelo, "i8") != 0 && strcmp(novoModelo, "i9") != 0);
    strcpy(modelo, novoModelo);
    capturarEntrada("Porta/conexao (ex: USB): ", conexao, sizeof(conexao));
    if (entradaEncerrada) return;
    parametro = lerInteiro("Parametro adicional (0 para USB/Bluetooth): ");
    if (entradaEncerrada) return;
    if ((tipo == 1 || tipo == 4) && parametro != 0) {
        printf("Para USB ou Bluetooth, o parametro adicional sera ajustado para 0.\n");
        parametro = 0;
    }
    printf("Parametros configurados: tipo=%d, modelo=%s, conexao=%s, parametro=%d.\n",
           tipo, modelo, conexao, parametro);
}

void abrirConexao(void) {
    if (conexaoAberta) {
        printf("Conexao ja esta aberta.\n");
        return;
    }
    int retorno = AbreConexaoImpressora(tipo, modelo, conexao, parametro);
    if (retorno == 0) {
        conexaoAberta = 1;
        printf("Conexao aberta com sucesso.\n");
    } else {
        printf("Erro ao abrir conexao. Codigo: %d\n", retorno);
    }
}

void fecharConexao(void) {
    if (!conexaoAberta) {
        printf("A conexao ja esta fechada.\n");
        return;
    }
    int retorno = FechaConexaoImpressora();
    if (retorno == 0) {
        conexaoAberta = 0;
        printf("Conexao fechada com sucesso.\n");
    } else {
        printf("Erro ao fechar conexao. Codigo: %d\n", retorno);
    }
}

void imprimirTexto(void) {
    char texto[200];
    int copias, algumaCopia = 0;
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }
    capturarEntrada("Texto a imprimir: ", texto, sizeof(texto));
    if (entradaEncerrada || texto[0] == '\0') {
        printf("Nenhum texto informado.\n");
        return;
    }
    copias = lerInteiro("Quantas copias? ");
    if (entradaEncerrada) return;
    if (copias <= 0) {
        printf("A quantidade de copias deve ser maior que zero.\n");
        return;
    }
    for (int i = 1; i <= copias; i++) {
        int retorno = ImpressaoTexto(texto, 1, 4, 0);
        if (retorno >= 0) {
            algumaCopia = 1;
            printf("Copia %d enviada para impressao.\n", i);
        } else printf("Erro ao imprimir texto. Codigo: %d\n", retorno);
    }
    if (algumaCopia) {
        int corte = Corte(5);
        if (corte == 0) printf("Corte realizado.\n");
        else printf("Erro ao cortar papel. Codigo: %d\n", corte);
    }
}

void imprimirQRCode(void) {
    char dados[500];
    int retorno, corte;
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }
    capturarEntrada("Conteudo do QR Code: ", dados, sizeof(dados));
    if (entradaEncerrada || dados[0] == '\0') {
        printf("Nenhum conteudo informado.\n");
        return;
    }
    retorno = ImpressaoQRCode(dados, 6, 4);
    if (retorno == 0) {
        printf("QR Code enviado para impressao.\n");
        corte = Corte(5);
        if (corte == 0) printf("Corte realizado.\n");
        else printf("Erro ao cortar papel. Codigo: %d\n", corte);
    } else printf("Erro ao imprimir QR Code. Codigo: %d\n", retorno);
}

void imprimirCodigoBarras(void) {
    int retorno, corte;
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }
    retorno = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    if (retorno == 0) {
        printf("Codigo de barras enviado para impressao.\n");
        corte = Corte(5);
        if (corte == 0) printf("Corte realizado.\n");
        else printf("Erro ao cortar papel. Codigo: %d\n", corte);
    } else printf("Erro ao imprimir codigo de barras. Codigo: %d\n", retorno);
}

void imprimirXMLSAT(void) {
    char *dadosXML;
    int retorno, corte;
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }
    dadosXML = lerArquivo("XMLSAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler XMLSAT.xml. Confira a pasta de execucao.\n");
        return;
    }
    retorno = ImprimeXMLSAT(dadosXML, 0);
    free(dadosXML);
    if (retorno == 0) {
        printf("XML SAT enviado para impressao.\n");
        corte = Corte(5);
        if (corte == 0) printf("Corte realizado.\n");
        else printf("Erro ao cortar papel. Codigo: %d\n", corte);
    } else printf("Erro ao imprimir XML SAT. Codigo: %d\n", retorno);
}

void imprimirXMLCancelamentoSAT(void) {
    char *dadosXML;
    int retorno, corte;
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }
    dadosXML = lerArquivo("CANC_SAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler CANC_SAT.xml. Confira a pasta de execucao.\n");
        return;
    }
    retorno = ImprimeXMLCancelamentoSAT(dadosXML, ASS_QRCODE, 0);
    free(dadosXML);
    if (retorno == 0) {
        printf("XML de cancelamento SAT enviado para impressao.\n");
        corte = Corte(5);
        if (corte == 0) printf("Corte realizado.\n");
        else printf("Erro ao cortar papel. Codigo: %d\n", corte);
    } else printf("Erro ao imprimir XML de cancelamento SAT. Codigo: %d\n", retorno);
}

void abrirGavetaElgin(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }
    int retorno = AbreGavetaElgin(1, 50, 50);
    if (retorno == 0) printf("Comando da gaveta Elgin enviado com sucesso.\n");
    else printf("Erro ao abrir gaveta Elgin. Codigo: %d\n", retorno);
}

void abrirGaveta(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }
    int retorno = AbreGaveta(1, 5, 10);
    if (retorno == 0) printf("Comando de abertura da gaveta enviado com sucesso.\n");
    else printf("Erro ao abrir gaveta. Codigo: %d\n", retorno);
}

void emitirSinalSonoro(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }
    int retorno = SinalSonoro(4, 50, 5);
    if (retorno == 0) printf("Sinal sonoro enviado com sucesso.\n");
    else printf("Erro ao emitir sinal sonoro. Codigo: %d\n", retorno);
}

int fazerLogin(void) {
    char usuario[50], senha[50];
    for (int tentativa = 1; tentativa <= 3; tentativa++) {
        printf("\n--- LOGIN (%d de 3) ---\n", tentativa);
        capturarEntrada("Usuario: ", usuario, sizeof(usuario));
        capturarEntrada("Senha: ", senha, sizeof(senha));
        if (strcmp(usuario, "admin") == 0 && strcmp(senha, "1234") == 0) {
            printf("Login realizado com sucesso.\n");
            return 1;
        }
        printf("Usuario ou senha incorretos. Tentativas restantes: %d\n", 3 - tentativa);
    }
    printf("Numero maximo de tentativas atingido.\n");
    return 0;
}

void exibirMenu(void) {
    printf("\n*************** MENU IMPRESSORA *****************\n");
    printf("1  - Configurar conexao\n");
    printf("2  - Abrir conexao\n");
    printf("3  - Fechar conexao\n");
    printf("4  - Imprimir texto\n");
    printf("5  - Imprimir QR Code\n");
    printf("6  - Imprimir codigo de barras\n");
    printf("7  - Imprimir XML SAT\n");
    printf("8  - Imprimir XML de cancelamento SAT\n");
    printf("9  - Abrir gaveta (Elgin)\n");
    printf("10 - Abrir gaveta\n");
    printf("11 - Sinal sonoro\n");
    printf("0  - Sair\n");
}

int main(void) {
    if (!carregarDLL()) {
        printf("Programa encerrado.\n");
        if (dll != NULL) FreeLibrary(dll);
        return 1;
    }

    if (!fazerLogin()) {
        printf("Acesso negado.\n");
        FreeLibrary(dll);
        return 1;
    }

    int opcao = -1;
    while (opcao != 0) {
        exibirMenu();
        opcao = lerInteiro("\nDigite a opcao desejada: ");
        if (opcao == 1) configurarConexao();
        else if (opcao == 2) abrirConexao();
        else if (opcao == 3) fecharConexao();
        else if (opcao == 4) imprimirTexto();
        else if (opcao == 5) imprimirQRCode();
        else if (opcao == 6) imprimirCodigoBarras();
        else if (opcao == 7) imprimirXMLSAT();
        else if (opcao == 8) imprimirXMLCancelamentoSAT();
        else if (opcao == 9) abrirGavetaElgin();
        else if (opcao == 10) abrirGaveta();
        else if (opcao == 11) emitirSinalSonoro();
        else if (opcao == 0) {
            fecharConexao();
            printf("Encerrando o programa.\n");
        } else {
            printf("Opcao invalida. Escolha de 0 a 11.\n");
        }
    }

    FreeLibrary(dll);
    return 0;
}
