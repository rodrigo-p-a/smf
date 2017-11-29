
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

// Missing Features:
// - You should be able to associate multiple clock plugin instances.
// - Imagine the following scenario:
//   - you have 2 associated pipelines, 1 doing audio and 1 doing video.
//   - It is likely that audio samples represent a larger increment of time than
//     video samples.
//     - perhaps letting one audio sample through advances the audio pipelines
//       stream clock by 1 full second but each video frame only advances the
//       video stream clock by 1/15'th of a second.
//   - NOTE: Since a one second audio clip takes one second to play, perhaps you
//     should always release your sample BEFORE you sleep!
//   - Keeping these pipelines synced involves electing one of them to be the
//     master stream clock for the group. The master should always be the stream
//     with the larger time increment per sample (likely audio). When a sample
//     arrives at the master the masters stream clock should be incremented by
//     the samples duration. Any blocked slave streams should be woken and if
//     the masters stream clock is now in the future it should then sleep until
//     that time. 
//   - slave streams should allow samples through until their stream clocks
//     catch up to the master. slave streams should then block until awoken
//     by the master.

shared_ptr<av_packet> quartz::process(shared_ptr<av_packet> pkt)
{
    auto now = steady_clock::now();

    if(_reset || ((now - _baseTime) > minutes(10)))
    {
        _freq = pkt->get_ts_freq();
        _baseTime = steady_clock::now();
        _streamTime = milliseconds();
        _reset = false;
    }

    auto then = _baseTime + _streamTime;

    if(then > now)
        ck_usleep(duration_cast<microseconds>(then - now).count());

    _streamTime += milliseconds(ck_convert_clock_freq<int64_t>(pkt->get_duration(), _freq, 1000));

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
