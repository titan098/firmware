#pragma once
#include "configuration.h"

#if defined(USE_GPSD)
#include <GPSDStream.h>

/**
 * GPSDGlue is a wrapper around HardwareSerial to allow the existing _gps_serial object in the GPS module to be used and consume
 * the stream of data from GPSDStream.
 */
class GPSDGlue : public HardwareSerial
{
  private:
    GPSDStream *_gpsd;

  public:
    GPSDGlue(GPSDStream *gpsd);
    ~GPSDGlue();

    void begin(unsigned long baudrate){};
    void begin(unsigned long baudrate, uint16_t config){};
    void end(){};
    void flush(){};

    int available();
    int peek();
    int read();
    size_t write(uint8_t c);

    operator bool();
};

#endif