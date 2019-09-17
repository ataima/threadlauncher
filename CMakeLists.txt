# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)

project (threadswitcher)
set(CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)
set(PROJECT_HOME ${CMAKE_SOURCE_DIR} )

include_directories(.)


 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "./*.cpp")

ADD_EXECUTABLE(threadswitcher  ${SOURCES})

install ( TARGETS threadswitcher DESTINATION ${CMAKE_INSTALL_PREFIX} )

set(CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")

target_link_libraries  ( threadswitcher  pthread )

add_custom_command (
    TARGET threadswitcher
    PRE_BUILD
    COMMAND ${PROJECT_HOME}/beauty.sh ${CMAKE_CURRENT_SOURCE_DIR}
)


