#ifndef __GAVEL_HTML
#define __GAVEL_HTML
#include <Arduino.h>

class HTMLBuilder {
public:
  HTMLBuilder();
  HTMLBuilder* print(const char* line);
  HTMLBuilder* println(const char* line);
  HTMLBuilder* println();
  HTMLBuilder* print(String line);
  HTMLBuilder* println(String line);
  HTMLBuilder* print(int line);
  HTMLBuilder* println(int line);

  HTMLBuilder* tag(String line);
  HTMLBuilder* tag(String line, String options);
  HTMLBuilder* openTag(String line);
  HTMLBuilder* openTag(String line, String options);
  HTMLBuilder* closeTag();
  HTMLBuilder* closeTag(String line);
  HTMLBuilder* closeTag(String line, String options);
  HTMLBuilder* brTag();
  HTMLBuilder* hrTag();
  HTMLBuilder* openTrTag();
  HTMLBuilder* openTrTag(String options);
  HTMLBuilder* openTdTag();
  HTMLBuilder* openTdTag(String options);
  HTMLBuilder* tdTag(String line);
  HTMLBuilder* tdTag(String line, String options);
  HTMLBuilder* thTag(String line);
  char* buffer();
  unsigned long length();

private:
  unsigned long index;
};

#endif