
#ifndef __smf_plugin_factory_h
#define __smf_plugin_factory_h

#include "smf/plugin.h"
#include "cppkit/ck_dynamic_library.h"
#include <map>
#include <string>
#include <memory>

namespace smf
{

class plugin_factory
{
public:
    CK_API plugin_factory( const cppkit::ck_string& pluginDir );
    CK_API virtual ~plugin_factory() noexcept {}

    CK_API std::shared_ptr<source> create_source( const cppkit::ck_string& name );
    CK_API std::shared_ptr<filter> create_filter( const cppkit::ck_string& name );

private:
    cppkit::ck_string _pluginDir;
    // NOTE: The _libs member MUST come before the plugin maps because the members are destroyed in reverse order
    // of their declaration.
    std::list<std::shared_ptr<cppkit::ck_dynamic_library> > _libs;
    std::map<std::string, std::shared_ptr<plugin> > _sourcePluginMap;
    std::map<std::string, std::shared_ptr<plugin> > _filterPluginMap;
};

}

#endif
