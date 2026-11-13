
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

sqlite3 *db;

// Limpar buffer
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Ler texto
void lerTexto(char *texto, int tamanho) {
    if (fgets(texto, tamanho, stdin)) {
        texto[strcspn(texto, "\n")] = '\0';
    }
}

// Conectar ao banco
int conectarBanco() {
    int resultado = sqlite3_open("alunos.db", &db);

    if (resultado != SQLITE_OK) {
        printf("Erro ao conectar: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    printf("Banco conectado com sucesso!\n");
    return 1;
}

// Criar tabela
void criarTabela() {
    char *mensagem = NULL;

    const char *sql =
        "CREATE TABLE IF NOT EXISTS alunos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "primeiroNome TEXT NOT NULL,"
        "sobrenome TEXT NOT NULL,"
        "sexo TEXT NOT NULL,"
        "telefone TEXT NOT NULL,"
        "endereco TEXT NOT NULL"
        ");";

    if (sqlite3_exec(db, sql, NULL, NULL, &mensagem) != SQLITE_OK) {
        printf("Erro: %s\n", mensagem);
        sqlite3_free(mensagem);
    }
}

// Adicionar aluno
void adicionarAluno() {
    sqlite3_stmt *stmt;

    char nome[100], sobrenome[100];
    char sexo[20], telefone[100], endereco[100];

    printf("\n===== CADASTRAR ALUNO =====\n");

    printf("Primeiro nome: ");
    lerTexto(nome, sizeof(nome));

    printf("Sobrenome: ");
    lerTexto(sobrenome, sizeof(sobrenome));

    printf("Sexo: ");
    lerTexto(sexo, sizeof(sexo));

    printf("Telefone: ");
    lerTexto(telefone, sizeof(telefone));

    printf("Endereco: ");
    lerTexto(endereco, sizeof(endereco));

    if (strlen(nome) == 0 || strlen(sobrenome) == 0 ||
        strlen(sexo) == 0 || strlen(telefone) == 0 ||
        strlen(endereco) == 0) {
        printf("\nPreencha todos os campos!\n");
        return;
    }

    const char *sql =
        "INSERT INTO alunos "
        "(primeiroNome, sobrenome, sexo, telefone, endereco) "
        "VALUES (?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar cadastro!\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, sobrenome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, sexo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, telefone, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, endereco, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nAluno cadastrado com sucesso!\n");
        printf("ID: %lld\n",
               (long long)sqlite3_last_insert_rowid(db));
    } else {
        printf("Erro ao cadastrar aluno!\n");
    }

    sqlite3_finalize(stmt);
}

// Listar alunos
void listarAlunos() {
    sqlite3_stmt *stmt;

    const char *sql = "SELECT * FROM alunos ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar alunos!\n");
        return;
    }

    printf("\n========== LISTA DE ALUNOS ==========\n");

    printf("%-5s %-15s %-15s %-12s %-15s %s\n",
           "ID", "Nome", "Sobrenome", "Sexo",
           "Telefone", "Endereco");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-5d %-15s %-15s %-12s %-15s %s\n",
            sqlite3_column_int(stmt, 0),
            (const char *)sqlite3_column_text(stmt, 1),
            (const char *)sqlite3_column_text(stmt, 2),
            (const char *)sqlite3_column_text(stmt, 3),
            (const char *)sqlite3_column_text(stmt, 4),
            (const char *)sqlite3_column_text(stmt, 5));
    }

    sqlite3_finalize(stmt);
}

