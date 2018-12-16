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

#define NO_SQLITE_CFG_CHECK_SIG 1

#define NO_SQLITE_ERR_PAGE_NOT_FOUND 30001
#define NO_SQLITE_ERR_FINDING_COL    30002
#define NO_SQLITE_ERR_SHORT_READ     30003
#define NO_SQLITE_ERR_NOT_SQLITE_DB  30004

struct cursor_struct {
  byte pagePath;
  int cellPos;
  int cellCount;
};

class SQLiteNoSQL {

private:
  byte *buf;
  char *fileName;
  FILE *fd;
  int pageSize, usableSize, maxLocal, minLocal, maxLeaf, minLeaf;
  int err_no;
  static const int8_t data_size_map[10];
  int getPage(uint32_t pageNo);
  void readFromFile(uint32_t pageNo, size_t bytesToRead);

public:
  SQLiteNoSQL(char *filePath, byte *workingBuf = NULL);
  int32_t getRootPageOf(char *objName, struct cursor_struct *cursor = NULL);
  int locate(int32_t rootPage, byte **payloadPtr,
        int64_t rowId, byte *keyArray[] = NULL, int keyLenArray[] = NULL,
        struct cursor_struct *cursor = NULL);
  int next(struct cursor_struct *cursor, byte **payloadPtr);
  int parseColumn(byte *payload, int colIdx, byte **valuePtr);
  void close();

  int get_errno() {
    return err_no;
  }
  char *str_err() {
    if (err_no < 32000)
      return strerror(err_no);
    return "error";
  }

  static int getVarInt(byte *at, uint64_t *ret);
  static uint64_t getIntValue(byte *at, int sz);
  static double getFloatValue(byte *at);
  static int compare(const char *v1, byte len1, const char *v2,
      byte len2, int k = 0);
  static uint32_t fourBytesToInt(byte *arr, int pos) {
    return (arr[pos] << 24) + (arr[pos + 1] << 16) + (arr[pos + 2] << 8) + arr[pos + 3];
  }
  static int twoBytesToInt(byte *arr, int pos) {
    return (arr[pos] << 8) + arr[pos + 1];
  }
};

#endif // SQLiteNoSQL_h
