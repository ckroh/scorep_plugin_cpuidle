cmake_minimum_required(VERSION 2.8)
project(cpuidle_plugin)

set(SCOREP_FOUND false)
option(BACKEND_SCOREP "Build plugin using scorep(ON) or vampirtrace(OFF)" ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/common;${CMAKE_MODULE_PATH}")
set(CMAKE_PREFIX_PATH "$/opt/scorep;${CMAKE_PREFIX_PATH}")

# Intialize git submodules if not done already
file(GLOB SUBMODULE_FILES "${CMAKE_SOURCE_DIR}/common/*")
list(LENGTH SUBMODULE_FILES COUNT_COMMON)
if(${COUNT_COMMON} EQUAL 0)
    message(STATUS "Initializing git submodule")
    execute_process(COMMAND "git" "submodule" "init" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    execute_process(COMMAND "git" "submodule" "update" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

#include(common/DefaultBuildType.cmake)


if(BACKEND_SCOREP)
    include(common/FindScorep.cmake)
    if(SCOREP_FOUND)
        include_directories(${SCOREP_INCLUDE_DIRS})
        add_definitions("-DBACKEND_SCOREP")
    else()
        message("Score-P was not found, falling back to VampirTrace!")
    endif()
endif()

find_package(Scorep REQUIRED)
include(ScorepCXXPlugin.cmake)

include_directories(include)

add_library(${PROJECT_NAME} SHARED scorep_cpuidle_plugin.cpp)

#target_link_libraries(${PROJECT_NAME} ${X86_ADAPT_LIBRARIES})

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
