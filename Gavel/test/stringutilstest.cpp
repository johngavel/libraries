#include "../src/GavelUtil/stringutils.h"

#include "../src/GavelUtil/stringutils.cpp"

#include <cassert>
#include <cstdio>
#include <cstring>

int PASSED = 0;
// Helper macro for manual assertion
#define ASSERT_EQ_STR(actual, expected, testname) assert(strcmp(actual, expected) == 0)

void testConstructors() {
  StringBuilder sb1("Hello");
  assert(strcmp(sb1.c_str(), "Hello") == 0);
  printf("Constructor with const char*: PASS\n");

  StringBuilder sb2(42);
  assert(strcmp(sb2.c_str(), "42") == 0);
  printf("Constructor with int: PASS\n");

  StringBuilder sb3('A');
  assert(strcmp(sb3.c_str(), "65") == 0); // Note: appends ASCII code
  printf("Constructor with char: PASS\n");
}

void testAssignment() {
  StringBuilder sb;
  sb = "World";
  assert(strcmp(sb.c_str(), "World") == 0);
  printf("Assignment with const char*: PASS\n");

  sb = 123;
  assert(strcmp(sb.c_str(), "123") == 0);
  printf("Assignment with int: PASS\n");
}

void testAppend() {
  StringBuilder sb("Value: ");
  sb + 99 + ", Done";
  assert(strcmp(sb.c_str(), "Value: 99, Done") == 0);
  printf("Append chaining: PASS\n");
}

void testClear() {
  StringBuilder sb("ClearMe");
  sb.clear();
  assert(strcmp(sb.c_str(), "") == 0);
  printf("Clear method: PASS\n");
}

void testBufferLimit() {
  StringBuilder sb;
  for (int i = 0; i < 200; i++) {
    sb + "X"; // Will truncate silently
  }
  assert(strlen(sb.c_str()) == (STRINGBUILDER_MAX - 1));
  printf("Buffer limit handling: PASS (truncation occurs)\n");
}

int main() {
  // Test safeAppend
  char dest1[10] = "Hi";
  ASSERT_EQ_STR(safeAppend(dest1, "There", 10), "HiThere", "safeAppend basic");
  char dest2[10] = "HiThere";
  ASSERT_EQ_STR(safeAppend(dest2, "1234567890", 10), "HiThere12", "safeAppend overflow");

  // Test tab
  char buffer1[20] = "abc";
  ASSERT_EQ_STR(tab(4, buffer1, 20), "abc ", "tab basic");

  // Test hexByteString
  char hexbuf[4];
  ASSERT_EQ_STR(hexByteString(255, hexbuf, 4), "FF", "hexByteString 255");
  ASSERT_EQ_STR(hexByteString(0, hexbuf, 4), "00", "hexByteString 0");

  // Test decByteString
  char decbuf[4];
  ASSERT_EQ_STR(decByteString(99, decbuf, 4), "99", "decByteString 99");
  ASSERT_EQ_STR(decByteString(100, decbuf, 4), "99", "decByteString capped");

  // Test getMacString
  unsigned char mac[6] = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E};
  char macbuf[18];
  ASSERT_EQ_STR(getMacString(mac, macbuf, 18), "00:1A:2B:3C:4D:5E", "getMacString basic");

  // Test getIPString
  unsigned char ip[4] = {192, 168, 1, 1};
  char ipbuf[16];
  ASSERT_EQ_STR(getIPString(ip, ipbuf, 16), "192.168.1.1", "getIPString basic");

  // Test timeString
  char timebuf[10];
  ASSERT_EQ_STR(timeString(3661, timebuf, 10), "1:01:01", "timeString 3661");

  // Test trimWhitespace
  char wsbuf[20] = "  hello world  ";
  ASSERT_EQ_STR(trimWhitespace(wsbuf, 20), "hello world", "trimWhitespace basic");

  testConstructors();
  testAssignment();
  testAppend();
  testClear();
  testBufferLimit();
  printf("All tests completed.\n");

  return PASSED;
}