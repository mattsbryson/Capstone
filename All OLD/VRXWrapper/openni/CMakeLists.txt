cmake_minimum_required (VERSION 2.6)

project (openni)

include_directories ("${OPENNI_INCLUDE}")
add_library (openni openni.cpp)


