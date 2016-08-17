
#ifndef __smf_plugin_h
#define __smf_plugin_h

#include "smf/source.h"
#include "smf/filter.h"
#include "cppkit/ck_string.h"
#include <list>
#include <memory>

namespace smf
{

class plugin
{
public:
    CK_API plugin();
    CK_API virtual ~plugin() noexcept;

    CK_API virtual std::list<cppkit::ck_string> get_source_names() = 0;
    CK_API virtual std::list<cppkit::ck_string> get_filter_names() = 0;

    CK_API virtual std::shared_ptr<source> create_source( const cppkit::ck_string& name ) = 0;
    CK_API virtual std::shared_ptr<filter> create_filter( const cppkit::ck_string& name ) = 0;
};

}

#endif
