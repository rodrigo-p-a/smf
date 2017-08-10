
#include "smf/plugin_factory.h"
#include "cppkit/ck_path.h"

using namespace smf;
using namespace cppkit;
using namespace std;

extern "C" typedef void* (*create_plugin)();

plugin_factory::plugin_factory( const ck_string& pluginDir ) :
    _pluginDir( pluginDir ),
    _libs(),
    _sourcePluginMap(),
    _filterPluginMap()
{
    ck_path path( pluginDir );

    // iterate through our plugin dir and for each file
    //    - see if it has a create_plugin entry point
    //    - ask plugins for the source names they create
    //    - ask plugins for the filter names they create
    //    - build a map between source names and the plugin that creates them and filter names and the plugin that
    //      creates them.

    ck_string fileName;
    while( path.read_dir( fileName ) )
    {
        try
        {
            std::shared_ptr<ck_dynamic_library> lib = make_shared<ck_dynamic_library>();
            lib->load( pluginDir + PATH_SLASH + fileName );

            create_plugin cpf = (create_plugin)lib->resolve_symbol( "create_plugin" );

            if( !cpf )
                CK_THROW(("Unable to locate plugin entry point!"));

            _libs.push_back( lib );

            shared_ptr<plugin> p( (plugin*)cpf() );

            list<ck_string> sourceNames = p->get_source_names();

            for( const ck_string& sourceName : sourceNames )
                _sourcePluginMap.insert( make_pair(sourceName, p) );

            list<ck_string> filterNames = p->get_filter_names();

            for( const ck_string& filterName : filterNames )
                _filterPluginMap.insert( make_pair(filterName, p) );
        }
        catch(std::exception& ex)
        {
            CK_LOG_ERROR( "Encountered and error: %s while opening %s", ex.what(), fileName.c_str() );
        }
        catch(...)
        {
            CK_LOG_ERROR( "Encountered an unknown error while opening: %s", fileName.c_str() );
        }
    }
}

shared_ptr<source> plugin_factory::create_source( const ck_string& name ) const
{
    return _sourcePluginMap.at(name)->create_source( name );
}

shared_ptr<filter> plugin_factory::create_filter( const ck_string& name ) const
{
    return _filterPluginMap.at(name)->create_filter( name );
}
