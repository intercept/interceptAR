cmake_minimum_required (VERSION 3.13)

file(GLOB_RECURSE CommonSources *.ixx)
SOURCE_GROUP("common" FILES ${CommonSources})

SET(CommonSources "${CommonSources}" CACHE INTERNAL "CommonSources")


