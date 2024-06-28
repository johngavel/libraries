#ifndef __GAVEL_AVG
#define __GAVEL_AVG

class Average {
public:
  Average() : count(0), accumulator(0), factor(100000), average(0.0){};
  void sample(unsigned long value) {
    accumulator += value;
    count++;
    if (count > factor) reset();
  };
  double getAverage() { return average; };
  void setFactor(unsigned long __factor) {
    if (factor != __factor) {
      reset();
      factor = __factor;
    }
  }

private:
  unsigned long count;
  unsigned long accumulator;
  unsigned long factor;
  double average;
  void reset() {
    if (count != 0) { average = (double) accumulator / (double) count; }
    accumulator = average;
    count = 1;
  }
};

#endif
