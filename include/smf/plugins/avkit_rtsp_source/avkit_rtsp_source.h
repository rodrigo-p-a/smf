
#ifndef __avkit_rtsp_source_h
#define __avkit_rtsp_source_h

#include "smf/source.h"
#include "cppkit/ck_string.h"
#include "avkit/av_demuxer.h"
#include <memory>
#include <map>
#include <string>

class avkit_rtsp_source_test;

using namespace smf;

class avkit_rtsp_source : public source
{
    friend class ::avkit_rtsp_source_test;

public:
    CK_API avkit_rtsp_source();
    CK_API virtual ~avkit_rtsp_source() throw();

    CK_API virtual void run();
    CK_API virtual void stop();
    CK_API virtual bool get( std::shared_ptr<avkit::av_packet>& pkt );
    CK_API virtual void set_param( const cppkit::ck_string& name, const cppkit::ck_string& val );
    CK_API virtual void commit_params();

private:
    avkit_rtsp_source( const avkit_rtsp_source& );
    avkit_rtsp_source& operator = ( const avkit_rtsp_source& );

    cppkit::ck_string _rtspURL;
    std::shared_ptr<avkit::av_demuxer> _demux;
    bool _gotClockRate;
    std::pair<int,int> _clockRate;
    int64_t _pts;
    std::map<std::string, cppkit::ck_string> _params;
    bool _running;
    bool _demuxCBFired;
    bool _firstKey;
};

#endif
