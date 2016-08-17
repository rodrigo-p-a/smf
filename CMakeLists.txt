cmake_minimum_required(VERSION 2.8)
project(smf)

set(SOURCES source/control.cpp
            source/media_object.cpp
            source/plugin_factory.cpp
            source/plugin.cpp
            source/plugins/avkit_decoder/avkit_decoder.cpp
	    source/plugins/avkit_rtsp_source/avkit_rtsp_source.cpp)

set(COMMON_LIBS cppkit avkit)

include(common.cmake NO_POLICY_SCOPE)
include(${devel_artifacts_path}/build/base_lib.cmake NO_POLICY_SCOPE)
