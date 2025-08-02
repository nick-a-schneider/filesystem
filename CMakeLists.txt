cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(filesystem C)
option(BUILD_TESTS "Build unit tests" OFF)

include(FetchContent)
if(NOT TARGET bitmap_allocator)
    message(STATUS "Fetching bitmap_allocator...")
    FetchContent_Declare(
        bitmap_allocator
        GIT_REPOSITORY https://github.com/nick-a-schneider/bitmap_allocator.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(bitmap_allocator)
endif()

if(NOT TARGET bitmap_allocator)
    message(FATAL_ERROR "Failed to fetch bitmap_allocator")
endif()

add_library(filesystem STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/object_table.c
)

target_link_libraries(filesystem PUBLIC
    bitmap_allocator
)

target_include_directories(filesystem PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(filesystem::filesystem ALIAS filesystem)

install(TARGETS filesystem
    EXPORT filesystemTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(EXPORT filesystemTargets
    FILE filesystemTargets.cmake
    NAMESPACE filesystem::
    DESTINATION lib/cmake/filesystem
)

install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()
