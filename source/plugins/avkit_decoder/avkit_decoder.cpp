
#include "smf/plugins/avkit_decoder/avkit_decoder.h"
#include "avkit/options.h"
#include "avkit/utils.h"

using namespace smf;
using namespace cppkit;
using namespace avkit;
using namespace std;

avkit_decoder::avkit_decoder() :
    _decoder(),
    _params(),
    _inputWidth( 0 ),
    _inputHeight( 0 ),
    _requestedWidth( 0 ),
    _requestedHeight( 0 ),
    _configLok()
{
}

avkit_decoder::~avkit_decoder() noexcept
{
}

shared_ptr<av_packet> avkit_decoder::process( shared_ptr<av_packet> pkt )
{
    if( !_decoder )
        _decoder = make_shared<h264_decoder>( get_normal_h264_decoder_options() );

    unique_lock<recursive_mutex> g(_configLok);

    _decoder->decode( pkt );

    if( _inputWidth != _decoder->get_input_width() || _inputHeight != _decoder->get_input_height() )
    {
        _inputWidth = _decoder->get_input_width();
        _inputHeight = _decoder->get_input_height();

        uint16_t destWidth = 0, destHeight = 0;

        aspect_correct_dimensions( _inputWidth,
                                   _inputHeight,
                                   _requestedWidth,
                                   _requestedHeight,
                                   destWidth,
                                   destHeight );

        _decoder->set_output_width( destWidth );
        _decoder->set_output_height( destHeight );
    }

    auto decoded = _decoder->get();
    // mostly the md is migrated, but we are changing the size of the video here
    // so set the dimensions.
    decoded->migrate_md_from( *pkt );
    decoded->set_width( _decoder->get_output_width() );
    decoded->set_height( _decoder->get_output_height() );
    return decoded;
}

void avkit_decoder::set_param( const cppkit::ck_string& name, const cppkit::ck_string& val )
{
    unique_lock<recursive_mutex> g(_configLok);

    if( _params.find( name ) == _params.end() )
        _params.insert( make_pair( name, val ) );
    else _params[name] = val;
}

void avkit_decoder::commit_params()
{
    unique_lock<recursive_mutex> g(_configLok);

    _requestedWidth = _params["width"].to_uint16();
    _requestedHeight = _params["height"].to_uint16();

    _inputWidth = 0; // trigger a reconfig on the next frame...
}

void avkit_decoder::init_from_demuxer( avkit::av_demuxer& demuxer )
{
    _decoder = make_shared<h264_decoder>( demuxer, get_normal_h264_decoder_options() );
}
