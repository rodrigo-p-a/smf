
#ifndef __smf_control_h
#define __smf_control_h

#include "smf/stateful.h"
#include "smf/media_object.h"
#include "smf/filter.h"

#include "cppkit/ck_string.h"
#include <memory>
#include <list>
#include <map>

namespace smf
{

class control : public stateful
{
public:
    CK_API control();
    CK_API virtual ~control() throw();

    CK_API virtual void run();
    CK_API virtual void stop();

    CK_API void set_param( const cppkit::ck_string& key, const cppkit::ck_string& name, const cppkit::ck_string& val );
    CK_API void commit_params( const cppkit::ck_string& key );

    CK_API void add_filter( const cppkit::ck_string& key, std::shared_ptr<filter> filter );

    CK_API void add_source( const cppkit::ck_string& key, std::shared_ptr<source> source );

    CK_API void connect( const cppkit::ck_string& keyA, const cppkit::ck_string& keyB );

    CK_API void add_media_object( const cppkit::ck_string& key, std::shared_ptr<media_object> mo );

    CK_API bool healthy() const;

private:
    std::map<std::string,std::shared_ptr<media_object> > _elementIndex;
    std::list<std::shared_ptr<media_object> > _elements;
};

}

#endif
