#include <stdio.h>
#include <stdlib.h>


typedef enum ESTADO {
    E_LIVRE,
    E_OCUPADO,
    E_APAGADO
} ESTADO;

typedef struct ELEM { // Elemento
    int chave;
    int valor;
    ESTADO estado;
} ELEM;

typedef struct THEA { // Tabela hash com endereçamento aberto
    int m;
    ELEM* TH; // Tabela hash
} THEA;

int THEA_Hash(THEA* TH, int chave, int k);
THEA* THEA_Criar(int m);
int THEA_Inserir(THEA *TH, int chave, int valor);
int THEA_Buscar(THEA *TH, int chave);
void THEA_Remover(THEA* TH, int chave);
void THEA_Destruir(THEA* TH);

// Conteúdo de tabelahash_ea.c
int THEA_Hash(THEA* TH, int chave, int k) {
    return ((chave % TH->m) + k) % TH->m;
}

THEA* THEA_Criar(int m) {
    THEA* novo;
    int i;

    novo = malloc(sizeof(THEA));
    if (novo == NULL) {
        perror("Falha ao alocar memória para THEA");
        exit(EXIT_FAILURE);
    }

    novo->TH = malloc(sizeof(ELEM) * m);
    if (novo->TH == NULL) {
        perror("Falha ao alocar memória para elementos da tabela hash");
        free(novo);
        exit(EXIT_FAILURE);
    }
    novo->m = m;

    for(i = 0; i < m; i++) {
        novo->TH[i].estado = E_LIVRE;
        novo->TH[i].chave = -1; // Inicializando chave com valor inválido
        novo->TH[i].valor = -1; // Inicializando valor com valor inválido
    }

    return novo;
}

int THEA_Inserir(THEA *TH, int chave, int valor) {
    int h, k, h_inicial;
    k = 0;
    h = THEA_Hash(TH, chave, k);
    h_inicial = h;

    while(TH->TH[h].estado == E_OCUPADO) {
        if(TH->TH[h].chave == chave) {
            break;
        }

        k++;
        h = THEA_Hash(TH, chave, k);

        if(h == h_inicial) {
            return -1;
        }
    }
    TH->TH[h].chave = chave;
    TH->TH[h].valor = valor;
    TH->TH[h].estado = E_OCUPADO;

    return h;
}

int THEA_Buscar(THEA *TH, int chave) {
    int h, k, h_inicial;

    k = 0;
    h = THEA_Hash(TH, chave, k);
    h_inicial = h;

    while(TH->TH[h].estado != E_LIVRE) {
        if((TH->TH[h].estado == E_OCUPADO) && (TH->TH[h].chave == chave)) {
            return h;
        }

        k++;
        h = THEA_Hash(TH, chave, k);

        if(h_inicial == h) {
            return -1;
        }
    }
    return -1;
}

void THEA_Remover(THEA *TH, int chave) {
    int p;
    p = THEA_Buscar(TH, chave);
    if(p != -1) {
        TH->TH[p].estado = E_APAGADO;
    }
}

// Função para liberar a memória alocada para a tabela hash
void THEA_Destruir(THEA* TH) {
    free(TH->TH);
    free(TH);
}

// Conteúdo de main.c
int main() {
    THEA* t;
    t = THEA_Criar(10);

    if (THEA_Inserir(t, 10, 10) == -1) printf("Falha ao inserir chave 10\n");
    if (THEA_Inserir(t, 21, 21) == -1) printf("Falha ao inserir chave 21\n");
    if (THEA_Inserir(t, 44, 44) == -1) printf("Falha ao inserir chave 44\n");
    if (THEA_Inserir(t, 32, 32) == -1) printf("Falha ao inserir chave 32\n");
    if (THEA_Inserir(t, 12, 12) == -1) printf("Falha ao inserir chave 12\n");
    if (THEA_Inserir(t, 37, 37) == -1) printf("Falha ao inserir chave 37\n");
    if (THEA_Inserir(t, 43, 43) == -1) printf("Falha ao inserir chave 43\n");

    printf("------------------------------\n");

    int i;
    for(i = 0; i < t->m; i++) {
        const char e = t->TH[i].estado == E_OCUPADO ? 'O'
            : (t->TH[i].estado == E_LIVRE ? 'L' : 'A');
        printf("%d: %d, %c\n", i, t->TH[i].chave, e);
    }

    printf("3? %d\n", THEA_Buscar(t, 3));
    printf("10? %d\n", THEA_Buscar(t, 10));
    printf("7? %d\n", THEA_Buscar(t, 7));
    printf("37? %d\n", THEA_Buscar(t, 37));
    printf("43? %d\n", THEA_Buscar(t, 43));
    printf("44? %d\n", THEA_Buscar(t, 44));

    printf("------------------------------\n");

    THEA_Remover(t, 44);
    printf("------------------------------\n");

    printf("44? %d\n", THEA_Buscar(t, 44));
    printf("43? %d\n", THEA_Buscar(t, 43));
    printf("------------------------------\n");

    // Liberação da memória alocada
    THEA_Destruir(t);

    return 0;
}
