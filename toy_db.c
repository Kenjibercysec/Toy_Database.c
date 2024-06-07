#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_TABLE_SIZE 10
#define MAX_CLASS_NAME 100
#define MAX_FIELD_NAME 100
#define MAX_FIELD_COUNT 10

typedef enum {
    E_LIVRE,
    E_OCUPADO,
    E_APAGADO
} ESTADO;

typedef struct Field {
    char name[MAX_FIELD_NAME];
    char type[MAX_FIELD_NAME];
} Field;

typedef struct Class {
    char name[MAX_CLASS_NAME];
    Field fields[MAX_FIELD_COUNT];
    int field_count;
    struct Class *next;
} Class;

typedef struct Object {
    int id;
    void **field_values;
    Class *class;
    struct Object *next;
} Object;

typedef struct ELEM {
    int chave;
    Object *valor;
    ESTADO estado;
    struct ELEM *prox;
} ELEM;

typedef struct {
    ELEM **table;
    int size;
    int count;
} HashTable;

Class *classList = NULL;

// Funções hash
int h1(int key, int size) {
    return key % size;
}

// Criar uma nova tabela hash
HashTable* createTable(int size) {
    HashTable *newTable = (HashTable*)malloc(sizeof(HashTable));
    newTable->table = (ELEM**)malloc(sizeof(ELEM*) * size);
    for (int i = 0; i < size; i++) {
        newTable->table[i] = NULL;
    }
    newTable->size = size;
    newTable->count = 0;
    return newTable;
}

// Criar um novo elemento
ELEM* createElement(int chave, Object *valor) {
    ELEM *newElem = (ELEM*)malloc(sizeof(ELEM));
    newElem->chave = chave;
    newElem->valor = valor;
    newElem->estado = E_OCUPADO;
    newElem->prox = NULL;
    return newElem;
}

// Função para limpar a tela
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Inserir um item na tabela hash
void insert(HashTable *table, int key, Object *value) {
    int idx = h1(key, table->size);
    ELEM *newElem = createElement(key, value);

    if (table->table[idx] == NULL) {
        table->table[idx] = newElem;
    } else {
        ELEM *current = table->table[idx];
        while (current->prox != NULL) {
            current = current->prox;
        }
        current->prox = newElem;
    }
    table->count++;

    if (table->count > table->size * 0.7) {
        printf("Redimensionando a tabela hash...\n");
        int newSize = table->size * 2;
        HashTable *newTable = createTable(newSize);

        for (int i = 0; i < table->size; i++) {
            ELEM *current = table->table[i];
            while (current != NULL) {
                insert(newTable, current->chave, current->valor);
                current = current->prox;
            }
        }
        free(table->table);
        *table = *newTable;
    }
}

// Buscar um item na tabela hash
Object* search(HashTable *table, int key) {
    int idx = h1(key, table->size);
    ELEM *current = table->table[idx];

    while (current != NULL) {
        if (current->chave == key && current->estado == E_OCUPADO) {
            return current->valor;
        }
        current = current->prox;
    }
    return NULL;
}

// Excluir um item da tabela hash
void delete(HashTable *table, int key) {
    int idx = h1(key, table->size);
    ELEM *current = table->table[idx];
    ELEM *prev = NULL;

    while (current != NULL) {
        if (current->chave == key && current->estado == E_OCUPADO) {
            if (prev == NULL) {
                table->table[idx] = current->prox;
            } else {
                prev->prox = current->prox;
            }
            current->estado = E_APAGADO;
            table->count--;
            free(current);
            return;
        }
        prev = current;
        current = current->prox;
    }
}

void display(HashTable *table) {
    printf("Tabela Hash:\n");
    for (int i = 0; i < table->size; i++) {
        ELEM *current = table->table[i];
        if (current == NULL) {
            printf("Posição %d: Vazio\n", i);
        } else {
            printf("Posição %d:\n", i);
            while (current != NULL) {
                printf("  Chave = %d, Objeto ID = %d\n", current->chave, current->valor->id);
                current = current->prox;
            }
        }
    }
}

// Criar uma nova classe
Class* createClass(const char *name) {
    Class *newClass = (Class*)malloc(sizeof(Class));
    strcpy(newClass->name, name);
    newClass->field_count = 0;
    newClass->next = classList;
    classList = newClass;
    return newClass;
}

// Adicionar um campo à classe
void addField(Class *class, const char *name, const char *type) {
    if (class->field_count < MAX_FIELD_COUNT) {
        strcpy(class->fields[class->field_count].name, name);
        strcpy(class->fields[class->field_count].type, type);
        class->field_count++;
    } else {
        printf("Número máximo de campos atingido.\n");
    }
}

