#include "configuration.h"

#if defined(USE_GPSD)
#include "GPSDStream.h"

GPSDStream::~GPSDStream()
{
    close();
}

bool GPSDStream::open(const char *host, const char *port)
{
    // if we are already have an object then don't try recreate it
    if (_gpsd != nullptr) {
        return true;
    }

    // record the host and port
    if (_host == "") {
        _host = String(host);
    }

    if (_port == "") {
        _port = String(port);
    }

    // create a new gpsmm object and open the stream for reading NMEA sentences
    _gpsd = new gpsmm(_host.c_str(), _port.c_str());
    if (_gpsd->stream(WATCH_ENABLE | WATCH_NMEA) == nullptr) {
        LOG_ERROR("Failed to open gpsd stream on %s:%s", host, port);

        delete _gpsd;
        _gpsd = nullptr;
    };
    LOG_INFO("Connected to gpsd: %s:%s", host, port);
    return true;
}

bool GPSDStream::close(void)
{
    LOG_INFO("Closing gpsd stream");
    if (_gpsd != nullptr) {
        delete _gpsd;
        _gpsd = nullptr;
        _gps_buffer = nullptr;
        _last_refresh = 0;
        _buffer = String("");
    }
}

void GPSDStream::refresh()
{
    // if we aren't open then close the existing connection and reopen it
    if (_gpsd == nullptr || !_gpsd->is_open()) {
        close();
        open(_host.c_str(), _port.c_str());
    }

    bool must_refresh = (_last_refresh - millis()) > MAX_REFRESH_TIME;

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

bool GPSDStream::is_open()
{
    return _gpsd->is_open();
}

#endif