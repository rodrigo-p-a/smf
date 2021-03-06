
#ifndef __smf_stateful_h
#define __smf_stateful_h

#include "cppkit/os/ck_exports.h"

namespace smf
{

class stateful
{
public:
    CK_API stateful() = default;
    CK_API virtual ~stateful() throw() {}

    CK_API virtual void run() = 0;
    // stop() should never throw, but is not marked nothrow
    // because our media objects actually handle this condition (sort of).
    CK_API virtual void stop() = 0;
};

}

#endif
