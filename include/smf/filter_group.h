
#ifndef __smf_filter_group_h
#define __smf_filter_group_h

#include "smf/filter.h"

#include "avkit/av_packet.h"
#include <list>
#include <memory>

namespace smf
{

class filter_group : public filter
{
public:
    CK_API filter_group() = default;
    CK_API virtual ~filter_group() noexcept {}

    CK_API void add( std::shared_ptr<filter> filter ) { _filters.push_back( filter ); }

    CK_API virtual std::shared_ptr<avkit::av_packet> process( std::shared_ptr<avkit::av_packet> pkt )
    {
        for( std::shared_ptr<filter> f : _filters )
            pkt = f->process( pkt );

        return pkt;
    }

    CK_API virtual void set_param( const cppkit::ck_string&, const cppkit::ck_string& )
    {
        CK_THROW(("Cannot set parameters on a filter_group as a whole."));
    }

    CK_API virtual void commit_params()
    {
        CK_THROW(("Cannot commit parameters on a filter_group as a whole."));
    }

private:
    std::list<std::shared_ptr<filter> > _filters;
};

}

#endif
