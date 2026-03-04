cmake_minimum_required(VERSION 3.26)
project(MessageToObjects
        VERSION 1.0.0
        DESCRIPTION "A library to aid polymorphic creation of objects from string-messages"
        HOMEPAGE_URL "https://github.com/kingkybel/MessageToObjects")

include(GNUInstallDirs)
include(CTest)

# Use shared settings from cmake-common submodule
include(cmake-common/cmake/DkybBuildSettings.cmake)
dkyb_apply_common_settings(CXX_STANDARD 23)

execute_process(
        COMMAND bash -c "printf \"%s\" $(cat /etc/os-release | grep -E \"^ID=[^\\n]*|^VERSION_ID=[^\\n]*\" | cut -f2 -d= | tr -d '\"')"
        OUTPUT_VARIABLE LINUX_VERSION
)
message("Linux version is '${LINUX_VERSION}'")

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(
        BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/include
        system
)
link_directories(
        /usr/local/lib
        system
)

add_subdirectory(test)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=* -p ${CMAKE_SOURCE_DIR}/build")

file(GLOB HEADER_FILES ${CMAKE_SOURCE_DIR}/include/*.h)

# Install the header files directly under 'include/dkyb'
install(FILES ${HEADER_FILES}
        DESTINATION include/dkyb)
