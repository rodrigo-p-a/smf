
#include "smf/plugins/avkit_decoder/avkit_decoder.h"
#include "avkit/options.h"
#include "avkit/utils.h"

using namespace smf;
using namespace cppkit;
using namespace avkit;
using namespace std;

static const int DROPPED_HEALTHY_THRESHOLD = 2;

avkit_decoder::avkit_decoder() :
    _decoder(),
    _params(),
    _inputWidth( 0 ),
    _inputHeight( 0 ),
    _requestedWidth( 0 ),
    _requestedHeight( 0 ),
    _configLok(),
    _decodeAttempts(16),
    _pictType(""),
    _dropTillNextKey(true)
{
}

avkit_decoder::~avkit_decoder() throw()
{
}

shared_ptr<av_packet> avkit_decoder::process( shared_ptr<av_packet> pkt )
{
    bool wasDropTillNextKey = _dropTillNextKey;

    if( _dropTillNextKey )
    {
        if( !pkt->is_key() )
            return shared_ptr<av_packet>();
        else _dropTillNextKey = false;
    }

    if( !_decoder )
        _decoder = make_shared<h264_decoder>( (_pictType.empty()) ? get_normal_h264_decoder_options() : get_normal_h264_decoder_options(_pictType) );

    unique_lock<recursive_mutex> g(_configLok);

    try
    {
        _decoder->decode( pkt );
    }
    catch( ck_exception& ex )
    {
        // If we were dropping till next key and we get here it means that even
        // the next key frame failed to decode. In that case, I rethrow the exception
        // here so that the whole stream can be restarted.
        if( wasDropTillNextKey )
            throw(ex);

        _dropTillNextKey = true;
    }

    if( !_dropTillNextKey )
    {
        if( _inputWidth != _decoder->get_input_width() || _inputHeight != _decoder->get_input_height() )
        {
            _inputWidth = _decoder->get_input_width();
            _inputHeight = _decoder->get_input_height();

            if( _requestedWidth == 0 )
                _requestedWidth = _inputWidth;

            if( _requestedHeight == 0 )
                _requestedHeight = _inputHeight;

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

    return shared_ptr<av_packet>();
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

    if( _params.find("width") != _params.end() )
        _requestedWidth = _params["width"].to_uint16();
    else _requestedWidth = 0;

    if( _params.find("height") != _params.end() )
        _requestedHeight = _params["height"].to_uint16();
    else _requestedHeight = 0;

    if( _params.find("decode_attempts") != _params.end() )
        _decodeAttempts = _params["decode_attempts"].to_int();

    if( _params.find("pict_type") != _params.end() )
        _pictType = _params["pict_type"];

    _inputWidth = 0; // trigger a reconfig on the next frame...
}

void avkit_decoder::init_from_demuxer( avkit::av_demuxer& demuxer )
{
    _decoder = make_shared<h264_decoder>( demuxer, get_normal_h264_decoder_options(), _decodeAttempts );
}
