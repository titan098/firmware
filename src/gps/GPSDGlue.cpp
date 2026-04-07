#include "configuration.h"

#if defined(USE_GPSD)
#include <GPSDGlue.h>

GPSDGlue::GPSDGlue(GPSDStream *gpsd)
{
    _gpsd = gpsd;
}

GPSDGlue::~GPSDGlue()
{
    _gpsd->close();
}

int GPSDGlue::available()
{
    return _gpsd->available();
}

int GPSDGlue::peek()
{
    return _gpsd->peek();
}

int GPSDGlue::read()
{
    return _gpsd->read();
}

size_t GPSDGlue::write(uint8_t c)
{
    return 0;
}

GPSDGlue::operator bool()
{
    return _gpsd != nullptr && _gpsd->is_open();
}

#endif