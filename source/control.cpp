
#include "smf/control.h"

using namespace smf;
using namespace cppkit;
using namespace std;

control::control() :
    stateful(),
    _elementIndex(),
    _elements(),
    _kvs()
{
}

control::~control() throw()
{
    stop();
}

void control::run()
{
    list<shared_ptr<media_object> >::reverse_iterator i, esentry;
    for( i = _elements.rbegin(), esentry = _elements.rend(); i != esentry; i++ )
        (*i)->run();
}

void control::stop()
{
    list<shared_ptr<media_object> >::reverse_iterator i, esentry;
    for( i = _elements.rbegin(), esentry = _elements.rend(); i != esentry; i++ )
        (*i)->stop();
}

void control::set_param( const cppkit::ck_string& key, const ck_string& name, const ck_string& val )
{
    _elementIndex[key]->set_param( name, val );
}

void control::commit_params( const cppkit::ck_string& key )
{
    _elementIndex[key]->commit_params();
}

void control::add_filter( const ck_string& key, shared_ptr<filter> filter )
{
    shared_ptr<media_object> p = make_shared<media_object>();
    p->attach_filter( filter );

    _elementIndex.insert( make_pair(key, p) );
    _elements.push_back( p );
}

void control::add_source( const ck_string& key, shared_ptr<source> source )
{
    shared_ptr<media_object> p = make_shared<media_object>();
    p->set_source( source );

    _elementIndex.insert( make_pair(key, p) );
    _elements.push_back( p );
}

void control::connect( const ck_string& keyA, const ck_string& keyB )
{
    _elementIndex[keyA]->connect( _elementIndex[keyB] );
}

void control::add_media_object( const ck_string& key, shared_ptr<media_object> mo )
{
    _elementIndex.insert( make_pair(key, mo) );
    _elements.push_back( mo );
}

std::shared_ptr<media_object> control::get_media_object( const cppkit::ck_string& key )
{
    auto found = _elementIndex.find( key );
    if( found == _elementIndex.end() )
        CK_THROW(("Unable to file media object with name: %s",key.c_str()));
    return found->second;
}

bool control::healthy() const
{
    bool healthy = true;
    for( auto& mo : _elements )
    {
        if( !mo->healthy() )
            healthy = false;
    }
    return healthy;
}

void control::set_kv( const ck_string& key, const ck_string& val )
{
    _kvs[key.to_std_string()] = val;
}

ck_nullable<ck_string> control::get_kv( const ck_string& key ) const
{
    ck_nullable<ck_string> result;

    auto f = _kvs.find(key.to_std_string());
    if(f != _kvs.end())
        result.set_value(f->second);

    return result;
}
