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
    if (_gpsd->stream(WATCH_ENABLE | WATCH_JSON) == nullptr) {
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
        _last_refresh = millis();
        _buffer = String("");
    }
}

void GPSDStream::refresh()
{
    // if we aren't open then close the existing connection and reopen it
    if (_gpsd == nullptr || !_gpsd->is_open()) {
        LOG_WARN("GPSD stream is not open, closing and reopening...");
        close();
        open(_host.c_str(), _port.c_str());
    }

    // if data hasn't been requested in 30 seconds then clear the buffer
    if ((millis() - _last_refresh) >= MAX_REFRESH_TIME) {
        LOG_DEBUG("GPSD stream hasn't been requested in %d seconds, refreshing...", MAX_REFRESH_TIME / 1000);
        _buffer = String("");
    }

    // if we can't read from gpsd then do not proceed
    if ((_gps_data = _gpsd->read()) == nullptr) {
        _gps_buffer = nullptr;
        return;
    }

    // if there is still data being processed then don't try repopulate the buffer
    if (_buffer.length() > 0) {
        return;
    }

    // build the NMEA packets to be processed by the gps module. libgpsmm can return
    // nmea sentences however polling and managing the data nmea stream is more complex to
    // rather just construct the sentences we need and return them.
    if (_gps_data->fix.mode >= MODE_2D) {
        String GGA = "GNGGA";                                               // Talker ID
        GGA += String(",") + to_nmea_time(_gps_data->fix.time);             // Timestamp: hhmmss.sss
        GGA += (String(",") + lat_to_nmea(_gps_data->fix.latitude) + ",");  // Latitude: ddmm.mmmmmm
        GGA += (_gps_data->fix.latitude >= 0) ? "N" : "S";                  // North/South
        GGA += (String(",") + lon_to_nmea(_gps_data->fix.longitude) + ","); // Longitude: dddmm.mmmmmm
        GGA += (_gps_data->fix.longitude >= 0) ? "E" : "W";                 // East/West
        GGA += (String(",") + 1);                                           // GPS Quality
        GGA += (String(",") + _gps_data->satellites_used);                  // Sats
        GGA += (String(",") + _gps_data->dop.hdop);                         // HDOP
        GGA += (String(",") + _gps_data->fix.altitude);                     // Alt
        GGA += ",M";                                                        // AltVal
        GGA += (String(",") + _gps_data->fix.geoid_sep);                    // GeoSepID
        GGA += ",M";                                                        // GeoVal
        GGA += ",";                                                         // DGPS Age
        GGA += ",";                                                         // DGPS Ref
        GGA = (String("$") + GGA + String("*") + String(nmea_checksum(GGA.c_str()), HEX) + "\r\n");
        _buffer += GGA;

        String RMC = "GNRMC";                                               // Talker ID
        RMC += String(",") + to_nmea_time(_gps_data->fix.time);             // Timestamp: hhmmss.sss
        RMC += ",A";                                                        // Validity A/N
        RMC += (String(",") + lat_to_nmea(_gps_data->fix.latitude) + ",");  // Latitude: ddmm.mmmmmm
        RMC += (_gps_data->fix.latitude >= 0) ? "N" : "S";                  // North/South
        RMC += (String(",") + lon_to_nmea(_gps_data->fix.longitude) + ","); // Longitude: dddmm.mmmmmm
        RMC += (_gps_data->fix.longitude >= 0) ? "E" : "W";                 // East/West
        RMC += (String(",") + _gps_data->fix.speed);                        // Speed over ground
        RMC += (String(",") + _gps_data->fix.track);                        // Course over ground
        RMC += (String(",") + to_nmea_date(_gps_data->fix.time));           // date in in ddmmyy
        RMC += ",";                                                         // Magnetic variation
        RMC += ",";                                                         // Magnetic Variation direction
        RMC += ",A";                                                        // Position Indicator
        RMC += ",U";                                                        // Navigation Status
        RMC = (String("$") + RMC + String("*") + String(nmea_checksum(RMC.c_str()), HEX) + "\r\n");
        _buffer += RMC;

        _last_refresh = millis();
    }
}

int GPSDStream::available()
{
    refresh();
    return _buffer.length();
}

int GPSDStream::read()
{
    if (available() > 0) {
        int ret = _buffer[0];
        _buffer = _buffer.substring(1);
        return ret;
    }
    return 0;
}

int GPSDStream::peek()
{
    if (available() > 0) {
        return _buffer[0];
    }
    return 0;
}

bool GPSDStream::is_open()
{
    return _gpsd->is_open();
}

String GPSDStream::lat_to_nmea(double lat)
{
    char buf[50] = {};
    double fracpart, intpart;

    fracpart = modf(lat, &intpart);
    sprintf(buf, "%02d", abs(int(intpart)));
    sprintf(buf, "%s%08.5f", buf, abs(fracpart * 60));
    return String(buf);
}

String GPSDStream::lon_to_nmea(double lon)
{
    char buf[50] = {};
    double fracpart, intpart;

    fracpart = modf(lon, &intpart);
    sprintf(buf, "%03d", abs(int(intpart)));
    sprintf(buf, "%s%08.5f", buf, abs(fracpart * 60));
    return String(buf);
}

uint8_t GPSDStream::nmea_checksum(const char *sentence)
{
    uint8_t checksum = 0;
    for (int i = 0; sentence[i] != '\0'; i++) {
        checksum ^= sentence[i];
    }
    return checksum;
}

String GPSDStream::to_nmea_time(timespec_t gpsd_time)
{
    char buf[20] = {};
    const auto tm = std::gmtime(&gpsd_time.tv_sec);
    strftime(buf, sizeof(buf), "%H%M%S.000", tm);
    return String(buf);
}

String GPSDStream::to_nmea_date(timespec_t gpsd_time)
{
    char buf[20] = {};
    const auto tm = std::gmtime(&gpsd_time.tv_sec);
    strftime(buf, sizeof(buf), "%d%m%y", tm);
    return String(buf);
}

#endif