#include "SQLiteNoSQL.h"

// returns page number on success, 0 on failure
// check errno for error details.
int32_t SQLiteNoSQL::getRootPageOf(char *obj_name, struct cursor_struct *cursor) {
  byte *pay_load;
  int32_t pageno;
  err_no = 0;
  locatePayload(1, &pay_load, 0, NULL, NULL, cursor);
  if (this->err_no)
     return 0;
  return pageno;
}

int SQLiteNoSQL::locate(int32_t root_page, byte **pay_load_ptr,
      int64_t row_id, byte *key_array[], int key_len_array[],
      struct cursor_struct *cursor) {

}
      
int SQLiteNoSQL::next(struct cursor_struct *cursor, byte **pay_load_ptr) {

}

int SQLiteNoSQL::parseColumn(byte *pay_load, int col_idx, byte *parsedValue, int *value_len) {

}

void SQLiteNoSQL::close() {

}
