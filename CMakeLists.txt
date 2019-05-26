cmake_minimum_required(VERSION 3.14)
project(Lab_WordCount)


set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

find_package(Boost REQUIRED COMPONENTS filesystem locale system)
find_package(Qt5Core REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(LibArchive REQUIRED)


INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Concurrent_INCLUDE_DIRS})
#


#get_target_property(QtCore_location Qt5::Core LOCATION)

add_executable(Lab_WordCount3_multi_thread src/Word_count3_multi_thread.cpp dependencies/config_reader.h dependencies/FileProcessor.cpp dependencies/FileProcessor.h dependencies/time_meter.h dependencies/thread_safe_queue.h)
target_link_libraries( Lab_WordCount3_multi_thread ${LibArchive_LIBRARIES} ${Boost_LIBRARIES} Qt5::Core Qt5::Concurrent)

#add_executable(Lab_WordCount2_multi_thread src/Word_count2_multi_thread.cpp dependencies/time_meter.h dependencies/thread_safe_queue.cpp dependencies/thread_safe_queue.h)
#target_link_libraries( Lab_WordCount2_multi_thread)