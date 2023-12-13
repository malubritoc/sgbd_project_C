#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura da linha
typedef struct Row
{
    char **values;
    struct Row *next;
} Row;

// Estrutura tabela
typedef struct
{
    char *name;
    char *primary_key;
    int columns;
    char **column_names;
    Row *rows;
} Table;

// Função auxiliar para liberar a memória alocada para uma linha
void free_row(Row *row, int columns)
{
    if (row)
    {
        for (int i = 0; i < columns; ++i)
        {
            free(row->values[i]);
        }
        free(row->values);
        free(row);
    }
}

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

    while (1)
    {
        if (scanf("%d", &new_table->columns) == 1 && new_table->columns > 0)
        {
            // Entrada bem-sucedida, sair do loop
            break;
        }
        else
        {
            // Limpar o buffer de entrada para evitar um loop infinito
            while (getchar() != '\n')
                ;
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

    // Inicializar a lista de linhas
    new_table->rows = NULL;

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

    FILE *file_all = fopen("table.csv", "a");
    if (!file_all)
    {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

     // Escrever os dados da tabela no arquivo em formato CSV
    fprintf(file_all, "%s,%s,%d", table->name, table->primary_key, table->columns);

    for (int i = 0; i < table->columns; ++i)
    {
        fprintf(file_all, ",%s", table->column_names[i]);
    }

    fprintf(file_all, "\n");

        // Fechar o arquivo
    fclose(file_all);
}

// Função para liberar a memória alocada para uma tabela
void free_table(Table *table)
{
    if (table)
    {
        free(table->name);
        free(table->primary_key);
        free(table->column_names);

        // Liberar a memória alocada para as linhas
        Row *current_row = table->rows;
        while (current_row != NULL)
        {
            Row *next_row = current_row->next;
            free_row(current_row, table->columns);
            current_row = next_row;
        }

        free(table);
    }
}

Table *load_table(const char *filename)
{
    printf("leu");
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

void list_tables(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Erro ao abrir o arquivo para leitura");
        return;
    }

    char line[256]; // Ajuste o tamanho conforme necessário

    printf("Tabelas cadastradas:\n");

    // Lê cada linha do arquivo e imprime o nome da tabela
    while (fgets(line, sizeof(line), file))
    {
        char table_name[50];                 // Ajuste o tamanho conforme necessário
        sscanf(line, "%49[^,]", table_name); // Lê o nome da tabela até a primeira vírgula
        printf("%s\n", table_name);
    }

    // Fechar o arquivo
    fclose(file);
}

void delete_table(const char *table_name, const char *filename)
{
    FILE *file = fopen("table.csv", "r");
    if (!file)
    {
        perror("Erro ao abrir o arquivo para leitura");
        return;
    }

    // Criar um arquivo temporário
    FILE *temp_file = fopen("temp_table.csv", "w");
    if (!temp_file)
    {
        perror("Erro ao criar o arquivo temporário");
        fclose(file);
        return;
    }

    char line[256];
    int table_deleted = 0;

    // Ler e copiar as tabelas para o arquivo temporário
    while (fgets(line, sizeof(line), file))
    {
        char table_name_in_file[50];
        sscanf(line, "%49[^,],", table_name_in_file);

        if (strcmp(table_name_in_file, table_name) != 0)
        {
            // A tabela não deve ser excluída, copiar para o arquivo temporário
            fprintf(temp_file, "%s", line);
        }
        else
        {
            // A tabela foi excluída
            table_deleted = 1;
            printf("Tabela '%s' apagada com sucesso.\n", table_name);
        }
    }

    fclose(file);
    fclose(temp_file);

    // Substituir o arquivo original pelo arquivo temporário
    if (rename("temp_table.csv", "table.csv") != 0)
    {
        perror("Erro ao substituir o arquivo original");
    }

    if (!table_deleted)
    {
        printf("A tabela '%s' não foi encontrada.\n", table_name);
    }

    // Use a função remove para apagar o arquivo
    if (remove(filename) == 0) {
        printf("Arquivo '%s' removido com sucesso.\n", filename);
    } else {
        perror("Erro ao remover o arquivo");
    }

}

// Função para adicionar uma nova linha à tabela
void add_row(Table *table)
{
    // Alocar memória para os valores das colunas
    char **row_values = (char **)malloc(table->columns * sizeof(char *));
    if (!row_values)
    {
        fprintf(stderr, "Erro na alocação de memória.\n");
        return;
    }

    // Preencher os valores das colunas
    for (int i = 0; i < table->columns; ++i)
    {
        printf("Digite o valor para %s: ", table->column_names[i]);
        row_values[i] = (char *)malloc(50); // Ajuste o tamanho conforme necessário
        scanf("%s", row_values[i]);
    }

    // Criar uma nova linha (Row) e configurar seus valores
    Row *new_row = (Row *)malloc(sizeof(Row));
    if (!new_row)
    {
        fprintf(stderr, "Erro na alocação de memória para a nova linha.\n");
        for (int i = 0; i < table->columns; ++i)
        {
            free(row_values[i]);
        }
        free(row_values);
        return;
    }

    new_row->values = row_values;

    // Adicionar a nova linha à tabela
    if (table->rows == NULL)
    {
        // Se a lista de linhas estiver vazia, a nova linha será a primeira
        table->rows = new_row;
    }
    else
    {
        // Encontrar o final da lista de linhas e adicionar a nova linha
        Row *last_row = table->rows;
        while (last_row->next != NULL)
        {
            last_row = last_row->next;
        }
        last_row->next = new_row;
    }

    // Liberar a memória alocada para a nova linha
    free_row(new_row, table->columns);
}

int main()
{
    int choice;

    // Loop principal do programa
    while (1)
    {
        // Exibe o menu
        printf("\nMenu:\n");
        printf("1. Listar tabelas\n");
        printf("2. Criar uma nova tabela\n");
        printf("3. Deletar uma tabela\n");
        printf("4. Adicionar linha a uma tabela\n"); // Nova opção para adicionar uma linha
        printf("5. Sair\n");
        printf("Escolha uma opção (1-5): ");

        // Obtém a escolha do usuário
        if (scanf("%d", &choice) != 1)
        {
            // Limpa o buffer de entrada em caso de entrada inválida
            scanf("%*s");
            printf("Opção inválida. Tente novamente.\n");
            continue;
        }

        // Declaração da variável new_table aqui
        Table *new_table;

        // Executa a opção escolhida
        switch (choice)
        {
        case 1:
            // Listar tabelas
            list_tables("table.csv");
            break;
        case 2:
            // Criar uma nova tabela
            new_table = create_table();
            if (new_table)
            {
                // Salvar tabela no arquivo
                save_table(new_table, strcat(new_table->name, ".csv"));
                printf("Nova tabela criada e salva com sucesso.\n");
            }
            break;
        case 3:
            // Deletar uma tabela
            printf("Digite o nome da tabela que deseja deletar: ");
            char table_to_delete[50];
            scanf("%s", table_to_delete);
            delete_table(table_to_delete, strcat(table_to_delete, ".csv"));
            break;
        case 4:
            // Adicionar linha a uma tabela
            printf("Digite o nome da tabela à qual deseja adicionar uma linha: ");
            char table_name_to_add_row[50];
            scanf("%s", table_name_to_add_row);

            // Carregar a tabela
            Table *table_to_add_row = load_table(table_name_to_add_row);
            if (table_to_add_row)
            {
                // Adicionar a linha à tabela
                add_row(table_to_add_row);
                // Salvar a tabela de volta ao arquivo
                save_table(table_to_add_row, strcat(table_to_add_row->name, ".csv"));
                // Liberar a memória alocada para a tabela
                free_table(table_to_add_row);
            }
            break;
        default:
            // Opção inválida
            printf("Opção inválida. Tente novamente.\n");
        }
    }

    return 0;
}
