cmake_minimum_required(VERSION 3.12.3)
project (mailer)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
add_library(mailer STATIC mailer.cpp)
list(APPEND CMAKE_PREFIX_PATH $ENV{LIB_CURL})
list(APPEND CMAKE_LIBRARY_PATH $ENV{LIB_CURL})

find_package(curl)

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries (mailer ${CURL_LIBRARIES})
endif (CURL_FOUND)

install(TARGETS mailer DESTINATION lib)
install(FILES mailer.h DESTINATION include)