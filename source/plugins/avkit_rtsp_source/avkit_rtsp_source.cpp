
#include "smf/plugins/avkit_rtsp_source/avkit_rtsp_source.h"

using namespace smf;
using namespace std;
using namespace cppkit;
using namespace avkit;

avkit_rtsp_source::avkit_rtsp_source() :
    _rtspURL(),
    _demux(),
    _gotClockRate(false),
    _clockRate(),
    _pts( 0 ),
    _params(),
    _running( false ),
    _demuxCBFired( false ),
    _firstKey( false )
{
}

avkit_rtsp_source::~avkit_rtsp_source() throw()
{
}

void avkit_rtsp_source::run()
{
    _running = true;
    _demuxCBFired = false;
    _firstKey = false;
}

void avkit_rtsp_source::stop()
{
    _running = false;
}

bool avkit_rtsp_source::get( shared_ptr<av_packet>& pkt )
{
    if( !_demux )
        _demux = make_shared<av_demuxer>( _rtspURL, false );

    int videoStream = _demux->get_video_stream_index();

    if( !_gotClockRate )
    {
        // at stream negotiation time we know the clock rate (it's int the
        // SDP), but we don't know the duration (framerate) until we start
        // pulling frames. For this reason, the _clockRate.second is accurate
        // but _clockRate.first is not valid.
        _clockRate = _demux->get_time_base( videoStream );
        _gotClockRate = true;
    }

    int index = -1;
    bool gotVideoFrame = false;

    while( _running && !gotVideoFrame && _demux->read_frame( index ) )
    {
        if( index != videoStream )
            continue;

        if( !_firstKey )
        {
            if( _demux->is_key() )
                _firstKey = true;
            else continue;
        }

        pkt = _demux->get();
        if( !_demuxCBFired )
        {
            if( _demuxCB )
                _demuxCB( *_demux );
            _demuxCBFired = true;
        }

        pkt->set_ts_freq( _clockRate.second );
        pkt->set_pts( _pts );
        pkt->set_dts( _pts );
        _pts += pkt->get_duration();
        gotVideoFrame = true;
    }

    return gotVideoFrame;
}

void avkit_rtsp_source::set_param( const cppkit::ck_string& name, const cppkit::ck_string& val )
{
    if( _params.find( name ) == _params.end() )
        _params.insert( make_pair( name, val ) );
    else _params[name] = val;
}

void avkit_rtsp_source::commit_params()
{
    _rtspURL = _params["rtsp_url"];
}
