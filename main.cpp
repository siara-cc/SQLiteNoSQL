#include <stdio.h>
#include "SQLiteNoSQL.h"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: executable <sqlite db file name> <table/index name> <key_column_count(n)> <key_start_1> ... <key_start_n> [key_end_1] ... [key_end_n]\n");
        return 1;
    }
    SQLiteNoSQL sqliteNoSQL(argv[1]);
    uint32_t root_page = sqliteNoSQL.getRootPageOf(argv[2]);
    return 0;
}
