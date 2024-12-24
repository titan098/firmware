#include "GPSDStream.h"

#include <libgpsmm.h>

GPSDStream::~GPSDStream()
{
    if (_gpsd != NULL) {
        delete _gpsd;
    }
}

bool GPSDStream::open(const char *host, const char *port)
{
    _gpsd = new gpsmm(host, port);
    if (_gpsd->stream(WATCH_ENABLE | WATCH_NMEA) == nullptr) {
        LOG_ERROR("Failed to open gpsd stream on %s:%s", host, port);

        delete _gpsd;
        _gpsd = nullptr;
    };
    LOG_INFO("Connected to gpsd: %s:%s", host, port);
    return true;
}

void GPSDStream::refresh()
{
    bool must_refresh = (_last_refresh - millis()) > 30000;

    if (!must_refresh && _buffer.length() > 0) {
        return;
    }

    _buffer = String("");
    if ((_gps_data = _gpsd->read()) == nullptr) {
        _gps_buffer = nullptr;
        return;
    }
    _last_refresh = millis();
    _gps_buffer = _gpsd->data();
    _buffer = String(_gps_buffer);
}

int GPSDStream::available()
{
    refresh();
    return _buffer.length();
}

int GPSDStream::read()
{
    available();
    if (_buffer.length() > 0) {
        int ret = _buffer[0];
        _buffer = _buffer.substring(1);
        return ret;
    }
    return 0;
}

int GPSDStream::peek()
{
    available();
    if (_buffer.length() > 0) {
        return _buffer[0];
    }
    return 0;
}