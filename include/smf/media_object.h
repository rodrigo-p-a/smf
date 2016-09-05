
#ifndef __smf_media_object_h
#define __smf_media_object_h

#include "smf/source.h"
#include "smf/filter.h"
#include "avkit/av_packet.h"

#include <mutex>
#include <thread>
#include <condition_variable>
#include <list>
#include <memory>

namespace smf
{

class media_object : public stateful
{
public:
    CK_API media_object();
    CK_API virtual ~media_object() throw() {}

    CK_API virtual void run();
    CK_API virtual void stop();

    CK_API void set_param( const cppkit::ck_string& name, const cppkit::ck_string& val );
    CK_API void commit_params();

    CK_API bool healthy() const { return !_errorState; }

    CK_API void write( std::shared_ptr<avkit::av_packet> pkt );

    CK_API void set_source( std::shared_ptr<source> source ) { _source = source; }

    CK_API void disconnect_all() { _next.clear(); }

    CK_API void connect( std::shared_ptr<media_object> next ) { _next.push_back( next ); }

    CK_API void attach_filter( std::shared_ptr<filter> filter ) { _filter = filter; }

    CK_API virtual void* entry_point();

private:
    inline bool _queue_full() const
    {
        if( _inputQueue.size() >= _queueLimit )
            return true;
        return false;
    }

    void _enqueue( std::shared_ptr<avkit::av_packet> pkt );
    void* _default_entry_point();
    void* _source_entry_point();

    std::list<std::shared_ptr<media_object> > _next;
    std::list<std::shared_ptr<avkit::av_packet> > _inputQueue;
    size_t _queueLimit;
    std::recursive_mutex _queueLock;
    std::condition_variable_any _queueCond;
    std::recursive_mutex _producerLock;
    std::condition_variable_any  _producerCond;
    bool _running;
    std::shared_ptr<filter> _filter;
    std::shared_ptr<source> _source;
    bool _errorState;
    std::thread _thread;
};

}

#endif
