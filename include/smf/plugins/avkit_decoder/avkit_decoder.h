
#ifndef __smf_avkit_decoder_h
#define __smf_avkit_decoder_h

#include "smf/filter.h"
#include "avkit/h264_decoder.h"
#include "avkit/av_demuxer.h"
#include "cppkit/os/ck_exports.h"
#include <map>
#include <mutex>

namespace smf
{

class avkit_decoder final : public filter
{
public:
    CK_API avkit_decoder();
    CK_API virtual ~avkit_decoder() throw();

    CK_API virtual std::shared_ptr<avkit::av_packet> process( std::shared_ptr<avkit::av_packet> pkt );

    CK_API virtual void set_param( const cppkit::ck_string& name, const cppkit::ck_string& val );
    CK_API virtual void commit_params();

    CK_API void init_from_demuxer( avkit::av_demuxer& demuxer );

private:
    std::shared_ptr<avkit::h264_decoder> _decoder;
    std::map<std::string, cppkit::ck_string> _params;
    uint16_t _inputWidth;
    uint16_t _inputHeight;
    uint16_t _requestedWidth;
    uint16_t _requestedHeight;
    std::recursive_mutex _configLok;
    int _decodeAttempts;
    cppkit::ck_string _pictType;
    bool _dropTillNextKey;
};

}

#endif
