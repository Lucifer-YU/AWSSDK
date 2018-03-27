cmake_minimum_required (VERSION 2.6)
project (awsfx)

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Display verbose info
set (CMAKE_VERBOSE_MAKEFILE ON)

# Setting up version numbers
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

# Find dependencies
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
find_package(CURL)
if (NOT CURL_FOUND)
    message(FATAL_ERROR "libcurl is not found!")
endif ()
find_package(LibXml2)
if (NOT LibXml2_FOUND)
    message(FATAL_ERROR "libxml2 is not found!")
endif ()
find_package(Iconv)
if (NOT Iconv_FOUND)
    message(FATAL_ERROR "libiconv is not found!")
endif ()
find_package(OPENSSL)
if (NOT OPENSSL_FOUND)
    message(FATAL_ERROR "libopenssl is not found!")
endif ()

# Configuration files
configure_file (
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h
)
if (UNIX)
	set (CMAKE_C_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ") # –pg
    set (CMAKE_C_FLAGS_RELEASE "$ENV{CXXFLAGS} -DNDEBUG -O3 -Wall ")
elseif (WIN32)
    set (CMAKE_C_FLAGS_DEBUG "-D_CRT_SECURE_NO_WARNINGS ")
    set (CMAKE_C_FLAGS_RELEASE "-D_CRT_SECURE_NO_WARNINGS -DNDEBUG ")
endif ()

# Header & source files
set (CMAKE_SOURCE_DIR .)
include_directories (
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}
)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/Foundation awsfx_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/IO awsfx_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/AWS awsfx_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/JSON awsfx_SRCS)

include_directories (
    ${LIBXML2_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
    ${Iconv_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

# The targets
set(TARGET awsfx)
add_library(${TARGET} STATIC ${awsfx_SRCS})

# Macro for add example target
macro(add_example_target EXAMPLE_TARGET)
    unset (example_SRC)
    aux_source_directory(examples/${EXAMPLE_TARGET} example_SRC)
    add_executable("example_${EXAMPLE_TARGET}" ${example_SRC})
    target_link_libraries("example_${EXAMPLE_TARGET}" awsfx ${Iconv_LIBRARY} ${LIBXML2_LIBRARIES} ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES})
endmacro(add_example_target)

add_example_target(SQSTest)
