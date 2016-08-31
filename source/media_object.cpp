
#include "smf/media_object.h"
#include "cppkit/ck_exception.h"

using namespace smf;
using namespace cppkit;
using namespace avkit;
using namespace std;

media_object::media_object() :
    stateful(),
    _next(),
    _inputQueue(),
    _queueLimit( 5 ),
    _queueLock(),
    _queueCond(),
    _producerLock(),
    _producerCond(),
    _running( false ),
    _filter(),
    _source(),
    _errorState( false ),
    _thread()
{
}

void media_object::run()
{
    if( !_running )
    {
        _running = true;

        if( _source )
            _source->run();
        else _filter->run();

        _thread = thread( &media_object::entry_point, this );
    }
}

void media_object::stop()
{
    if( _running )
    {
        _running = false;

        if( _source )
        {
            _source->stop();
        }
        else
        {
            _filter->stop();

            {
                unique_lock<recursive_mutex> g( _producerLock );
                _producerCond.notify_one();
            }

            {
                unique_lock<recursive_mutex> queueGuard( _queueLock );
                _queueCond.notify_one();
            }
        }

        _thread.join();
    }
}

void media_object::set_param( const ck_string& name, const ck_string& val )
{
    if( _source )
        _source->set_param( name, val );

    if( _filter )
        _filter->set_param( name, val );
}

void media_object::commit_params()
{
    if( _source )
        _source->commit_params();

    if( _filter )
        _filter->commit_params();
}

void media_object::write( shared_ptr<av_packet> pkt )
{
    if( !_running )
        CK_THROW(("Unable to write() to non running media_object."));

    if( _filter )
    {
        try
        {
            pkt = _filter->process( pkt );
        }
        catch( exception& ex )
        {
            _errorState = true;
            CK_LOG_ERROR( "MEDIA_OBJECT: %s", ex.what() );
        }
    }

    if( !_next.empty() && !_errorState )
    {
        list<shared_ptr<media_object> >::iterator i, sentry;
        for( i = _next.begin(), sentry = _next.end(); i != sentry; i++ )
            (*i)->_enqueue( pkt );
    }
}

void media_object::_enqueue( shared_ptr<av_packet> pkt )
{
    if( !_running || _errorState )
        return;

    unique_lock<recursive_mutex> prodGuard( _producerLock );

    // First, we must check if the queue is full and wait until it isn't.

    bool queueFull = false;

    do
    {
        // We briefly acquire the queue lock to check if the queue is full. We can't hold this
        // this lock because then the consumer wouldn't be able to drain it.
        {
            unique_lock<recursive_mutex> queueGuard( _queueLock );
            queueFull = _queue_full();
        }

        // What if the queue is drained here?

        // Only if the queue was actually full should we wait. While waiting, the producer lock is
        // released (thus allowing the consumer to signal us, should it consume a buffer );

        if( queueFull )
            _producerCond.wait_for( prodGuard, chrono::milliseconds(100), [&](){return !(queueFull && _running);} );

        //_producerCond.wait( prodGuard, [&](){return !(queueFull && _running);} );
    } while( queueFull && _running );

    // Finally, we must have a free slot so append it and signal the consumer.
    if( (!queueFull) && _running )
    {
        unique_lock<recursive_mutex> queueGuard( _queueLock );
        _inputQueue.push_back( pkt );
        _queueCond.notify_one();
    }
    else CK_LOG_NOTICE("REMOVE ME: DROPPAGE in media_object.");
}

void* media_object::_default_entry_point()
{
    while( _running )
    {
        shared_ptr<av_packet> pkt;

        {
            unique_lock<recursive_mutex> g( _queueLock );

            if( !_inputQueue.empty() )
            {
                pkt = _inputQueue.front();
                _inputQueue.pop_front();
            }
        }

        if( pkt )
        {
            {
                unique_lock<recursive_mutex> g( _producerLock );
                _producerCond.notify_one();
            }

            write( pkt );
        }
        else
        {
            unique_lock<recursive_mutex> g( _queueLock );

            _queueCond.wait( g, [&](){return !(_inputQueue.empty() && _running);} );
        }
    }

    return NULL;
}

void* media_object::_source_entry_point()
{
    while( _running )
    {
        shared_ptr<av_packet> pkt;

        bool gotav_packet = false;

        try
        {
            gotav_packet = _source->get( pkt );
        }
        catch( exception& ex )
        {
            CK_LOG_NOTICE("%s",ex.what());
            _errorState = true;
        }

        if( _running && gotav_packet && !_errorState )
            write( pkt );
    }

    return NULL;
}

void* media_object::entry_point()
{
    if( _source )
        _source_entry_point();
    else _default_entry_point();

    return NULL;
}
