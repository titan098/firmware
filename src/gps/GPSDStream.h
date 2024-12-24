#pragma once

#include <libgpsmm.h>

#include "Stream.h"
#include "configuration.h"

class GPSDStream : public Stream
{
  public:
    GPSDStream(){};
    ~GPSDStream();

    bool open(const char *host, const char *port);
    bool close(void);

    int available();
    int read();
    int peek();

    // NoOp placeholders to deal with the Stream API
    size_t write(uint8_t strPtr) { return 0; }
    size_t write(const char *str) { return 0; }
    size_t write(uint8_t *str, size_t len) { return 0; }
    void begin(uint16_t rate){};
    void end(){};

  private:
    void refresh();

    String _host;
    String _port;

    gpsmm *_gpsd = nullptr;
    struct gps_data_t *_gps_data = nullptr;
    const char *_gps_buffer = nullptr;
    uint32_t _last_refresh = 0;

    String _buffer;
};