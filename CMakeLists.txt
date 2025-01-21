# ----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(nevod)

# ----------------------------------------------------------------------------
# Find the ROOT package
#
find_package(ROOT REQUIRED COMPONENTS RIO Net)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

# ----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" OFF)

if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()

# ----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include
    ${Geant4_INCLUDE_DIR})

# ----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# ----------------------------------------------------------------------------
# Add yaml-cpp
include(FetchContent)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)

# ----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(nevod main.cc ${sources} ${headers})
target_link_libraries(nevod ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} yaml-cpp::yaml-cpp)

# message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})

# ----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build NECnt. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(NEVOD_SCRIPTS
    config/run.mac
    config/vis.mac
)

foreach(_script ${NEVOD_SCRIPTS})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_script}
        ${PROJECT_BINARY_DIR}/${_script}
        COPYONLY
    )
endforeach()

# ----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(TNEVOD DEPENDS nevod)

# ----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS nevod DESTINATION bin)
