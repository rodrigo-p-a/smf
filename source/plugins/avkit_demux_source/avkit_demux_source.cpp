
#include "smf/plugins/avkit_demux_source/avkit_demux_source.h"

using namespace smf;
using namespace std;
using namespace cppkit;
using namespace avkit;

avkit_demux_source::avkit_demux_source() :
    _fileName(),
    _demux(),
    _params(),
    _running(false)
{
}

avkit_demux_source::~avkit_demux_source() throw()
{
}

void avkit_demux_source::run()
{
    _running = true;
}

void avkit_demux_source::stop()
{
    _running = false;
}

bool avkit_demux_source::get( shared_ptr<av_packet>& pkt )
{
    if( !_demux )
        _demux = make_shared<av_demuxer>(_fileName, true);

    int videoStream = _demux->get_video_stream_index();

    int index = -1;
    bool gotVideoFrame = false;
    while(_running && !gotVideoFrame)
    {
        bool readFrame = _demux->read_frame(index);
        if(!readFrame)
            CK_THROW(("Failed to read frame from file."));
        if(index != videoStream)
            continue;

        pkt = _demux->get();
        gotVideoFrame = true;
    }
#if 0
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

    while( _running && !gotVideoFrame )
    {
        bool readFrame = _demux->read_frame( index );
        if( !readFrame )
            CK_THROW(("Failed to read frame from RTSP source."));

        if( index != videoStream )
            continue;

        if( !_firstKey )
        {
            if( _demux->is_key() )
                _firstKey = true;
            else continue;
        }

        if( _keyFrameOnly && !_demux->is_key() )
            continue;

        pkt = _demux->get();
        if( !_demuxCBFired )
        {
            if( _demuxCB )
                _demuxCB( *_demux );
            _demuxCBFired = true;
        }

        pkt->set_pts( _pts );
        pkt->set_dts( _pts );
        _pts += pkt->get_duration();
        gotVideoFrame = true;
    }
#endif
    return gotVideoFrame;
}

void avkit_demux_source::set_param( const cppkit::ck_string& name, const cppkit::ck_string& val )
{
    if( _params.find( name ) == _params.end() )
        _params.insert( make_pair( name, val ) );
    else _params[name] = val;
}

void avkit_demux_source::commit_params()
{
    _fileName = _params["file_name"];
}
