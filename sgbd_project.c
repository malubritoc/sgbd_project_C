#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura tabela
typedef struct
{
    char *name;
    char *primary_key;
    int columns;
    char **column_names;
} Table;

Table *create_table()
{
    Table *new_table = (Table *)malloc(sizeof(Table));

    if (!new_table)
    {
        // Falha na alocação de memória
        return NULL;
    }

    // Perguntar ao usuário pelo nome da tabela
    printf("Digite o nome da tabela: ");
    char table_name[50]; // Ajuste o tamanho conforme necessário
    scanf("%s", table_name);
    new_table->name = strdup(table_name);

    // Perguntar ao usuário pelo número de colunas
    printf("Digite o número de colunas: ");

    while (1) {
        if (scanf("%d", &new_table->columns) == 1 && new_table->columns > 0) {
            // Entrada bem-sucedida, sair do loop
            break;
        } else {
            // Limpar o buffer de entrada para evitar um loop infinito
            while (getchar() != '\n');
            // Solicitar ao usuário para inserir novamente
            printf("Entrada inválida. Digite o número de colunas novamente: ");
        }
    }

    // Alocar memória para os nomes das colunas
    new_table->column_names = (char **)malloc(new_table->columns * sizeof(char *));
    if (!new_table->column_names)
    {
        // Falha na alocação de memória
        free(new_table->name);
        free(new_table);
        return NULL;
    }

    // Perguntar ao usuário pelo nome da chave primária
    printf("Digite o nome da coluna 1 (chave primária): ");
    char primary_key[50]; // Ajuste o tamanho conforme necessário
    scanf("%s", primary_key);
    new_table->primary_key = strdup(primary_key);

    if (!new_table->primary_key)
    {
        // Falha na alocação de memória
        free(new_table->name);
        free(new_table->column_names);
        free(new_table);
        return NULL;
    }

    new_table->column_names[0] = strdup(new_table->primary_key); // A primeira coluna é a chave primária
    if (!new_table->column_names[0])
    {
        // Falha na alocação de memória
        free(new_table->name);
        free(new_table->primary_key);
        free(new_table->column_names);
        free(new_table);
        return NULL;
    }

    // Perguntar ao usuário pelos nomes das colunas restantes
    for (int i = 1; i < new_table->columns; ++i)
    {
        printf("Digite o nome da coluna %d: ", i + 1);
        char column_name[50]; // Ajuste o tamanho conforme necessário
        scanf("%s", column_name);
        new_table->column_names[i] = strdup(column_name);

        if (!new_table->column_names[i])
        {
            // Falha na alocação de memória
            for (int j = 1; j < i; ++j)
            {
                free(new_table->column_names[j]);
            }
            free(new_table->name);
            free(new_table->primary_key);
            free(new_table->column_names);
            free(new_table);
            return NULL;
        }
    }

    return new_table;
}

void save_table(Table *table, const char *filename)
{
    FILE *file = fopen(filename, "a");
    if (!file)
    {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    // Escrever os dados da tabela no arquivo em formato CSV
    fprintf(file, "%s,%s,%d", table->name, table->primary_key, table->columns);

    for (int i = 0; i < table->columns; ++i)
    {
        fprintf(file, ",%s", table->column_names[i]);
    }

    fprintf(file, "\n");

    // Fechar o arquivo
    fclose(file);
}

void free_table(Table *table)
{
    // Libere a memória alocada para a tabela
    if (table)
    {
        free(table->name);
        free(table->primary_key);
        for (int i = 0; i < table->columns; ++i)
        {
            free(table->column_names[i]);
        }
        free(table->column_names);
        free(table);
    }
}

Table *load_table(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Erro ao abrir o arquivo para leitura");
        return NULL;
    }

    // Alocar memória para a tabela
    Table *loaded_table = (Table *)malloc(sizeof(Table));
    if (!loaded_table)
    {
        perror("Erro na alocação de memória");
        fclose(file);
        return NULL;
    }

    // Ler os dados da tabela do arquivo em formato CSV
    if (fscanf(file, "%[^,],%[^,],%d", loaded_table->name, loaded_table->primary_key, &loaded_table->columns) != 3)
    {
        perror("Erro ao ler os dados da tabela");
        fclose(file);
        free_table(loaded_table);
        return NULL;
    }

    // Alocar memória para os nomes das colunas
    loaded_table->column_names = (char **)malloc(loaded_table->columns * sizeof(char *));
    if (!loaded_table->column_names)
    {
        perror("Erro na alocação de memória");
        fclose(file);
        free_table(loaded_table);
        return NULL;
    }

    for (int i = 0; i < loaded_table->columns; ++i)
    {
        loaded_table->column_names[i] = (char *)malloc(50); // Ajuste o tamanho conforme necessário
        if (fscanf(file, ",%[^,\n]", loaded_table->column_names[i]) != 1)
        {
            perror("Erro ao ler o nome da coluna");
            fclose(file);
            free_table(loaded_table);
            return NULL;
        }
    }

    // Fechar o arquivo
    fclose(file);

    return loaded_table;
}

int main()
{
    Table *my_table = create_table();

    if (!my_table)
    {
        fprintf(stderr, "Erro ao criar a tabela.\n");
        return 1;
    }

    save_table(my_table, "table.csv");

    return 0;
}
