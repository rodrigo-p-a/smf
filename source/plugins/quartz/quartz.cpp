
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
    _firstAfterRun(false),
    _runStartTime(),
    _streamTime(),
    _lastPTS(-1)
{
}

quartz::~quartz() throw()
{
}

void quartz::run()
{
    _firstAfterRun = true;
}

void quartz::stop()
{
    _firstAfterRun = false;
}

shared_ptr<av_packet> quartz::process( shared_ptr<av_packet> pkt )
{
    if(_firstAfterRun)
    {
        _freq = pkt->get_ts_freq();
        _runStartTime = steady_clock::now();
        _streamTime = milliseconds();
        _firstAfterRun = false;
    }

    auto pts = pkt->get_pts();

    if(_lastPTS != -1)
    {
        _streamTime += milliseconds((int)(((double)(pts - _lastPTS) / (double)_freq) * 1000));
        
        auto then = _runStartTime + _streamTime;

        auto now = steady_clock::now();
        
        if(then > now)
            ck_usleep(duration_cast<microseconds>(then - now).count());
    }
        
    _lastPTS = pts;

    return pkt;
}