// Criar um novo objeto
Object* createObject(Class *class, int id) {
    Object *newObject = (Object*)malloc(sizeof(Object));
    newObject->id = id;
    newObject->class = class;
    newObject->field_values = (void**)malloc(sizeof(void*) * class->field_count);
    for (int i = 0; i < class->field_count; i++) {
        newObject->field_values[i] = NULL;
    }
    newObject->next = NULL;
    return newObject;
}

// Definir um valor de campo para um objeto
void setFieldValue(Object *object, const char *field_name, void *value) {
    for (int i = 0; i < object->class->field_count; i++) {
        if (strcmp(object->class->fields[i].name, field_name) == 0) {
            object->field_values[i] = value;
            return;
        }
    }
    printf("Campo não encontrado.\n");
}

void printObject(Object *object) {
    printf("Objeto ID: %d\n", object->id);
    for (int i = 0; i < object->class->field_count; i++) {
        if (strcmp(object->class->fields[i].type, "int") == 0) {
            printf("%s: %d\n", object->class->fields[i].name, *(int*)object->field_values[i]);
        } else if (strcmp(object->class->fields[i].type, "string") == 0) {
            printf("%s: %s\n", object->class->fields[i].name, (char*)object->field_values[i]);
        }
    }
}

Class* findClass(const char *name) {
    Class *current = classList;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int main() {
    HashTable *table = createTable(INITIAL_TABLE_SIZE);
    int choice, key, int_value;
    char class_name[MAX_CLASS_NAME], field_name[MAX_FIELD_NAME], field_type[MAX_FIELD_NAME], string_value[MAX_FIELD_NAME];
    Class *class;
    Object *object;

    while (1) {
        printf("\nMenu:\n");
        printf("1. Criar Classe\n");
        printf("2. Adicionar Campo à Classe\n");
        printf("3. Criar Objeto\n");
        printf("4. Definir Valor de Campo\n");
        printf("5. Buscar Objeto\n");
        printf("6. Excluir Objeto\n");
        printf("7. Exibir Tabela\n");
        printf("8. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Digite o nome da classe: ");
                scanf("%s", class_name);
                createClass(class_name);
                break;
            case 2:
                printf("Digite o nome da classe: ");
                scanf("%s", class_name);
                class = findClass(class_name);
                if (class != NULL) {
                    printf("Digite o nome do campo: ");
                    scanf("%s", field_name);
                    printf("Digite o tipo do campo (int/string): ");
                    scanf("%s", field_type);
                    addField(class, field_name, field_type);
                } else {
                    printf("Classe não encontrada.\n");
                }
                break;
            case 3:
                printf("Digite o nome da classe: ");
                scanf("%s", class_name);
                class = findClass(class_name);
                if (class != NULL) {
                    printf("Digite o ID do objeto: ");
                    scanf("%d", &key);
                    object = createObject(class, key);
                    insert(table, key, object);
                } else {
                    printf("Classe não encontrada.\n");
                }
                break;
            case 4:
                printf("Digite a chave do objeto: ");
                scanf("%d", &key);
                object = search(table, key);
                if (object != NULL) {
                    printf("Digite o nome do campo: ");
                    scanf("%s", field_name);
                    printf("Digite o valor do campo: ");
                    for (int i = 0; i < object->class->field_count; i++) {
                        if (strcmp(object->class->fields[i].name, field_name) == 0) {
                            if (strcmp(object->class->fields[i].type, "int") == 0) {
                                scanf("%d", &int_value);
                                int *p = (int*)malloc(sizeof(int));
                                *p = int_value;
                                setFieldValue(object, field_name, p);
                            } else if (strcmp(object->class->fields[i].type, "string") == 0) {
                                scanf("%s", string_value);
                                char *p = (char*)malloc(strlen(string_value) + 1);
                                strcpy(p, string_value);
                                setFieldValue(object, field_name, p);
                            }
                        }
                    }
                } else {
                    printf("Objeto não encontrado.\n");
                }
                break;
            case 5:
                printf("Digite a chave do objeto: ");
                scanf("%d", &key);
                object = search(table, key);
                if (object != NULL) {
                    printObject(object);
                } else {
                    printf("Objeto não encontrado.\n");
                }
                break;
            case 6:
                printf("Digite a chave do objeto: ");
                scanf("%d", &key);
                delete(table, key);
                break;
            case 7:
                display(table);
                break;
            case 8:
                exit(0);
            default:
                printf("Opção inválida!\n");
        }

        printf("Pressione Enter para continuar...");
        getchar(); // Captura o newline do scanf
        getchar(); // Espera pelo Enter
        clearScreen();
    }

    return 0;
}
