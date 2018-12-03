#ifndef SQLiteNoSQL_h
#define SQLiteNoSQL_h

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

typedef unsigned char byte;
#ifndef NULL
#define NULL 0
#endif

#define NO_SQLITE_ERR_PAGE_NOT_FOUND 30001

struct cursor_struct {
  byte page_path;
  int cell_pos;
  int cell_count;
};

class SQLiteNoSQL {

private:
  byte *buf;
  char *file_name;
  FILE *fd;
  int err_no;
  int getValueLen(byte *payload, int col_idx);

public:
  SQLiteNoSQL(char *file_path, byte *working_buf = NULL) {
    buf = working_buf;
    err_no = 0;
    fd = fopen(file_path, "r");
    if (fd == NULL) {
      err_no = errno;
      return;
    }
  }
  int get_errno() {
    return err_no;
  }
  char *str_err() {
    if (err_no < 32000)
      return strerror(err_no);
    return "error";
  }
  int32_t getRootPageOf(char *obj_name, struct cursor_struct *cursor = NULL);
  int locate(int32_t root_page, byte **payload_ptr,
        int64_t row_id, byte *key_array[] = NULL, int key_len_array[] = NULL,
        struct cursor_struct *cursor = NULL);
  int next(struct cursor_struct *cursor, byte **payload_ptr);
  int parseColumn(byte *payload, int col_idx, byte *parsedValue);
  void close();

};

#endif // SQLiteNoSQL_h
