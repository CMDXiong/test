PROJECT( boolean )

CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

SET( WXART2D_FLAGS "" )
SET( WXART2D_INCLUDE_DIRS "" )
SET( WXART2D_LIBRARY_DIRS "" )
SET( WXART2D_LIBRARIES "" )
SET( WXART2D_LIBRARIES_WXLIKE "" )
SET( WXART2D_THIRDPARTY_LIBRARIES "" )

SET (LIBRARY_OUTPUT_PATH ${boolean_BINARY_DIR}/lib/ CACHE PATH "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${boolean_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all executables.")
##################################################
# include macro definitions
##################################################

INCLUDE( ${boolean_SOURCE_DIR}/bin/CheckMSVC.cmake )

INCLUDE (${boolean_SOURCE_DIR}/bin/Macros.cmake)
IF( NOT WIN32 )
    INCLUDE( ${boolean_SOURCE_DIR}/bin/FindPkgConfig.cmake )

    PKGCONFIG("gtk+-2.0 >= 2.2.0")
    IF(PKGCONFIG_FOUND)
      IF(CMAKE_PKGCONFIG_C_FLAGS)
        SET( GTK2_FOUND  "TRUE" )
        #do something with ${PKGCONFIG_LIBRARIES}
      ENDIF(CMAKE_PKGCONFIG_C_FLAGS)
    ELSE(PKGCONFIG_FOUND)
        SET( GTK2_FOUND  "" )
    ENDIF(PKGCONFIG_FOUND)
ENDIF( NOT WIN32 )

##################################################
# Set all includes, flags, libraries
# Please only use the A2D_ functions to add options in this section,
# or use EXTENDART2DVARS to set flag, include and lib at the same time.
##################################################

A2D_INCLUDE_DIRECTORIES( "${boolean_SOURCE_DIR}/modules" )
A2D_INCLUDE_DIRECTORIES( "${boolean_SOURCE_DIR}" )
A2D_LINK_DIRECTORIES(  "${boolean_BINARY_DIR}/lib" )  
IF( MSVC )
    #VC specific configuaration stuff
    A2D_LINK_DIRECTORIES(  ${boolean_BINARY_DIR}/lib/Debug )  
ENDIF( MSVC )

##################################################
# Set all includes, flags, libraries, related to wxWidgets
##################################################

# the order here is important, lower level libs last
SET(wxWidgets_USE_LIBS xrc qa html adv core xml net base )

INCLUDE (${boolean_SOURCE_DIR}/bin/FindwxWidgets.cmake)

A2D_ADD_DEFINITIONS( "${wxWidgets_CXX_FLAGS}" )
A2D_ADD_DEFINITIONS( "-D_CRT_SECURE_NO_DEPRECATE" )
A2D_ADD_DEFINITIONS( "${wxWidgets_DEFINITIONS}" )
A2D_INCLUDE_DIRECTORIES( "${wxWidgets_INCLUDE_DIRS}" )
A2D_LINK_DIRECTORIES(  "${wxWidgets_LIB_DIR}" )

# set precompiled headers if wanted/possible
SET_PRECOMPILED_HEADER_FLAGS()
# set some flags to prevent mixing up debug wxWidgets and release libraries or visa versa
SET_WX_SYNCLIBRARY_FLAGS()

##################################################
# Generate wxart2d-config files for both cmake and unix
##################################################

ADD_SUBDIRECTORY( modules )

##################################################
# add the wxWidgets libraries at the end of the list, meaning link in first.
# this is needed, else linking errors.
##################################################
A2D_LINK_LIBRARIES( "${wxWidgets_LIBRARIES}" )

##################################################
# Set all includes, flags, libraries
##################################################

SET( WXART2D_LIBRARIES_WXLIKE ${WXART2D_LIBRARIES_WXLIKE} CACHE STRING "package libs directory path" FORCE )
SET( WXART2D_FLAGS ${WXART2D_FLAGS} CACHE STRING "package libs directory path" FORCE )
SET( WXART2D_INCLUDE_DIRS ${WXART2D_INCLUDE_DIRS} CACHE STRING "package libs directory path" FORCE )
SET( WXART2D_LIBRARY_DIRS ${WXART2D_LIBRARY_DIRS} CACHE STRING "package libs directory path" FORCE )
SET( WXART2D_LIBRARIES ${WXART2D_LIBRARIES} CACHE STRING "package libs directory path" FORCE )
SET( WXART2D_THIRDPARTY_LIBRARIES ${WXART2D_THIRDPARTY_LIBRARIES} CACHE STRING "package libs directory path" FORCE )

ADD_SUBDIRECTORY( apps )
