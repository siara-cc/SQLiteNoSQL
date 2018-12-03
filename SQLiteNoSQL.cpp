#include "SQLiteNoSQL.h"

// returns page number on success, 0 on failure
// check errno for error details.
int32_t SQLiteNoSQL::getRootPageOf(char *obj_name, struct cursor_struct *cursor) {
  byte *payload;
  int32_t pageno;
  err_no = 0;
  struct cursor_struct local_cursor;
  if (cursor == NULL)
    cursor = &local_cursor;
  locate(1, &payload, 0, NULL, NULL, cursor);
  if (this->err_no)
     return 0;
  do {
    uint32_t value_len = getValueLen(payload, 2);
    byte value[value_len];
    parseColumn(payload, 2, value);
    if (value_len == strlen(obj_name) &&
      strnstr(obj_name, value, strlen(obj_name)) == obj_name) {
        
      return 
    }
  } while (next(cursor, &payload));
  err_no = NO_SQLITE_ERR_PAGE_NOT_FOUND;
  return 0;
}

int SQLiteNoSQL::locate(int32_t root_page, byte **payload_ptr,
      int64_t row_id, byte *key_array[], int key_len_array[],
      struct cursor_struct *cursor) {

}
      
int SQLiteNoSQL::next(struct cursor_struct *cursor, byte **payload_ptr) {

}

int SQLiteNoSQL::getValueLen(byte *payload, int col_idx) {

}

int SQLiteNoSQL::parseColumn(byte *payload, int col_idx, byte *parsedValue) {

}

void SQLiteNoSQL::close() {
  fclose(fd);
}

static int getVarInt(byte *at, uint64_t *ret) {
  int len = 0;
  *ret = 0;
  while (len++ < 8) {
    *ret <<= 7;
    byte b = *at++;
    *ret += b & 0x7F;
    if ((b >> 7) == 0)
        return len;
  }
  len++;
  *ret <<= 8;
  *ret += *at;
  return len;
}

static int compare(const char *v1, byte len1, const char *v2,
      byte len2, int k = 0) {
  int lim = (len2 < len1 ? len2 : len1);
  while (k < lim) {
      byte c1 = v1[k];
      byte c2 = v2[k];
      k++;
      if (c1 < c2)
          return -k;
      else if (c1 > c2)
          return k;
  }
  if (len1 == len2)
      return 0;
  k++;
  return (len1 < len2 ? -k : k);
}
