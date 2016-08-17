
#ifndef __smf_source_h
#define __smf_source_h

#include "smf/stateful.h"
#include "avkit/av_packet.h"
#include "avkit/av_demuxer.h"
#include "cppkit/ck_string.h"
#include <memory>
#include <functional>

namespace smf
{

class source : public stateful
{
public:
    CK_API source() = default;
    CK_API virtual ~source() noexcept {}

    CK_API virtual bool get( std::shared_ptr<avkit::av_packet>& pkt ) = 0;

    CK_API virtual void run() = 0;
    CK_API virtual void stop() = 0;

    CK_API virtual void set_param( const cppkit::ck_string& name, const cppkit::ck_string& val ) = 0;
    CK_API virtual void commit_params() = 0;

    CK_API void register_demuxer_init_cb( std::function<void(avkit::av_demuxer& demuxer)> cb ) { _demuxCB = cb; }

protected:
    std::function<void(avkit::av_demuxer& demuxer)> _demuxCB;
};

}

#endif