// Pesquisar aluno
void pesquisarAluno() {
    sqlite3_stmt *stmt;
    int id;

    printf("\nDigite o ID do aluno: ");

    if (scanf("%d", &id) != 1) {
        limparBuffer();
        printf("ID invalido!\n");
        return;
    }

    limparBuffer();

    const char *sql = "SELECT * FROM alunos WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro na pesquisa!\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("\nAluno encontrado!\n");

        printf("ID: %d\n", sqlite3_column_int(stmt, 0));
        printf("Nome: %s\n", sqlite3_column_text(stmt, 1));
        printf("Sobrenome: %s\n", sqlite3_column_text(stmt, 2));
        printf("Sexo: %s\n", sqlite3_column_text(stmt, 3));
        printf("Telefone: %s\n", sqlite3_column_text(stmt, 4));
        printf("Endereco: %s\n", sqlite3_column_text(stmt, 5));
    } else {
        printf("\nAluno nao encontrado!\n");
    }

    sqlite3_finalize(stmt);
}

// Editar aluno
void editarAluno() {
    sqlite3_stmt *stmt;
    int id;

    char nome[100], sobrenome[100];
    char sexo[20], telefone[100], endereco[100];

    printf("\nDigite o ID do aluno: ");

    if (scanf("%d", &id) != 1) {
        limparBuffer();
        printf("ID invalido!\n");
        return;
    }

    limparBuffer();

    printf("Novo nome: ");
    lerTexto(nome, sizeof(nome));

    printf("Novo sobrenome: ");
    lerTexto(sobrenome, sizeof(sobrenome));

    printf("Novo sexo: ");
    lerTexto(sexo, sizeof(sexo));

    printf("Novo telefone: ");
    lerTexto(telefone, sizeof(telefone));

    printf("Novo endereco: ");
    lerTexto(endereco, sizeof(endereco));

    const char *sql =
        "UPDATE alunos SET primeiroNome=?, sobrenome=?, "
        "sexo=?, telefone=?, endereco=? WHERE id=?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar edicao!\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, sobrenome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, sexo, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, telefone, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, endereco, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0)
            printf("\nAluno atualizado!\n");
        else
            printf("\nAluno nao encontrado!\n");
    } else {
        printf("Erro ao editar aluno!\n");
    }

    sqlite3_finalize(stmt);
}

// Remover aluno
void removerAluno() {
    sqlite3_stmt *stmt;
    int id;
    char confirmacao;

    printf("\nDigite o ID do aluno: ");

    if (scanf("%d", &id) != 1) {
        limparBuffer();
        printf("ID invalido!\n");
        return;
    }

    limparBuffer();

    printf("Deseja realmente remover? (S/N): ");
    scanf(" %c", &confirmacao);
    limparBuffer();

    if (confirmacao != 'S' && confirmacao != 's') {
        printf("Operacao cancelada!\n");
        return;
    }

    const char *sql = "DELETE FROM alunos WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar remocao!\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0)
            printf("\nAluno removido!\n");
        else
            printf("\nAluno nao encontrado!\n");
    } else {
        printf("Erro ao remover aluno!\n");
    }

    sqlite3_finalize(stmt);gcc -Wall -Wextra -std=c11 main.c -o sistema -lsqlite3
}

// Menu principal
int main() {
    int opcao;

    if (!conectarBanco()) {
        if (db != NULL)
            sqlite3_close(db);
        return 1;
    }

    criarTabela();

    do {
        printf("\n================================\n");
        printf("       GERENCIAR ALUNOS\n");
        printf("================================\n");
        printf("1 - Adicionar aluno\n");
        printf("2 - Listar alunos\n");
        printf("3 - Pesquisar aluno\n");
        printf("4 - Editar aluno\n");
        printf("5 - Remover aluno\n");
        printf("0 - Sair\n");
        printf("================================\n");
        printf("Escolha: ");

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            printf("Opcao invalida!\n");
            continue;
        }

        limparBuffer();

        switch (opcao) {
            case 1: adicionarAluno(); break;
            case 2: listarAlunos(); break;
            case 3: pesquisarAluno(); break;
            case 4: editarAluno(); break;
            case 5: removerAluno(); break;
            case 0: printf("Encerrando sistema...\n"); break;
            default: printf("Opcao invalida!\n");
        }

    } while (opcao != 0);

    sqlite3_close(db);

    return 0;
}
