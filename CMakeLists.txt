cmake_minimum_required(VERSION 2.8)

# DO NOT EDIT THIS FILE WITH AN EXTERNAL TOOL!

# Set the project name.
project(XmasTiles)

#------------------------------------------------------------------------------
# Set output directories
#------------------------------------------------------------------------------

if(MSVC)

    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/VS08-CL/out)

elseif(CMAKE_COMPILER_IS_GNUCXX)

    # Default to a debug build for GCC

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose Build Type: Debug or Release" FORCE)
    endif(NOT CMAKE_BUILD_TYPE) 

    if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
        set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/debug)
    else()
        set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/release)
    endif()

endif(MSVC)

# This is to fix OmniCPPComplete in Vim
include_directories(${CMAKE_SOURCE_DIR}/code/compat)

message("Output directory is ${CMAKE_BINARY_DIR}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------

option(WINDOWS9X_COMPAT "Extra Windows 98/ME compatibility checks? (Currently Unused)" NO)
option(WINDOWS2K_COMPAT "Extra Windows 2000 compatibility checks? (Currently Unused)" NO)

#if(MSVC)
#    option(NO_ALLBUILD "Remove 'All Build' from project file" NO)
#endif(MSVC)

#------------------------------------------------------------------------------
# Setup Platform independent sources and libraries
#------------------------------------------------------------------------------

set(SOURCES
    code/main.cpp
#    code/gitinfo.h
    code/stdint_compat.h
)

#file(GLOB JSON_LIB
#    src/thirdparty/jsoncpp/json/*.h
#    src/thirdparty/jsoncpp/*.cpp
#)

file(GLOB INTERFACES
    src/interface/*.h
)

#file(GLOB DATA_MODELS
#    src/model/*.h
#    src/model/*.cpp
#)

file(GLOB PRESENTERS
    code/presenter/*.h
    code/presenter/*.cpp
)

#------------------------------------------------------------------------------
# Setup Shared Windows resources
#------------------------------------------------------------------------------

#file(GLOB WIN_SOURCES
#    src/resources/win32/resource.h
#    src/resources/win32/resource.rc
#)

#------------------------------------------------------------------------------
# Setup Windows with Win32
#------------------------------------------------------------------------------

file(GLOB WIN_SOURCES
    code/win32/*.h
    code/win32/*.cpp
)

#------------------------------------------------------------------------------
# TODO: Other toolkits such as GTK maybe. This is just from another project
# but I kept it here to remind myself.
#------------------------------------------------------------------------------

#if(WINAPI_BUILD)
#    file(GLOB WINBUILD_SOURCES
#        src/win32api/*.h
#        src/win32api/*.cpp
#    )
#    add_compile_definitions(USE_WINAPI)
#endif(WINAPI_BUILD)

#------------------------------------------------------------------------------
# Group all the sources for easy finding (MSVC only)
#------------------------------------------------------------------------------

SOURCE_GROUP("presenters"            FILES ${PRESENTERS})
#SOURCE_GROUP("model"                FILES ${DATA_MODELS})
SOURCE_GROUP("interfaces"            FILES ${INTERFACES})
#SOURCE_GROUP("thirdparty/json"      FILES ${JSON_LIB})
SOURCE_GROUP("win32"                 FILES ${WIN_SOURCES})

#if(WINAPI_BUILD)
#
#    SOURCE_GROUP("winapi"       FILES ${WINBUILD_SOURCES})
#
#else()
#
#    #SOURCE_GROUP("gtk"        FILES ${WINBUILD_SOURCES})
#
#endif()

#------------------------------------------------------------------------------
# Set required Libs (MSVC)
#------------------------------------------------------------------------------

# TODO: Globbing? Find_Library?

if(MSVC)
    set(
        WIN32_LIBS
        comctl32.lib
    )
endif()

#==============================================================================
# Misc Things
#==============================================================================

#execute_process(COMMAND python updategitinfo.py WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

#==============================================================================
# Set compiler specific things 
#==============================================================================

# TODO: Simplify everything to reduce code reuse

#------------------------------------------------------------------------------
# Compiler Specific things pertaining to Microsoft Visual C++
#------------------------------------------------------------------------------

if(MSVC)

    # TODO: Confirm that this works with a newer VC.
	add_definitions(-D_UNICODE -DUNICODE)
	
	# Get rid of "ZERO_CHECK" in the project file
	set(CMAKE_SUPPRESS_REGENERATION true)

    # TODO: Something that gets rid of the automatically generated "ALL_BUILD" project.
    
    # Set CFlags
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
    set(CMAKE_CXX_FLAGS "/nologo /MP4 /c /Zi /FC /W4 /EHsc /FI omnifix.h")
    
    # Set LFlags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO /MANIFESTUAC:NO /SUBSYSTEM:WINDOWS /DEBUG")

    #if(CMAKE_CONFIGURATION_TYPES STREQUAL "Debug")

        #TODO: differences for Debug/Release builds

        # Get the user to set the lib and include folders
        #set(WIN32PP_INC_DIR "" CACHE PATH "Path to the win32++ include folder")

        # Setup the executable
        add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${INTERFACES} ${PRESENTERS} ${WIN_SOURCES})
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "XmasTiles")

        # Set the libs to link
        target_link_libraries(${PROJECT_NAME} ${WIN32_LIBS})

        # Set the include directories
        # target_include_directories(${PROJECT_NAME} PRIVATE ${WIN32PP_INC_DIR})

        #if(NO_ALLBUILD)
        #    execute_process(COMMAND python strip_allbuild.py WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
        #endif(NO_ALLBUILD)

    #endif()


endif(MSVC)
