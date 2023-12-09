#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura tabela
typedef struct {
    char* name;
    char* primary_key_name;
    int* primary_key;
    int columns;
    char** column_names;
} Table;

// Função de conversão do nome da tabela para número inteiro
int getPrimaryKey(const char *str) {
    unsigned int hash = 0;
    // Usando o size_t para garantir que o código gerado é positivo
    size_t length = strlen(str);

    for (size_t i = 0; i < length; ++i) {
        hash = hash * 31 + str[i];
    }

    return hash;
    // Nesse caso, duas palavras iguais, gerariam dois códigos iguais então é necessário fazer essa validação
};


Table* create_table(const char* table_name, const char* primary_key_name, int num_columns, char** column_names) {

}

int main() {
    const char *inputString = "exemplo";
    unsigned int hashCode = getPrimaryKey(inputString);

    printf("String: %s\n", inputString);
    printf("Hash Code: %u\n", hashCode);

    return 0;

};
