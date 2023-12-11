cmake_minimum_required (VERSION 3.13)


if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

file(GLOB_RECURSE CommonSources *.nothing)
SOURCE_GROUP("common" FILES ${CommonSources})

SET(CommonSources "${CommonSources}" CACHE INTERNAL "CommonSources")



    function(add_module name)
        message("Module target ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}")

        add_custom_target(${name}.gcm
                COMMAND
                    ${CMAKE_CXX_COMPILER}
                    "-std=c++20"
                    "-fmodules-ts"
                    "-xc++"
                    -c
                    ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}
                )
    endfunction()

    function(add_system_header_unit name)
        add_custom_target(${name}.gcm
                COMMAND
                    ${CMAKE_CXX_COMPILER}
                    "-std=c++20"
                    "-fmodules-ts"
                    "-xc++-system-header"
                    "-nostartfiles" #???? shouldn't be here?!
                    ${name}
                )
    endfunction()


    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gcm.cache)

    # GCC Bug?! https://stackoverflow.com/questions/67030839/how-to-use-modules-in-c-20-using-g
    #add_module(interceptCommon interceptCommon.ixx)

    #file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/interceptCommon.ixx DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/GCCBugFix.cpp)
    #configure_file(${CMAKE_CURRENT_SOURCE_DIR}/interceptCommon.ixx ${CMAKE_CURRENT_SOURCE_DIR}/GCCBugFix.cpp COPYONLY)
    add_module(interceptCommon interceptCommon.cpp)
    add_custom_target(gccBugFixFile ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "interceptCommon.ixx" "${CMAKE_CURRENT_SOURCE_DIR}/interceptCommon.cpp")
    add_dependencies(interceptCommon.gcm gccBugFixFile)


    #add_dependencies(interceptCommon.gcm gccBugFixFile)



    #add_system_header_unit(string_view)
    #add_system_header_unit(filesystem)
    #add_system_header_unit(vector)
    #add_system_header_unit(array)
    #add_system_header_unit(charconv)
    #add_system_header_unit(span)
    #add_system_header_unit(cstdint)
    #add_system_header_unit(functional)
    #add_system_header_unit(typeinfo)
    #add_system_header_unit(optional)
    #add_dependencies(interceptCommon.gcm filesystem.gcm string_view.gcm vector.gcm array.gcm charconv.gcm span.gcm cstdint.gcm functional.gcm typeinfo.gcm optional.gcm)


    add_custom_target(gcmLinkCommon ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "../gcm.cache" "gcm.cache") # #TODO custom for plugin template
    add_dependencies(interceptCommon.gcm gcmLinkCommon)

    # g++ -std=c++20 -fmodules-ts -xc++-system-header iostream
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

    function(add_module name)
        message("Module target ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}")

        file(MAKE_DIRECTORY ${PREBUILT_MODULE_PATH})
        add_custom_target(${name}.pcm
                COMMAND
                    ${CMAKE_CXX_COMPILER}
                    -std=c++20
                    -stdlib=libc++
                    -fmodules-ts


                    #-fbuiltin-module-map
                    -fno-implicit-module-maps
                    -fno-implicit-modules
                    #-fimplicit-module-maps
                    -fprebuilt-module-path=${PREBUILT_MODULE_PATH}

                    -c
                    #-x c++-module
                    ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}
                    -Xclang -emit-module-interface
                    -o ${PREBUILT_MODULE_PATH}/${name}.pcm

                )
    endfunction()




    add_module(InterceptCommon interceptCommon.cpp)
    # clang bug, doesn't handle .ixx even though it says it should, prints obscure error message about how -std=c++20 nor -fmodules-ts parameters are passed, even though they are!
    add_custom_target(gccBugFixFile ALL COMMAND ${CMAKE_COMMAND} -E create_symlink "interceptCommon.ixx" "${CMAKE_CURRENT_SOURCE_DIR}/interceptCommon.cpp")
    add_dependencies(InterceptCommon.pcm gccBugFixFile)

    message("CXX ${CMAKE_CXX_COMPILER}")


else()

file(GLOB_RECURSE CommonSources *.ixx *.cxx)
SOURCE_GROUP("common" FILES ${CommonSources})

SET(CommonSources "${CommonSources}" CACHE INTERNAL "CommonSources")


endif()
