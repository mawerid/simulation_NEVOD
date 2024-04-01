#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.16)
project(NEVOD)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if (WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
    set(G4VIS_USE ON)
    set(G4UI_USE ON)
else ()
    find_package(Geant4 REQUIRED)
    #    set(G4MULTITHREADED OFF)
endif ()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include
        ${Geant4_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/source/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(NEVOD NEVOD.cc ${sources} ${headers})
target_link_libraries(NEVOD ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build NECnt. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(NEVOD_SCRIPTS
        vis.mac
)

foreach (_script ${NEVOD_SCRIPTS})
    configure_file(
            ${PROJECT_SOURCE_DIR}/${_script}
            ${PROJECT_BINARY_DIR}/${_script}
            COPYONLY
    )
endforeach ()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(NEVOD DEPENDS NEVOD)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS NEVOD DESTINATION bin)
