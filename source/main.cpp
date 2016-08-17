
#include "smf/source.h"
#include "smf/filter.h"
#include "smf/filter_group.h"
#include "smf/control.h"
#include "cppkit/ck_string.h"
#include "cppkit/os/ck_exports.h"
#include "cppkit/os/ck_time_utils.h"
#include <memory>

using namespace smf;
using namespace cppkit;
using namespace std;

class fake_source : public source
{
public:
    CK_API fake_source() :
        source()
    {
    }

    CK_API virtual ~fake_source() throw()
    {
    }

    CK_API virtual bool get( std::shared_ptr<avkit::av_packet>& pkt )
    {
        ck_usleep( 15000 + (random() % 15000) );
        pkt = make_shared<avkit::av_packet>( 1024 );
        return true;
    }

    CK_API virtual void run() { }
    CK_API virtual void stop() { }

private:
};

class random_sleep : public filter
{
public:
    CK_API random_sleep() :
        filter()
    {
    }

    CK_API virtual ~random_sleep() throw()
    {
    }

    CK_API virtual std::shared_ptr<avkit::av_packet> process( std::shared_ptr<avkit::av_packet> pkt )
    {
        ck_usleep( random() % 1000000 );
        return pkt;
    }
};

// printer is a filter object that simply prints a period and flushes stdout when a buffer arrives.

class printer : public filter
{
public:
    CK_API printer( const cppkit::ck_string& ch ) :
        filter(),
        _ch( ch )
    {
    }

    CK_API virtual ~printer() throw() {}

    CK_API virtual std::shared_ptr<avkit::av_packet> process( std::shared_ptr<avkit::av_packet> pkt )
    {
        printf("%s",_ch.c_str());
        fflush(stdout);
        return pkt;
    }
private:
    cppkit::ck_string _ch;
};

int main( int argc, char* argv[] )
{
    try
    {
        std::shared_ptr<control> c = make_shared<control>();

        // with this technique, the application never directly touches a media_object. Instead, everything is done
        // indirectly through a control object.

        c->add_source( "source", make_shared<fake_source>() );

        // In this example, 3 separate filters are grouped and then added to our control. Grouping them here means
        // that they will all assigned to the same media object (and hence, run on the same thread).
        std::shared_ptr<filter_group> fg = make_shared<filter_group>();
        fg->add( make_shared<printer>( "-" ) );
        fg->add( make_shared<random_sleep>() );
        fg->add( make_shared<printer>( "=" ) );

        c->add_filter( "fg", fg );

        c->connect( "source", "fg" );

        for( int i = 0; i < 10; i++ )
        {
            c->run();

            ck_usleep( random() % 2000000 );

            c->stop();
        }
    }
    catch( exception& ex )
    {
        printf("%s\n",ex.what());
        fflush(stdout);
    }

    return 0;
}
