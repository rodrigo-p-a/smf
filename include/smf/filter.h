
#ifndef __smf_filter_h
#define __smf_filter_h

#include "smf/stateful.h"
#include "cppkit/os/ck_exports.h"
#include "cppkit/ck_string.h"
#include "avkit/av_packet.h"
#include <memory>

namespace smf
{

// Why?
//     I find its useful to divide video work into 2 categories: stateless and stateful. Here are some examples so you
//     can understand what I'm talking about:
//
//     Stateless
//         - Transcode a file from one format or container to another
//         - Transcode an h264 i frame into a jpeg
//
//     Statefull
//         - Record an RTP stream to disk
//         - Display video with transport controls (ff, rew, etc)...
//
//     With stateless video work the application gathers some data, applies some transformations, creates some output
//     and returns it. The job has a beginning and a known end. With statefull video work the application creates a
//     long lived pipeline through which data flows and then interacts with the state of the pipeline. The main
//     difference between the two is that in one case the application knows the work terminates and in the other the
//     application sees the work as potentially infinite (or at least has no idea when the end will be).
//
//     I believe that using a stateful video processing library to solve stateless problems is a mistake because it adds
//     a lot of unneeded complexity. As an example, consider the case of transcoding a clip from one format to another.
//     The job consists of reading frames from an input container, decoding them, re-encoding them and then finally
//     recontainerizing them. No part of this process requires a stateful pipeline and if you were forced to solve this
//     problem with a stateful pipeline it is very likely that you would call Run() on the pipeline one time only and
//     would have no need for features like "pause" or "frame forward". You would also discover that quite often the
//     precise configuration of one object depends on the output state of another (which doesn't lend itself to a
//     pipeline very easily as Pipelines tend to want to create all of their elements up front). Further, this process
//     also typically requires special work be done at the beginning or end of the process (for example, finalizing a
//     file in ffmpeg) which is complicated by stateful pipelines because they tend to hide all of their data on the
//     inside. All that said, sometimes you really need a stateful solution.
//
//     One of the things I have learned the hard way is that sometimes you cannot solve everyones problems, and in those
//     situations you should not try. Instead, the ideal strategy is to provide a tool box consisting of useful elements
//     with which they can fashion their own specific solutions. smf accomplishes this by providing a software "stack"
//     that allows the application developer to choose the level of abstraction appropriate to their problem.
//
//     At the lowest layer, smf provides filter objects. filter objects are little more than composable functions that
//     take some well defined input and make some well defined output. filter objects should not have threads, and they
//     know nothing about state. A filter should succeed and return a result or throw an error.
//
//     At the next level of abstraction filter objects can be grouped into filter_groups, which are themselves filter
//     objects. This allows an arbitrarily long and complex sequence of composable operations to be to be treated as a
//     single unit. filter_groups work internally by simply passing the output of one filter to the input of the next.
//     Just like filter's, filter_groups have no concept of state and should not have any threads.
//
//     media_objects are the next level of abstraction. media_objects provide multithreading and buffering and are the
//     first object types with state. media_objects are always associated with another object (via composition) to
//     perform the media specific work, and media_objects have the capability of being linked.
//
//     Finally, at the last level of abstraction we provide Control objects, that allow you to group media_objects and
//     control their state (Run(), stop(), etc) in a single place.

class filter : public stateful
{
public:
    CK_API filter() = default;
    CK_API virtual ~filter() throw() {}

    CK_API virtual std::shared_ptr<avkit::av_packet> process( std::shared_ptr<avkit::av_packet> pkt ) = 0;

    CK_API virtual void set_param( const cppkit::ck_string& name, const cppkit::ck_string& val ) = 0;
    CK_API virtual void commit_params() = 0;

    CK_API virtual void run() {}
    CK_API virtual void stop() {}
};

}

#endif
