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
    FILE *file = fopen(filename, "r");
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
    if (rename("temp_table.csv", filename) != 0)
    {
        perror("Erro ao substituir o arquivo original");
    }

    if (!table_deleted)
    {
        printf("A tabela '%s' não foi encontrada.\n", table_name);
    }
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
        printf("4. Sair\n");
        printf("Escolha uma opção (1-4): ");

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
                save_table(new_table, "table.csv");
                printf("Nova tabela criada e salva com sucesso.\n");
            }
            break;
        case 3:
            // Deletar uma tabela
            printf("Digite o nome da tabela que deseja deletar: ");
            char table_to_delete[50]; // Ajuste o tamanho conforme necessário
            scanf("%s", table_to_delete);
            delete_table(table_to_delete, "table.csv");
            break;
        case 4:
            // Sair do programa
            printf("Saindo do programa.\n");
            exit(0);
        default:
            // Opção inválida
            printf("Opção inválida. Tente novamente.\n");
        }
    }

    return 0;
}
