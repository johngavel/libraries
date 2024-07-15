#include "html.h"

#include "datastructure.h"
#include "serialport.h"

#define HTML_BUFFER_SIZE 49152
#define TAG_LENGTH 20
#define SIZE_OF_STACK 32
static char stackMemory[TAG_LENGTH * SIZE_OF_STACK];
static ClassicStack stack(SIZE_OF_STACK, TAG_LENGTH, stackMemory);
static char html[HTML_BUFFER_SIZE];

HTMLBuilder::HTMLBuilder() {
  memset(html, 0, HTML_BUFFER_SIZE);
  index = 0;
}

HTMLBuilder* HTMLBuilder::print(const char* line) {
  unsigned long size = strlen(line);
  if ((size + index) < HTML_BUFFER_SIZE) {
    memcpy(&html[index], line, strlen(line));
    index += size;
  } else {
    CONSOLE->println(ERROR, "Building HTML exceeded buffer length");
  }
  return this;
}

HTMLBuilder* HTMLBuilder::println() {
  char crlf[3] = "\r\n";
  print(String(crlf));
  return this;
}

HTMLBuilder* HTMLBuilder::println(const char* line) {
  print(line);
  println();
  return this;
}

HTMLBuilder* HTMLBuilder::print(String line) {
  char charArray[200];
  line.toCharArray(charArray, 200);
  print(charArray);
  return this;
}

HTMLBuilder* HTMLBuilder::println(String line) {
  print(line);
  println();
  return this;
}

HTMLBuilder* HTMLBuilder::print(int line) {
  char charArray[10];
  sprintf(charArray, "%d", line);
  print(charArray);
  return this;
}

HTMLBuilder* HTMLBuilder::println(int line) {
  print(line);
  println();
  return this;
}

char* HTMLBuilder::buffer() {
  if (!stack.empty()) CONSOLE->println(ERROR, "Closing Tags were not called! " + String(stack.count()));
  stack.clear();
  return html;
}

unsigned long HTMLBuilder::length() {
  return index;
}

HTMLBuilder* HTMLBuilder::tag(String line) {
  print("<" + line + ">");
  return this;
}

HTMLBuilder* HTMLBuilder::tag(String line, String options) {
  print("<" + line + " " + options + ">");
  return this;
}

HTMLBuilder* HTMLBuilder::brTag() {
  tag("br");
  return this;
}

HTMLBuilder* HTMLBuilder::hrTag() {
  tag("hr", "style=\"width:50%\"");
  return this;
}

HTMLBuilder* HTMLBuilder::openTag(String line) {
  tag(line);
  stack.push((void*) line.c_str());
  return this;
}

HTMLBuilder* HTMLBuilder::openTag(String line, String options) {
  tag(line, options);
  stack.push((void*) line.c_str());
  return this;
}

HTMLBuilder* HTMLBuilder::closeTag() {
  char line[TAG_LENGTH];
  if (stack.count() > 0) {
    stack.pop(line);
    print("</" + String(line) + ">");
  } else {
    CONSOLE->println(ERROR, "No Closing Tag avaialable!");
  }
  return this;
}

HTMLBuilder* HTMLBuilder::closeTag(String line) {
  print("<" + line + " />");
  return this;
}

HTMLBuilder* HTMLBuilder::closeTag(String line, String options) {
  print("<" + line + " " + options + " />");
  return this;
}

HTMLBuilder* HTMLBuilder::openTrTag() {
  openTag("tr");
  return this;
}

HTMLBuilder* HTMLBuilder::openTrTag(String options) {
  openTag("tr", options);
  return this;
}

HTMLBuilder* HTMLBuilder::openTdTag() {
  openTag("td");
  return this;
}

HTMLBuilder* HTMLBuilder::openTdTag(String options) {
  openTag("td", options);
  return this;
}

HTMLBuilder* HTMLBuilder::tdTag(String line) {
  openTag("td");
  print(line);
  closeTag();
  return this;
}

HTMLBuilder* HTMLBuilder::tdTag(String line, String options) {
  openTag("td", options);
  print(line);
  closeTag();
  return this;
}

HTMLBuilder* HTMLBuilder::thTag(String line) {
  openTag("th");
  print(line);
  closeTag();
  return this;
}
