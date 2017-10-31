
#ifndef __smf_quartz_h
#define __smf_quartz_h

#include "smf/filter.h"
#include "cppkit/os/ck_exports.h"
#include <map>
#include <mutex>
#include <future>

namespace smf
{

class quartz final : public filter
{
public:
    CK_API quartz();
    CK_API virtual ~quartz() throw();

    CK_API virtual void run();
    CK_API virtual void stop();

    CK_API virtual std::shared_ptr<avkit::av_packet> process( std::shared_ptr<avkit::av_packet> pkt );

    CK_API virtual void set_param( const cppkit::ck_string& name, const cppkit::ck_string& val ) {}
    CK_API virtual void commit_params() {}

private:
    uint32_t _freq;
    bool _firstAfterRun;
    std::chrono::steady_clock::time_point _runStartTime;
    std::chrono::milliseconds _streamTime;
    int64_t _lastPTS;
};

}

#endif
