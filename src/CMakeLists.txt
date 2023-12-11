cmake_minimum_required (VERSION 3.12)
project (Intercept2 CXX ASM)

if(MSVC)
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest" )
else()

        # https://www.reddit.com/r/debian/comments/mmure2/comment/hwcffeo/?utm_source=share&utm_medium=web2x&context=3
        # https://stackoverflow.com/questions/17275348/how-to-specify-new-gcc-path-for-cmake
        # install clang

        # /nix/store/grd724gvgblkvklrmbdyqm5jp8phyb7r-cmake-3.23.2/bin/cmake -G "Ninja" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE:STRING="Debug" /home/wolf/projects/interceptar/src/src/CMakeLists.txt
        # and also do this https://stackoverflow.com/a/58338868
        # export CC=/nix/store/0vyacyskar330fsp3bnmk75brl1vlg6d-clang-wrapper-14.0.6/bin/clang
        # export CXX=/nix/store/0vyacyskar330fsp3bnmk75brl1vlg6d-clang-wrapper-14.0.6/bin/clang++
        # export CPLUS_INCLUDE_PATH=/nix/store/567idnfsci29qkwvpmy0yal96wy1qib3-gcc-12.1.0/include/c++/12.1.0/:/nix/store/567idnfsci29qkwvpmy0yal96wy1qib3-gcc-12.1.0/include/c++/12.1.0/x86_64-unknown-linux-gnu
        # export TMPDIR=$PREFIX/tmp

        if(EXISTS "/nix/store/0vyacyskar330fsp3bnmk75brl1vlg6d-clang-wrapper-14.0.6/bin/clang") # Dedmen hack to use my correct cland version
            set(CMAKE_C_COMPILER "/nix/store/0vyacyskar330fsp3bnmk75brl1vlg6d-clang-wrapper-14.0.6/bin/clang" CACHE PATH "" FORCE)
            set(CMAKE_CXX_COMPILER "/nix/store/0vyacyskar330fsp3bnmk75brl1vlg6d-clang-wrapper-14.0.6/bin/clang++" CACHE PATH "" FORCE)
            message("##################################################################################")
        endif()


    if(CMAKE_COMPILER_IS_GNUCXX)
        # https://www.reddit.com/r/debian/comments/mmure2/comment/hwcffeo/?utm_source=share&utm_medium=web2x&context=3
        # https://stackoverflow.com/questions/17275348/how-to-specify-new-gcc-path-for-cmake
        # install gcc12
        # then inside shell run `whereis gcc` to find path to gcc 12 install, and insert that below. Or just run a modern linux distribution that has gcc12 already
        # run cmake inside nix shell, I tried to get it working with visual studio remote linux compiling, but nothing really works to let it run inside the nix environemtn
        # cmake -G "Ninja" -DCMAKE_BUILD_TYPE:STRING="Debug" ../CMakeLists.txt

        if(EXISTS "/nix/store/567idnfsci29qkwvpmy0yal96wy1qib3-gcc-12.1.0/bin/gcc") # Dedmen hack to use my correct GCC version
            set(CMAKE_C_COMPILER "/nix/store/567idnfsci29qkwvpmy0yal96wy1qib3-gcc-12.1.0/bin/gcc" CACHE PATH "" FORCE)
            set(CMAKE_CXX_COMPILER "/nix/store/567idnfsci29qkwvpmy0yal96wy1qib3-gcc-12.1.0/bin/g++" CACHE PATH "" FORCE)
            message("##################################################################################")
        endif()
    endif()
endif()


find_package (Threads)

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
