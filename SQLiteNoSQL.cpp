#include "SQLiteNoSQL.h"
#include <limits.h>

// returns page number on success, 0 on failure
// check errno for error details.
int32_t SQLiteNoSQL::getRootPageOf(char *obj_name, struct cursor_struct *cursor) {
  byte *payload;
  err_no = 0;
  struct cursor_struct local_cursor;
  if (cursor == NULL)
    cursor = &local_cursor;
  locate(1, &payload, 0, NULL, NULL, cursor);
  if (err_no)
    return 0;
  do {
    byte *value_ptr;
    int value_len = parseColumn(payload, 2, &value_ptr);
    if (err_no)
       return 0;
    if (value_len == strlen(obj_name) &&
        strnstr(obj_name, (const char *) value_ptr, value_len) == obj_name) {
      value_len = parseColumn(payload, 4, &value_ptr);
      if (err_no)
        return 0;
      return getIntValue(value_ptr, value_len);
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

int SQLiteNoSQL::parseColumn(byte *payload, int col_idx, byte **parsedValue) {
  uint64_t ui64;
  int vlen = getVarInt(payload, &ui64);
  byte *data = payload + ui64;
  payload += vlen;
  int curCol = 1;
  while (payload < hdrLen && curCol < col_idx) {
    vlen = getVarInt(payload, &ui64);
    payload += vlen;
    curCol++;
  }
    switch (ui64) {
      case 0:
      case 8:
      case 9:
          *parsedValue = NULL;
        break;
      case 1:
      case 2:
      case 3:
      case 4:
        hdr += "<td>i" + (8 * colInfo[0]) + "</td>";
        det += getIntValue(arr, dataPtr, colInfo[0]);
        dataPtr += colInfo[0];
        break;
      case 5:
      case 6:
        hdr += "<td>i" + (colInfo[0] == 5 ? "48" : "64") + "</td>";
        det += getIntValue(arr, dataPtr, colInfo[0] == 5 ? 6 : 8);
        dataPtr += (colInfo[0] == 5 ? 6 : 8);
        break;
      case 7:
        hdr += "<td>f64</td>";
        det += getFloatValue(arr, dataPtr).toPrecision();
        dataPtr += 8;
        break;
      case 10:
      case 11:
        hdr += "?";
        det += "?";
        break;
      default:
        var dataLen = colInfo[0] - (colInfo[0] % 2 ? 12 : 13);
        dataLen /= 2;
        dataLen = Math.floor(dataLen);
        if (colInfo[0] % 2) {
          hdr += "<td>text</td>";
          var dec = new TextDecoder(txtEncoding);
          det += dec.decode(arr.slice(dataPtr, dataPtr + dataLen));
        } else {
          hdr += "<td>blob</td>";
          det += toHexString(arr.slice(dataPtr, dataLen));
        }
        dataPtr += dataLen;
    }
    if (pageId.substr(0, 2) == 'r0' && colIdx == 3) {
      var pageNo = det.substring(det.lastIndexOf("<td>") + 4);
      det += "<input type='button' value='Open'"
              + " onclick='openPage(\"" + pageId + "\"," + pageNo + ", \"b\", true)'/>";
    }
  }
  return [hdr, det];
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

static uint64_t getIntValue(byte *at, int sz) {
  uint64_t ret = 0;
  while (sz--) {
    ret <<= 8;
    ret += *at++;
  }
  return ret;
}

static double getFloatValue(byte *at) {
  double out;
  for (int i = 0; i < 8; ++i)
    out += (uint64_t) at[i] << (i * CHAR_BIT);
  return out;
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

int8_t data_size_map[] = {0, 1, 2, 3, 4, 6, 8, 8, 0, 0};
