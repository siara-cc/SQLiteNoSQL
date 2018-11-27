#ifndef SQLiteNoSQL_h
#define SQLiteNoSQL_h

#include <stdint.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

typedef unsigned char byte;
#ifndef NULL
#define NULL 0
#endif

class sqlite3_file_handler {

};

class SQLiteNoSQL {
private:
  byte *buf;
  sqlite3_file_handler fh;
  char *file_name;
  byte *page_path;

public:
  Sqlite3_512(char *file_name, sqlite3_file_handler& file_handler, byte *working_buf = NULL) {
    fh = file_handler;
    buf = working_buf;
  }
  int getRootPageOf(char *obj_name, uint32_t *root_page_ptr);
  int locatePayloadByRowId(uint32_t root_page, uint32_t rowid, byte **pay_load_ptr);
  int locatePayloadByKey(uint32_t root_page, byte **pay_load_ptr, ...);
  int parseColumn(byte *pay_load, int col_idx, byte *parsedValue, int *value_len);

};

#endif // SqliteNoSQL_h
