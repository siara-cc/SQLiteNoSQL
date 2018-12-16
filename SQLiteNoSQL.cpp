#include "SQLiteNoSQL.h"
#include <limits.h>
#include <math.h>

// constructor
SQLiteNoSQL::SQLiteNoSQL(char *filePath, byte *workingBuf = NULL) {
  buf = workingBuf;
  err_no = 0;
  fd = fopen(filePath, "rb");
  if (fd == NULL) {
    err_no = errno;
    return;
  }
  readFromFile(1, 100);
  if (err_no)
    return;
#ifdef NO_SQLITE_CFG_CHECK_SIG
  if (buf[0] != 83 || buf[1] != 81 || buf[2] != 76 || buf[3] != 105
          || buf[4] != 116 || buf[5] != 101 || buf[6] != 32 || buf[7] != 102
          || buf[8] != 111 || buf[9] != 114 || buf[10] != 109 || buf[11] != 97
          || buf[12] != 116 || buf[13] != 32 || buf[14] != 51 || buf[15] != 0) {
    err_no = NO_SQLITE_ERR_NOT_SQLITE_DB;
    return;
  }
#endif
  pageSize = twoBytesToInt(buf, 16);
  if (pageSize == 1)
      pageSize = 65536;
  usableSize = pageSize - buf[20];
  maxLocal = floor((usableSize - 12) * 64 / 255 - 23);
  minLocal = floor((usableSize - 12) * 32 / 255 - 23);
  maxLeaf = floor(usableSize - 35);
  minLeaf = floor((usableSize - 12) * 32 / 255 - 23);
}

int SQLiteNoSQL::getPage(uint32_t pageNo) {
  readFromFile(pageNo, pageSize);
}

void SQLiteNoSQL::readFromFile(uint32_t pageNo, size_t bytesToRead) {
  if (fseek(fd, pageNo))
  size_t bytesRead = fread(buf, pageSize, 1, fd);
  if (ferror(fd)) {
    err_no = errno;
    return;
  }
  if (bytesRead != bytesToRead) {
    err_no = NO_SQLITE_ERR_SHORT_READ;
    return;
  }
}

// returns page number on success, 0 on failure
// check errno for error details.
int32_t SQLiteNoSQL::getRootPageOf(char *objName, struct cursor_struct *cursor) {
  byte *payload;
  err_no = 0;
  struct cursor_struct localCursor;
  if (cursor == NULL)
    cursor = &localCursor;
  locate(1, &payload, 0, NULL, NULL, cursor);
  if (err_no)
    return 0;
  do {
    byte *valuePtr;
    int valueLen = parseColumn(payload, 2, &valuePtr);
    if (err_no)
       return 0;
    if (valueLen == strlen(objName) &&
        strnstr(objName, (const char *) valuePtr, valueLen) == objName) {
      valueLen = parseColumn(payload, 4, &valuePtr);
      if (err_no)
        return 0;
      return getIntValue(valuePtr, valueLen);
    }
  } while (next(cursor, &payload));
  err_no = NO_SQLITE_ERR_PAGE_NOT_FOUND;
  return 0;
}

int SQLiteNoSQL::locate(int32_t rootPage, byte **payloadPtr,
      int64_t rowId, byte *keyArray[], int keyLenArray[],
      struct cursor_struct *cursor) {
  buf = getPage(rootPage);
  do {
    add to page path
    binary search
    if (index and found)
      return payload
  } while (!leaf)
}
      
int SQLiteNoSQL::next(struct cursor_struct *cursor, byte **payloadPtr) {
  
}

int SQLiteNoSQL::parseColumn(byte *payload, int colIdx, byte **valuePtr) {
  uint64_t ui64;
  int vlen = getVarInt(payload, &ui64);
  byte *data = payload + ui64;
  payload += vlen;
  int curCol = 1;
  while (payload < data && curCol < colIdx) {
    vlen = getVarInt(payload, &ui64);
    payload += vlen;
    data += (ui64 < 10 ? data_size_map[ui64] :
      (ui64 - (ui64 % 2 ? 12 : 13)) / 2);
    curCol++;
  }
  *valuePtr = NULL;
  if (curCol == colIdx) {
    vlen = getVarInt(payload, &ui64);
    switch (ui64) {
      case 0:
        return 0;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        *valuePtr = data;
        return data_size_map[ui64];
      case 8:
        *valuePtr = (byte *) &data_size_map[0];
        return 1;
      case 9:
        *valuePtr = (byte *) &data_size_map[1];
        return 1;
      default:
        *valuePtr = data;
        return (ui64 - (ui64 % 2 ? 12 : 13)) / 2;
    }
  }
  errno = NO_SQLITE_ERR_FINDING_COL;
  return -1;
}

void SQLiteNoSQL::close() {
  fclose(fd);
}

int SQLiteNoSQL::getVarInt(byte *at, uint64_t *ret) {
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

uint64_t SQLiteNoSQL::getIntValue(byte *at, int sz) {
  uint64_t ret = 0;
  while (sz--) {
    ret <<= 8;
    ret += *at++;
  }
  return ret;
}

double SQLiteNoSQL::getFloatValue(byte *at) {
  double out;
  for (int i = 0; i < 8; ++i)
    out += (uint64_t) at[i] << (i * CHAR_BIT);
  return out;
}

int SQLiteNoSQL::compare(const char *v1, byte len1, const char *v2,
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

const int8_t SQLiteNoSQL::data_size_map[] = {0, 1, 2, 3, 4, 6, 8, 8, 0, 0};
