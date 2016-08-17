
#ifndef __smf_stateful_h
#define __smf_stateful_h

#include "cppkit/os/ck_exports.h"

namespace smf
{

class stateful
{
public:
    CK_API stateful() = default;
    CK_API virtual ~stateful() noexcept {}

    CK_API virtual void run() = 0;
    CK_API virtual void stop() = 0;
};

}

#endif
