#ifndef GAVEL_ASCII_TABLE
#define GAVEL_ASCII_TABLE

#include <serialport.h>

#define MAX_TABLE_COLUMNS 10

class AsciiTable {
public:
  AsciiTable();
  void addColumn(COLOR color, String header, unsigned long width);
  void printHeader();
  void printData(String line0 = "", String line1 = "", String line2 = "", String line3 = "", String line4 = "", String line5 = "", String line6 = "",
                 String line7 = "", String line8 = "", String line9 = "");
  void printDone(String done);

private:
  COLOR columnColor[MAX_TABLE_COLUMNS];
  unsigned long columnWidth[MAX_TABLE_COLUMNS];
  String columnHeader[MAX_TABLE_COLUMNS];
  int numberOfColumns;
};

#endif