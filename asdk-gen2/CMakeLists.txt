CMAKE_MINIMUM_REQUIRED(VERSION 3.24)

MESSAGE("CMake version:" ${CMAKE_VERSION})

SET(PROJECT_NAME "ASDK-GEN2")

### enable cross-compilation ###

SET(CMAKE_SYSTEM_NAME Generic)

### top-level cmake includes ###

INCLUDE(cmake/macro.cmake)
INCLUDE(platform/platform.cmake)
INCLUDE(cmake/helper_functions.cmake)

### top-level project ###

PROJECT(${PROJECT_NAME})

### use c language

ENABLE_LANGUAGE(C ASM)

### build asdk = build platform, middleware and RTOS

ASDK_GEN_VERSION_INFO()

# identify target platform

ADD_SUBDIRECTORY(platform)

# identify required middleware modules

ADD_SUBDIRECTORY(middleware)

# identify required library modules

ADD_SUBDIRECTORY(lib)

# create asdk interface

ADD_LIBRARY(asdk INTERFACE)

ADD_DEPENDENCIES(asdk platform)
ADD_DEPENDENCIES(asdk middleware)
ADD_DEPENDENCIES(asdk lib)

# expose include directories of lower layers

target_include_directories(
    asdk
    INTERFACE 
        common
        platform
        middleware
        lib
)

# expose link time symbols of lower layers

target_link_libraries(
    asdk
    INTERFACE
        platform
        middleware
        lib
)
