
#include "smf/plugins/quartz/quartz.h"
#include "avkit/options.h"
#include "avkit/utils.h"
#include "cppkit/os/ck_time_utils.h"

using namespace smf;
using namespace cppkit;
using namespace avkit;
using namespace std;

static const int DROPPED_HEALTHY_THRESHOLD = 2;

quartz::quartz() :
    _params(),
    _timeBaseNum(0),
    _timeBaseDen(0)
{
}

quartz::~quartz() throw()
{
}

shared_ptr<av_packet> quartz::process( shared_ptr<av_packet> pkt )
{
    auto f = (float)_timeBaseNum / (float)_timeBaseDen;

    ck_usleep((uint32_t)(f * 1000000));

    return pkt;
}

void quartz::set_param( const cppkit::ck_string& name, const cppkit::ck_string& val )
{
    if( _params.find( name ) == _params.end() )
        _params.insert( make_pair( name, val ) );
    else _params[name] = val;
}

void quartz::commit_params()
{
    _timeBaseNum = _params["time_base_num"].to_int();
    _timeBaseDen = _params["time_base_den"].to_int();
}
