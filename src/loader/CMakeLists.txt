cmake_minimum_required (VERSION 3.13)

file(GLOB_RECURSE LoaderSources *.hpp *.cpp *.ixx)
SOURCE_GROUP("loader" FILES ${HostSources})

add_executable(InterceptLoader SHARED ${LoaderSources} ${CommonSources})

set_target_properties(LoaderSources PROPERTIES OUTPUT_NAME "InterceptLoader")

if(CMAKE_COMPILER_IS_GNUCXX)
    #TODO
else()
    target_compile_options(
        InterceptLoader PRIVATE 
        "/MP"

        "$<$<CONFIG:Release>:/Zi>"
        "$<$<CONFIG:Debug>:/ZI>"

        "$<$<CONFIG:Release>:/MT>"
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<CONFIG:Release>:/Ox>"
        "$<$<CONFIG:Release>:/Ob2>"
        "$<$<CONFIG:Release>:/Oi>"
        "$<$<CONFIG:Release>:/Ot>"
    )
    target_compile_definitions(InterceptLoader PRIVATE 
        "$<$<CONFIG:Release>:RELEASE>"
    )

    target_link_options(InterceptLoader PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" "$<$<CONFIG:Release>:/OPT:ICF>" "/DEBUG:FULL") 
endif()

#Binary signing
if(EXISTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sign.bat" AND MSVC)
    add_custom_command(TARGET ${INTERCEPT_PLUGIN_NAME}
        POST_BUILD
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sign.bat
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif()
