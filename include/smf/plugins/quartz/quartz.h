
#ifndef __smf_quartz_h
#define __smf_quartz_h

#include "smf/filter.h"
#include "cppkit/os/ck_exports.h"
#include <map>
#include <mutex>

namespace smf
{

class quartz final : public filter
{
public:
    CK_API quartz();
    CK_API virtual ~quartz() throw();

    CK_API virtual std::shared_ptr<avkit::av_packet> process( std::shared_ptr<avkit::av_packet> pkt );

    CK_API virtual void set_param( const cppkit::ck_string& name, const cppkit::ck_string& val );
    CK_API virtual void commit_params();

private:
    std::map<std::string, cppkit::ck_string> _params;
    int _timeBaseNum;
    int _timeBaseDen;
};

}

#endif
