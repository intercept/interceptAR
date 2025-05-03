cmake_minimum_required (VERSION 3.30)

add_library(InterceptCommon)

file(GLOB_RECURSE CommonSources *.cpp)
file(GLOB_RECURSE CommonModules *.ixx)
target_sources(InterceptCommon
    PRIVATE
        ${CommonSources}
    PUBLIC
        FILE_SET CXX_MODULES
        FILES ${CommonModules}
)

# Tell CMake we're using C++23 but only C++20 is needed to consume it.
target_compile_features(InterceptCommon
  PRIVATE   cxx_std_23
  INTERFACE cxx_std_20
  PUBLIC cxx_std_20)
