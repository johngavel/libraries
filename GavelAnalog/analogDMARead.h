#ifndef __GAVEL_ANALOG_DMA_READ
#define __GAVEL_ANALOG_DMA_READ

#include "hardware/dma.h"

#include <architecture.h>
#include <stopwatch.h>

// Capture Pin 0 is GPIO26
// Capture Pin 1 is GPIO27
// Capture Pin 2 is GPIO28
#define CAPTURE_PINS 2
#define ROUND_ROBIN_MASK 0x03
#define CAPTURE_DEPTH 5000
#define CAPTURE_BUFFER_SIZE (CAPTURE_DEPTH * sizeof(uint16_t))

typedef struct {
  unsigned long timestamp;
  unsigned long sampleTime_us;
  uint16_t capture_buf[CAPTURE_DEPTH];
} ADC_BUFFER;

class TransferCapture {
public:
  virtual void processCapture(ADC_BUFFER* buffer) = 0;
};

class AnalogDmaRead : public Task {
public:
  AnalogDmaRead() : Task("AnalogDmaRead"){};
  void setTransfer(TransferCapture* __transfer) { transfer = __transfer; };
  void setupTask();
  void executeTask();
  ADC_BUFFER* getData();
  AvgStopWatch analysisTime;
  AvgStopWatch totalTime;
  AvgStopWatch downTime;

private:
  void setupDMA();
  void gatherSamples();
  void startSample();
  void analyseBuffer();
  void finishSample();
  ADC_BUFFER buffer[CAPTURE_PINS];
  int currentBuffer = 0;
  int lastBuffer = 0;
  dma_channel_config cfg;
  uint dma_chan;
  TransferCapture* transfer = nullptr;
};

#endif