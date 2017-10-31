
#include "smf/plugins/quartz/quartz.h"
#include "avkit/options.h"
#include "avkit/utils.h"
#include "cppkit/os/ck_time_utils.h"

using namespace smf;
using namespace cppkit;
using namespace avkit;
using namespace std;
using namespace std::chrono;

quartz::quartz() :
    _freq(0),
    _reset(false),
    _baseTime(),
    _streamTime(),
    _lastPTS(-1)
{
}

quartz::~quartz() throw()
{
}

void quartz::run()
{
    _reset = true;
}

void quartz::stop()
{
    _reset = false;
}

shared_ptr<av_packet> quartz::process(shared_ptr<av_packet> pkt)
{
    if(_reset)
    {
        _freq = pkt->get_ts_freq();
        _baseTime = steady_clock::now();
        _streamTime = milliseconds();
        _reset = false;
    }

    auto pts = pkt->get_pts();

    if(_lastPTS != -1)
    {
        _streamTime += milliseconds(ck_convert_clock_freq<int64_t>((pts - _lastPTS), _freq, 1000));
        
        auto then = _baseTime + _streamTime;

        auto now = steady_clock::now();

        // If its been more than ten minutes then reset our base time. This should
        // help us avoid any kind of rounding error.
        if((now - _baseTime) > minutes(10))
            _reset = true;
        
        if(then > now)
            ck_usleep(duration_cast<microseconds>(then - now).count());
    }
        
    _lastPTS = pts;

    return pkt;
}
