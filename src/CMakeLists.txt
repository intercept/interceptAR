cmake_minimum_required (VERSION 3.12)
project (Intercept2 CXX ASM)

find_package (Threads)

if(MSVC)
    set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /std:c++latest" )
#elseif(MSVC)
#    message(FATAL_ERROR "ERROR: You need a C++17 compatible compiler")
endif()

message("GENERATOR USED: '${CMAKE_GENERATOR}'")
message("COMPILER USED: '${CMAKE_CXX_COMPILER_ID}'")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/release/")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)

SET(CMAKE_INCLUDE_CURRENT_DIR ON) 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Global flags I want in every project
add_definitions(/DNOMINMAX)

add_subdirectory(common)
if(EXISTS "${PROJECT_SOURCE_DIR}/host/CMakeLists.txt")
    add_subdirectory(host)
endif()
add_subdirectory(examplePlugin)
