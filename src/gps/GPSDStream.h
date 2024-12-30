#pragma once
#include "configuration.h"

#if defined(USE_GPSD)
#include <ctime>
#include <iomanip>
#include <libgpsmm.h>

#include "Stream.h"

#define MAX_REFRESH_TIME 120000
#define CONNECTION_RETRIES 10

class GPSDStream : public Stream
{
  private:
    void refresh();

    String _host;
    String _port;
    int _connection_retries = 0;

    gpsmm *_gpsd = nullptr;
    struct gps_data_t *_gps_data = nullptr;
    const char *_gps_buffer = nullptr;
    uint32_t _last_refresh = 0;

    String _buffer;

    String lat_to_nmea(double lat);
    String lon_to_nmea(double lon);
    uint8_t nmea_checksum(const char *sentence);
    String to_nmea_time(timespec_t gpsd_time);
    String to_nmea_date(timespec_t gpsd_time);

  public:
    GPSDStream(){};
    ~GPSDStream();

    bool open(const char *host, const char *port);
    void close();

    int available();
    int read();
    int peek();

    bool is_open();

    // NoOp placeholders to deal with the Stream API
    size_t write(uint8_t strPtr) { return 0; }
    size_t write(const char *str) { return 0; }
    size_t write(uint8_t *str, size_t len) { return 0; }
};

#endif