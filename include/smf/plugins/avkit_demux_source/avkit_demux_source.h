
#ifndef __avkit_demux_source_h
#define __avkit_demux_source_h

#include "smf/source.h"
#include "cppkit/ck_string.h"
#include "avkit/av_demuxer.h"
#include <memory>
#include <map>
#include <string>

class avkit_demux_source_test;

namespace smf
{

class avkit_demux_source : public source
{
    friend class ::avkit_demux_source_test;

public:
    CK_API avkit_demux_source();
    CK_API virtual ~avkit_demux_source() throw();

    CK_API virtual void run();
    CK_API virtual void stop();
    CK_API virtual bool get( std::shared_ptr<avkit::av_packet>& pkt );
    CK_API virtual void set_param( const cppkit::ck_string& name, const cppkit::ck_string& val );
    CK_API virtual void commit_params();

private:
    avkit_demux_source( const avkit_demux_source& );
    avkit_demux_source& operator = ( const avkit_demux_source& );

    cppkit::ck_string _fileName;
    std::shared_ptr<avkit::av_demuxer> _demux;
    std::map<std::string, cppkit::ck_string> _params;
    bool _running;
};

}

#endif
