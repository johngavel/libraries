#ifndef __GAVEL_SCANNER
#define __GAVEL_SCANNER

#define SCAN Scan::get()

class Scan {
public:
  static Scan* get();
  void setup();
  void scan();

private:
  Scan(){};
  static Scan* scanner;
  static void scani2c();
};

#endif
