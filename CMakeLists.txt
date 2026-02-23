cmake_minimum_required(VERSION 3.20)

project(onion_event LANGUAGES CXX)

# ---- Library ----
add_library(onion_event INTERFACE)
add_library(onion::event ALIAS onion_event)
target_include_directories(onion_event INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
 
set_target_properties(onion_event PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# ---- Demo ----
option(ONION_EVENT_BUILD_DEMO "Build Event demo" ON)

if (ONION_EVENT_BUILD_DEMO)
    add_subdirectory(demo)
endif()
