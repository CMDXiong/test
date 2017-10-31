# - Find a wxWidgets (a.k.a., wxWindows) installation.
# This module finds if wxWidgets is installed and selects a default
# configuration to use.
#
# The following variables are searched for and set to defaults in case
# of multiple choices. Change them if the defaults are not desired:
#
#  wxWidgets_ROOT_DIR      - Base wxWidgets directory
#                            (e.g., C:/wxWidgets-2.6.3).
#  wxWidgets_LIB_DIR       - Path to wxWidgets libraries
#                            (e.g., C:/wxWidgets-2.6.3/lib/${s_wxCompiler}_lib).
#  wxWidgets_CONFIGURATION - Configuration to use
#                            (e.g., msw, mswd, mswu, mswunivud, etc.)
#  wxWidgets_FIND_COMPONENTS      - Libraries to use besides the common
#                            required ones; set to base and core by
#                            default. You could also list them in
#                            FIND_PACKAGE(wxWidgets REQUIRED
#                                         <components>)
#
# The following are set after configuration is done:
#
#  wxWidgets_FOUND            - Set to TRUE if wxWidgets was found.
#  wxWidgets_INCLUDE_DIRS     - Include directories for WIN32
#                               i.e., where to find "wx/wx.h" and
#                               "wx/setup.h"; possibly empty for unices.
#  wxWidgets_LIBRARIES        - Path to the wxWidgets libraries.
#  wxWidgets_LIBRARY_DIRS     - compile time link dirs, useful for
#                               rpath on UNIX. Typically an empty string
#                               in WIN32 environment.
#  wxWidgets_DEFINITIONS      - Contains defines required to compile/link
#                               against WX, e.g. -DWXUSINGDLL
#  wxWidgets_CXX_FLAGS        - Include dirs and ompiler flags for
#                               unices, empty on WIN32. Esentially
#                               "`wx-config --cxxflags`".
#  wxWidgets_USE_FILE         - convenience include file
#
#  wxWidgets_PFVERSION        - version of wxWidgets that was found
#  wxWidgets_PORT             - port of wxWidgets ( msw, gtk2, gtk etc. )
#  wxWidgets_UNV              - contains "unv" when universal wxWidgets is required
#  wxWidgets_UCD              - contains "u" when unicode is required
#  wxWidgets_DBG              - contains "d" when debug is required
#
# Sample usage:
#
#   SET(wxWidgets_USE_LIBS base core gl net)
#   FIND_PACKAGE(wxWidgets)
#   IF(wxWidgets_FOUND)
#     INCLUDE(${wxWidgets_USE_FILE})
#     # and for each of your dependant executable/library targets:
#     TARGET_LINK_LIBRARIES(<YourTarget> ${wxWidgets_LIBRARIES})
#   ENDIF(wxWidgets_FOUND)
#
# Sample usage with monolithic wx build:
#   SET(wxWidgets_USE_LIBS msw26 expat jpeg gl png regex tiff zlib)
#   ...

# NOTES
#
# This module has been tested on the WIN32 platform with wxWidgets
# 2.6.2, 2.6.3, and 2.5.3. However, it has been designed to
# easily extend support to all possible builds, e.g., static/shared,
# debug/release, unicode, universal, multilib/monolithic, etc..
#
# If you want to use the module and your build type is not supported
# out-of-the-box, please contact me to exchange information on how
# your system is setup and I'll try to add support for it.
#
# AUTHOR
#
# Miguel A. Figueroa-Villanueva (miguelf at ieee dot org).
# Jan Woetzel (jw at mip.informatik.uni-kiel.de).
#
# Based on previous works of:
# Jan Woetzel (FindwxWindows.cmake),
# Jorgen Bodde and Jerry Fath (FindwxWin.cmake).
#
# modified by Klaas Holwerda
#
# TODO/ideas
#
# (1) Option/Setting to use all available wx libs
# In contrast to expert developer who lists the
# minimal set of required libs in wxWidgets_USE_LIBS
# there is the newbie user:
#   - who just wants to link against WX with more 'magic'
#   - doesn't know the internal structure of WX or how it was built,
#     in particular if it is monolithic or not
#   - want to link against all available WX libs
# Basically, the intent here is to mimic what wx-config would do by
# default (i.e., `wx-config --libs`).
#
# Possible solution:
#   Add a reserved keyword "std" that initializes to what wx-config
# would default to. If the user has not set the wxWidgets_USE_LIBS,
# default to "std" instead of "base core" as it is now. To implement
# "std" will basically boil down to a FOR_EACH lib-FOUND, but maybe
# checking whether a minimal set was found.
#


#
# Helper macro to control the debugging output globally.
# - NOTE: This and all the DBG_MSG calls should be removed after the
#         module stabilizes.
#
MACRO(DBG_MSG p_msg)
  # MESSAGE(STATUS "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${p_msg}")
  # MESSAGE( "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${p_msg}")
ENDMACRO(DBG_MSG)

#
# conditional 
# - NOTE: This and all the DBG_MSG calls should be removed after the
#         module stabilizes.
#
MACRO( ERROR_MSG p_msg)
    # make FIND_PACKAGE friendly
    IF(NOT wxWidgets_FIND_QUIETLY)
        IF(wxWidgets_FIND_REQUIRED)
            MESSAGE( "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${p_msg}")
        ELSE(wxWidgets_FIND_REQUIRED)
            MESSAGE( STATUS "ERROR: ${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${p_msg}")
        ENDIF(wxWidgets_FIND_REQUIRED)
    ENDIF(NOT wxWidgets_FIND_QUIETLY)
ENDMACRO( ERROR_MSG )

#
SET(wxWidgets_FOUND FALSE)
SET( wxWidgets_INCLUDE_DIRS      "" )
SET( wxWidgets_LIBRARIES        "" )
SET( wxWidgets_LIBRARY_DIRS "" )
#NOT DO SET( wxWidgets_LIB_DIR "" )
SET( wxWidgets_CXX_FLAGS        "" )
SET( wxWidgets_DEFINITIONS "" )
SET( wxWidgets_UNV "" )
SET( wxWidgets_UCD "" )
SET( wxWidgets_DBG "" )

# Using SYSTEM with INCLUDE_DIRECTORIES in conjunction with wxWidgets on
# the Mac produces compiler errors. Set wxWidgets_INCLUDE_DIRS_NO_SYSTEM
# to prevent UsewxWidgets.cmake from using SYSTEM.
#
# See cmake mailing list discussions for more info:
#   http://www.cmake.org/pipermail/cmake/2008-April/021115.html
#   http://www.cmake.org/pipermail/cmake/2008-April/021146.html
#
IF(APPLE)
  SET(wxWidgets_INCLUDE_DIRS_NO_SYSTEM 1)
ENDIF(APPLE)

# DEPRECATED: This is a patch to support the DEPRECATED use of
# wxWidgets_USE_LIBS.
#
# If wxWidgets_USE_LIBS is set:
# - if using <components>, then override wxWidgets_USE_LIBS
# - else set wxWidgets_FIND_COMPONENTS to wxWidgets_USE_LIBS
IF(wxWidgets_USE_LIBS AND NOT wxWidgets_FIND_COMPONENTS)
  SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_USE_LIBS})
ENDIF(wxWidgets_USE_LIBS AND NOT wxWidgets_FIND_COMPONENTS)
DBG_MSG("wxWidgets_FIND_COMPONENTS : ${wxWidgets_FIND_COMPONENTS}")

OPTION( wxWidgets_USE_MONOLITHIC "Use wxWidgets build as monolithic library" OFF)
OPTION( wxWidgets_USE_DEBUG  "Use Debug versions of wxWidgets libraries" ON)
OPTION( wxWidgets_USE_UNICODE "Use Unicode versions of wxWidgets libraries" OFF)
OPTION( wxWidgets_USE_STATIC "Use static versions (.a) of wxWidgets libraries" ON)
OPTION( wxWidgets_USE_UNIVERSAL "Use Universal versions (.so) of wxWidgets libraries" OFF)

IF( wxWidgets_USE_UNIVERSEL )
    SET( wxWidgets_UNV "univ" )
ENDIF( wxWidgets_USE_UNIVERSEL )

IF ( wxWidgets_USE_UNICODE )
    SET( wxWidgets_UCD "u" )
ENDIF ( wxWidgets_USE_UNICODE )        

IF( wxWidgets_USE_DEBUG )
    SET( wxWidgets_DBG "d" )
ENDIF( wxWidgets_USE_DEBUG )
    
#=====================================================================
#=====================================================================
SET( wxWidgets_FIND_STYLE "NOT_DEFINED_STYLE_FIND" CACHE STRING "wxWidgets find style" FORCE )

IF ( ${CMAKE_GENERATOR} STREQUAL "MSYS Makefiles" )
	# remove the standard -O3 option, it does not work
    SET( wxWidgets_FIND_STYLE "UNIX_STYLE_FIND" CACHE STRING "wxWidgets find style" FORCE )
		
	SET (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING
		"Flags used by the compiler during release builds" FORCE)
	SET (CMAKE_CX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING
		"Flags used by the compiler during release builds" FORCE)
		
	IF( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} mono stc )
	ELSE( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} aui stc qa xrc html adv xml net core base )
	ENDIF( wxWidgets_USE_MONOLITHIC )
		
ENDIF ( ${CMAKE_GENERATOR} STREQUAL "MSYS Makefiles" )

IF ( ${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles" )
	OPTION( wxWidgets_MINGW_FROM_SOURCE  "Use wxWidgets source distribution instead of wxDevCpp devpack" ON)
	# remove the standard -O3 option, it does not work
    SET( wxWidgets_FIND_STYLE "WIN32_STYLE_FIND" CACHE STRING "wxWidgets find style" FORCE )   
   	  
	SET (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING
		"Flags used by the compiler during release builds" FORCE)
	SET (CMAKE_C_FLAGS_RELEASE "-DNDEBUG" CACHE STRING
		"Flags used by the compiler during release builds" FORCE)
	SET( wxWidgets_CXX_FLAGS ${wxWidgets_CXX_FLAGS}
        -D__WXMSW__ -D__GNUWIN32__ -D__WIN95__ 
		-fexceptions -fno-pcc-struct-return 
        -fstrict-aliasing 
		-Wall    #all warnings
        -Wno-unused-variable
        # -m32  # 32 bits
		#-fexpensive-optimizations 
		#-mwindows  # windows app
		#-mconsole    #console app
    )   
    #add some libs which are not already in CMAKE_CXX_STANDARD_LIBRARIES
    SET( wxWidgets_complibs
        winmm
        comctl32
        rpcrt4
        wsock32
        kernel32
        user32
        gdi32
        comdlg32
        winspool
        winmm
        shell32
        comctl32
        ole32
        oleaut32
        uuid
        rpcrt4
        advapi32
        wsock32
        odbc32
        opengl32			
    )
	
	IF( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} mono stc )
	ELSE( wxWidgets_USE_MONOLITHIC )
		#SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} aui stc qa xrc html adv xml net core base )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} aui stc xrc html adv xml net core base )
	ENDIF( wxWidgets_USE_MONOLITHIC )
	
ENDIF ( ${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles" )

IF ( ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" )
    SET( wxWidgets_FIND_STYLE "UNIX_STYLE_FIND" CACHE STRING "wxWidgets find style" FORCE )

    IF( CYGWIN OR MINGW )
		SET (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG" CACHE STRING
			"Flags used by the compiler during release builds" FORCE)
		SET (CMAKE_C_FLAGS_RELEASE "-DNDEBUG" CACHE STRING
			"Flags used by the compiler during release builds" FORCE)
    ENDIF( CYGWIN OR MINGW )
	
	IF( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} mono stc )
	ELSE( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} aui stc qa xrc html adv xml net core base )
	ENDIF( wxWidgets_USE_MONOLITHIC )
	
ENDIF ( ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" )

IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio.*" )
    SET( wxWidgets_FIND_STYLE "WIN32_STYLE_FIND" CACHE STRING "wxWidgets find style" FORCE )    
    #add some libs which are not already in CMAKE_CXX_STANDARD_LIBRARIES
    SET( wxWidgets_complibs 
        winmm
        comctl32
        rpcrt4
        wsock32
    )
	IF( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} mono stc )
	ELSE( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} aui stc qa xrc html adv xml net core base )
	ENDIF( wxWidgets_USE_MONOLITHIC )
	
ENDIF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio.*" )

IF ( ${CMAKE_GENERATOR} MATCHES "Borland Makefiles" )
    SET( wxWidgets_FIND_STYLE "WIN32_STYLE_FIND" CACHE STRING "wxWidgets find style" FORCE )    
    #add some libs which are not already in CMAKE_CXX_STANDARD_LIBRARIES
    SET( wxWidgets_complibs 
        winmm
        comctl32
        rpcrt4
        wsock32
    )
	
	SET( wxWidgets_CXX_FLAGS ${wxWidgets_CXX_FLAGS}
	-tWR -a8 -g0
    )   

	IF( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} mono stc )
	ELSE( wxWidgets_USE_MONOLITHIC )
		SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} aui stc xrc html adv xml net core )
	ENDIF( wxWidgets_USE_MONOLITHIC )
	
ENDIF ( ${CMAKE_GENERATOR} MATCHES "Borland Makefiles" )

#MESSAGE( "generator: ${CMAKE_GENERATOR}")
MARK_AS_ADVANCED( FORCE wxWidgets_FIND_STYLE )

#=====================================================================
# windows style find
#=====================================================================
IF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
	SET(wxWidgets_FOUND FALSE)

	# depending on the options set by the user, a configuration directory in "wxWidgets its root/lib/configuration_directory"
	# can be set. Later on it is checked if that configuration indeed exists. This configuration_directory is unique and holds
	# build.cfg file.
    SET( wxWidgets_CONFIGURATION "msw"  )
    IF( wxWidgets_USE_UNIVERSEL )
        SET( wxWidgets_CONFIGURATION "${wxWidgets_CONFIGURATION}univ"  )
    ENDIF( wxWidgets_USE_UNIVERSEL )
    
    IF ( wxWidgets_USE_UNICODE )
        SET( wxWidgets_CONFIGURATION "${wxWidgets_CONFIGURATION}u"  )
        SET( wxWidgets_DEFINITIONS ${wxWidgets_DEFINITIONS} -DwxUSE_UNICODE  )
    ENDIF ( wxWidgets_USE_UNICODE )        
    
    IF( wxWidgets_USE_DEBUG )
        SET( wxWidgets_CONFIGURATION "${wxWidgets_CONFIGURATION}d"  )
        SET( wxWidgets_DEFINITIONS ${wxWidgets_DEFINITIONS} -D_DEBUG_  -D__WXDEBUG__  )
    ENDIF( wxWidgets_USE_DEBUG )
	MARK_AS_ADVANCED( FORCE wxWidgets_CONFIGURATION )

    SET(WX_USE_REL_AND_DBG FALSE)
    SET( wxWidgets_PORT "msw" CACHE STRING
                "Set wxWidgets configuration (${wxWidgets_PORT})"  FORCE )            
	MARK_AS_ADVANCED( FORCE wxWidgets_PORT )

    IF( MSVC OR MSVC_IDE OR MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 )
        SET( s_wxCompiler "vc" )
        SET( s_libPre "" )
        SET( s_libExt "lib" )        
    ENDIF( MSVC OR MSVC_IDE OR MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 )
    IF( BORLAND )
        SET( s_wxCompiler "bcc" )
        SET( s_libPre "" )
        SET( s_libExt "lib" )        
    ENDIF( BORLAND )
    IF( "${CMAKE_CXX_COMPILER}" MATCHES ".*wcl.*" )
        SET( s_wxCompiler "wat" )
        SET( s_libPre "" )
        SET( s_libExt "lib" )        
    ENDIF( "${CMAKE_CXX_COMPILER}" MATCHES ".*wcl.*" )
    IF( MINGW )
        SET( s_wxCompiler "gcc" )
        SET( s_libPre "lib" )
        SET( s_libExt "a" )        
    ENDIF( MINGW )   

    # global settings for std and common wx libs
    # logic could determine _USE_MONOLITHIC automatically
    # but let the user decide for now.
    IF (wxWidgets_USE_MONOLITHIC)
		SET(wxWidgets_STD_LIBRARIES mono )
    ELSE (wxWidgets_USE_MONOLITHIC)
        SET(wxWidgets_STD_LIBRARIES base core ) # this is default
    ENDIF(wxWidgets_USE_MONOLITHIC)

    #useful common wx libs needed by almost all components
    SET(wxWidgets_COMMON_LIBRARIES png tiff jpeg zlib regex expat)

    #---------------------------------------------------------------------
    # WIN32: Helper MACROS
    #---------------------------------------------------------------------

    #
    # Find libraries associated to a configuration.
    # Will set variables in WX_libname(p_dbg) where (p_dbg stand for debug or not)
    # If library is not available, becomes WX_libname-NOTFOUND
    # param: p_unv universel wxWidgets prefix
    # param: p_ucd unicode postfix
    # param: p_dbg debug postfix
    MACRO(WX_FIND_LIBS p_unv p_ucd p_dbg)
        DBG_MSG("p_unv = ${p_unv}")
        DBG_MSG("p_ucd = ${p_ucd}")
        DBG_MSG("p_dbg = ${p_dbg}")

        # Find wxWidgets common libraries
        FOREACH(LIB png tiff jpeg zlib regex expat)
            SET( WX_${LIB}${p_dbg} "WX_${LIB}${p_dbg}-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)
            FIND_LIBRARY(WX_${LIB}${p_dbg}
                NAMES
                ${s_libPre}wx${LIB}${p_ucd}${p_dbg} # for regex
                ${s_libPre}wx${LIB}${p_dbg}
                PATHS ${s_wxLibDir}
                NO_DEFAULT_PATH
            )
            GET_FILENAME_COMPONENT( WX_${LIB}${p_dbg} ${WX_${LIB}${p_dbg}} NAME_WE )
            STRING(REGEX REPLACE "^lib" ""  WX_${LIB}${p_dbg} ${WX_${LIB}${p_dbg}} )		 
            MARK_AS_ADVANCED( WX_${LIB}${p_dbg} )
            DBG_MSG("detected ${s_libPre}wx${LIB}${p_ucd}${p_dbg} = ${WX_${LIB}${p_dbg}}")
        ENDFOREACH(LIB)

        # Find wxWidgets multilib base libraries
        SET( WX_base${p_dbg} "WX_base${p_dbg}-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)
        FIND_LIBRARY(WX_base${p_dbg}
            NAMES
            ${s_libPre}wxbase${wxWidgets_PFVERSION}${p_ucd}${p_dbg}
            PATHS ${s_wxLibDir}
            NO_DEFAULT_PATH
        )
        GET_FILENAME_COMPONENT( WX_base${p_dbg} ${WX_base${p_dbg}} NAME_WE )
        STRING(REGEX REPLACE "^lib" ""  WX_base${p_dbg} ${WX_base${p_dbg}} )		 
        MARK_AS_ADVANCED(WX_base${p_dbg})
        
        FOREACH(LIB net odbc xml )
            SET( WX_${LIB}${p_dbg} "WX_${LIB}${p_dbg}-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)
            FIND_LIBRARY(WX_${LIB}${p_dbg}
                NAMES
                ${s_libPre}wxbase${wxWidgets_PFVERSION}${p_ucd}${p_dbg}_${LIB}
                PATHS ${s_wxLibDir}
                NO_DEFAULT_PATH
            )
            GET_FILENAME_COMPONENT( WX_${LIB}${p_dbg} ${WX_${LIB}${p_dbg}} NAME_WE )
            STRING(REGEX REPLACE "^lib" ""  WX_${LIB}${p_dbg} ${WX_${LIB}${p_dbg}} )		 
            MARK_AS_ADVANCED(WX_${LIB}${p_dbg})
            DBG_MSG("detected wxbase${wxWidgets_PFVERSION}${p_ucd}${p_dbg}_${LIB} = ${WX_${LIB}${p_dbg}}")
        ENDFOREACH(LIB)
        
        # Find wxWidgets monolithic library
        SET( WX_mono${p_dbg} "WX_mono${p_dbg}-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)
        FIND_LIBRARY( WX_mono${p_dbg}
            NAMES
            ${s_libPre}wxmsw${p_unv}${wxWidgets_PFVERSION}${p_ucd}${p_dbg}
            PATHS ${s_wxLibDir}
            NO_DEFAULT_PATH
        )
        GET_FILENAME_COMPONENT( WX_mono${p_dbg} ${WX_mono${p_dbg}} NAME_WE )
        STRING(REGEX REPLACE "^lib" ""  WX_mono${p_dbg} ${WX_mono${p_dbg}} )		 
        MARK_AS_ADVANCED( WX_mono${p_dbg} )
        
        # Find wxWidgets multilib libraries
        FOREACH( LIB core adv aui html media xrc dbgrid gl qa stc richtext gizmos things netutils gizmos_xrc )
            SET( WX_${LIB}${p_dbg} "WX_${LIB}${p_dbg}-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)			
            FIND_LIBRARY(WX_${LIB}${p_dbg}
                NAMES
                ${s_libPre}wxmsw${p_unv}${wxWidgets_PFVERSION}${p_ucd}${p_dbg}_${LIB}
                PATHS ${s_wxLibDir}
                NO_DEFAULT_PATH
            )
            GET_FILENAME_COMPONENT( WX_${LIB}${p_dbg} ${WX_${LIB}${p_dbg}} NAME_WE )
            STRING(REGEX REPLACE "^lib" ""  WX_${LIB}${p_dbg} ${WX_${LIB}${p_dbg}} )		 
            MARK_AS_ADVANCED(WX_${LIB}${p_dbg})
            DBG_MSG("detected wxmsw${p_unv}${wxWidgets_PFVERSION}${p_ucd}${p_dbg}_${LIB} = ${WX_${LIB}${p_dbg}}")
        ENDFOREACH(LIB)

        #FOREACH( LIB gl ) the gl lib is now wxmsw28ud_gl.lib, so added above here, and removed here
        FOREACH( LIB )
            SET( WX_${LIB}${p_dbg} "WX_${LIB}${p_dbg}-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)
            FIND_LIBRARY(WX_${LIB}${p_dbg}
                NAMES
                ${s_libPre}wxmsw${p_unv}${wxWidgets_PFVERSION}${p_dbg}_${LIB}
                PATHS ${s_wxLibDir}
                NO_DEFAULT_PATH
            )
            GET_FILENAME_COMPONENT( WX_${LIB}${p_dbg} ${WX_${LIB}${p_dbg}} NAME_WE )
			STRING(REGEX REPLACE "^lib" ""  WX_${LIB}${p_dbg} ${WX_${LIB}${p_dbg}} )		 
            MARK_AS_ADVANCED(WX_${LIB}${p_dbg})
            DBG_MSG("detected wxmsw${p_unv}${wxWidgets_PFVERSION}${p_dbg}_${LIB} = ${WX_${LIB}${p_dbg}}")
        ENDFOREACH(LIB)
        
    ENDMACRO(WX_FIND_LIBS)

    #
    # Clear all library paths, so that FIND_LIBRARY refinds them.
    #
    # Clear a lib, reset its found flag, and mark as advanced.
    MACRO(WX_CLEAR_LIB p_lib)
        SET(${p_lib} "${p_lib}-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)
        SET(${p_lib}_FOUND FALSE)
        MARK_AS_ADVANCED(${p_lib})
    ENDMACRO(WX_CLEAR_LIB)
  
    # Clear all debug or release library paths (arguments are "d" or "").
    MACRO(WX_CLEAR_ALL_LIBS p_dbg)
        # Clear wxWidgets common libraries
        FOREACH(LIB png tiff jpeg zlib regex expat)
            WX_CLEAR_LIB(WX_${LIB}${p_dbg})
        ENDFOREACH(LIB)
    
        # Clear wxWidgets multilib base libraries
        WX_CLEAR_LIB(WX_base${p_dbg})
        FOREACH(LIB net odbc xml)
            WX_CLEAR_LIB(WX_${LIB}${p_dbg})
        ENDFOREACH(LIB)
    
        # Clear wxWidgets monolithic library
        WX_CLEAR_LIB(WX_mono${p_dbg})
    
        # Clear wxWidgets multilib libraries
        FOREACH(LIB core adv html media xrc dbgrid gl qa)
            WX_CLEAR_LIB(WX_${LIB}${p_dbg})
        ENDFOREACH(LIB)
    ENDMACRO(WX_CLEAR_ALL_LIBS)
    
    # Clear all wxWidgets debug libraries.
    MACRO(WX_CLEAR_ALL_DBG_LIBS)
        WX_CLEAR_ALL_LIBS("d")
    ENDMACRO(WX_CLEAR_ALL_DBG_LIBS)
    # Clear all wxWidgets release libraries.
    MACRO(WX_CLEAR_ALL_REL_LIBS)
        WX_CLEAR_ALL_LIBS("")
    ENDMACRO(WX_CLEAR_ALL_REL_LIBS)
	
	# check for build.cfg and extract given variable X=Y its value, and set this as a new variable wxWidgets_X with value Y
    MACRO( find_build_variable p_buildvar )	
		IF (EXISTS ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/build.cfg)
			SET( wxWidgets_BUILDFILE ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/build.cfg )
			file( STRINGS ${wxWidgets_BUILDFILE} wxWidgets_${p_buildvar} REGEX "^${p_buildvar}=" )
			STRING(REGEX REPLACE ".*=(.*) " "\\1"  wxWidgets_${p_buildvar} "${wxWidgets_${p_buildvar}}" )
			SET( wxWidgets_${p_buildvar} "${wxWidgets_${p_buildvar}}" CACHE STRING "gdiplus used" FORCE )				
			MARK_AS_ADVANCED( FORCE wxWidgets_${p_buildvar} )
		ELSE (EXISTS ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/build.cfg)
			ERROR_MSG("WXWIDGET_FOUND FALSE because ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/build.cfg does not exists.")
			SET(wxWidgets_FOUND FALSE)
		ENDIF (EXISTS ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/build.cfg)
    ENDMACRO( find_build_variable )

    #
    # Set the wxWidgets_LIBRARIES variable.
    # Also, Sets output variable wxWidgets_FOUND to FALSE if it fails.
    # The variables set in WX_FIND_LIBS are now used to set the wxWidgets_LIBRARIES variable
    # with the right libraries according to build type.
    MACRO(WX_SET_LIBRARIES p_LIBS p_dbg)
        IF(WX_USE_REL_AND_DBG)
            DBG_MSG("looking for ${${p_LIBS}}")
            FOREACH(LIB ${${p_LIBS}})
                DBG_MSG("Finding ${LIB} and ${LIB}d")
                DBG_MSG("WX_${LIB}  : ${WX_${LIB}}")
                DBG_MSG("WX_${LIB}d : ${WX_${LIB}d}")
                IF(WX_${LIB} AND WX_${LIB}d)
                    DBG_MSG("Found ${LIB} and ${LIB}d")
                    SET(wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES}
                        debug     ${WX_${LIB}d}
                        optimized ${WX_${LIB}}
                    )
                ELSE(WX_${LIB} AND WX_${LIB}d)
                    ERROR_MSG("- not found due to missing WX_${LIB}=${WX_${LIB}} or WX_${LIB}d=${WX_${LIB}d}")
                    SET(wxWidgets_FOUND FALSE)
                ENDIF(WX_${LIB} AND WX_${LIB}d)
            ENDFOREACH(LIB)
        ELSE(WX_USE_REL_AND_DBG)
            DBG_MSG("looking for ${${p_LIBS}}")
            FOREACH(LIB ${${p_LIBS}})
                DBG_MSG("Finding ${LIB}${p_dbg}")
                DBG_MSG("WX_${LIB}${p_dbg} : ${WX_${LIB}${p_dbg}}")
                IF(WX_${LIB}${p_dbg})
                    DBG_MSG("Found ${LIB}${p_dbg}")
                        SET(wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES}
                            ${WX_${LIB}${p_dbg}}
                        )
                ELSE(WX_${LIB}${p_dbg})
                    ERROR_MSG("- not found due to missing library  ${LIB}${p_dbg},\nset to WX_${LIB}${p_dbg}=${WX_${LIB}${p_dbg}}")
                    SET(wxWidgets_FOUND FALSE)
                ENDIF(WX_${LIB}${p_dbg})
            ENDFOREACH(LIB)
        ENDIF(WX_USE_REL_AND_DBG)
        
        FOREACH(LIB ${${p_LIBS}})
            DBG_MSG("required: ${LIB}")
            IF(LIB STREQUAL "gl")
                DBG_MSG("gl required: ${LIB}")
                SET(wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES}
                    opengl32
                    glu32
                )
            ENDIF(LIB STREQUAL "gl")
        ENDFOREACH(LIB ${${p_LIBS}})
    
        SET(wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES} ${wxWidgets_complibs} )
        
    ENDMACRO(WX_SET_LIBRARIES)

    #---------------------------------------------------------------------
    # WIN32: Start actual work.
    #---------------------------------------------------------------------
    #
    # Look for an installation tree.
    #
    IF( MINGW AND NOT wxWidgets_MINGW_FROM_SOURCE )
        
	    FIND_PATH(wxWidgets_ROOT_DIR 
	        NAMES include/common/wx/wx.h
	        PATHS
	        $ENV{wxWidgets_ROOT_DIR}
	        $ENV{WXWIN}
	        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\wxWidgets_is1;Inno Setup: App Path]"  # WX 2.6.x
	        DOC "wxWidgets base/installation directory?"
	    )
		SET( s_wxRootDir_wx_include ${wxWidgets_ROOT_DIR}/include/common )
		SET( s_wxRootDir_contrib_include ${wxWidgets_ROOT_DIR}/include/common )
    ELSE( MINGW AND NOT wxWidgets_MINGW_FROM_SOURCE )   
	    FIND_PATH(wxWidgets_ROOT_DIR 
	        NAMES include/wx/wx.h
	        PATHS
	        $ENV{wxWidgets_ROOT_DIR}
	        $ENV{WXWIN}
	        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\wxWidgets_is1;Inno Setup: App Path]"  # WX 2.6.x
	        DOC "wxWidgets base/installation directory?"
	    )
		SET( s_wxRootDir_wx_include ${wxWidgets_ROOT_DIR}/include )
		SET( s_wxRootDir_contrib_include ${wxWidgets_ROOT_DIR}/contrib/include )
    ENDIF( MINGW AND NOT wxWidgets_MINGW_FROM_SOURCE )   

    # If wxWidgets_ROOT_DIR changed, clear all libraries and lib dir.
    IF(NOT s_wxRootDir STREQUAL wxWidgets_ROOT_DIR)
        SET(s_wxRootDir ${wxWidgets_ROOT_DIR} CACHE INTERNAL "wxWidgets_ROOT_DIR")
        #  WX_CLEAR_ALL_DBG_LIBS()
        #  WX_CLEAR_ALL_REL_LIBS()
        SET(wxWidgets_LIB_DIR "wxWidgets_LIB_DIR-NOTFOUND" CACHE PATH "Cleared." FORCE)
    ENDIF(NOT s_wxRootDir STREQUAL wxWidgets_ROOT_DIR)

    IF(s_wxRootDir)

        # select one default tree inside the already determined wx tree
        # prefer static/shared order usually consistent with build settings
		IF( MINGW AND NOT wxWidgets_MINGW_FROM_SOURCE )
			FIND_PATH(wxWidgets_LIB_DIR
				NAMES ${s_libPre}wxpng.${s_libExt}
				PATHS
				${s_wxRootDir}/lib
				DOC "Path to wxWidgets libraries?"
				NO_DEFAULT_PATH
			)
		ELSE( MINGW AND NOT wxWidgets_MINGW_FROM_SOURCE )
	        IF( wxWidgets_USE_STATIC )
	            FIND_PATH(wxWidgets_LIB_DIR
	                NAMES ${s_libPre}wxpng.${s_libExt} ${s_libPre}wxpngd.${s_libExt}
	                PATHS
	                ${s_wxRootDir}/lib/${s_wxCompiler}_lib  
	                DOC "Path to wxWidgets libraries?"
	                NO_DEFAULT_PATH
	            )
	        ELSE( wxWidgets_USE_STATIC )
	            FIND_PATH(wxWidgets_LIB_DIR
	                NAMES ${s_libPre}wxpng.${s_libExt} ${s_libPre}wxpngd.${s_libExt}
	                PATHS
	                ${s_wxRootDir}/lib/${s_wxCompiler}_dll   
	                DOC "Path to wxWidgets libraries?"
	                NO_DEFAULT_PATH
	            )
	        ENDIF( wxWidgets_USE_STATIC )		
		ENDIF( MINGW AND NOT wxWidgets_MINGW_FROM_SOURCE )
        if ( NOT wxWidgets_LIB_DIR )
                ERROR_MSG("wxWidgets_LIB_DIR for wxWidgets_USE_STATIC = ${wxWidgets_USE_STATIC} was not found")
        endif ( NOT wxWidgets_LIB_DIR )
        # If wxWidgets_LIB_DIR changed, clear all libraries.
        IF(NOT s_wxLibDir STREQUAL wxWidgets_LIB_DIR)
            SET(s_wxLibDir ${wxWidgets_LIB_DIR} CACHE INTERNAL "wxWidgets_LIB_DIR")
            WX_CLEAR_ALL_DBG_LIBS()
            WX_CLEAR_ALL_REL_LIBS()
        ENDIF(NOT s_wxLibDir STREQUAL wxWidgets_LIB_DIR)

        IF(s_wxLibDir)

            SET(wxWidgets_FOUND TRUE)
                   
            IF(s_wxLibDir MATCHES ".*[dD][lL][lL].*")
                DBG_MSG("detected SHARED/DLL tree s_wxLibDir=${s_wxLibDir}")
                # add define for correct dllimport to link against WX DLL
                SET(wxWidgets_DEFINITIONS ${wxWidgets_DEFINITIONS} "-DWXUSINGDLL")
            ENDIF(s_wxLibDir MATCHES ".*[dD][lL][lL].*")

            #---------------------------------------------------------------------
            # WIN32: ???
            #---------------------------------------------------------------------
            # Search for possible configuration type availabilities
            # ***** SET(WX_LAST_CFG "")
            FOREACH(CFG mswunivud mswunivd mswud mswd mswunivu mswuniv mswu msw)
                SET(WX_${CFG}_FOUND FALSE)
                IF(EXISTS ${s_wxLibDir}/${CFG})
                    SET(s_wxConfiguration_LIST ${s_wxConfiguration_LIST} ${CFG})
                    SET(WX_${CFG}_FOUND TRUE)
                    SET(s_wxConfiguration ${CFG})
                ENDIF(EXISTS ${s_wxLibDir}/${CFG})
            ENDFOREACH(CFG)
			           
            # ***** SET(WX_USE_REL_AND_DBG FALSE)
            IF(s_wxConfiguration)
                # if the wanted configuration wasn't found, force the default one
                # else, use it but still force a refresh for the list in doc string
                IF(NOT WX_${wxWidgets_CONFIGURATION}_FOUND)
                    ERROR_MSG( FATAL_ERROR "${wxWidgets_CONFIGURATION} not found" )    
                    SET(wxWidgets_CONFIGURATION ${s_wxConfiguration} CACHE STRING
                    "Set wxWidgets configuration (${s_wxConfiguration_LIST})" FORCE)
                ELSE(NOT WX_${wxWidgets_CONFIGURATION}_FOUND)
                    SET(wxWidgets_CONFIGURATION ${wxWidgets_CONFIGURATION} CACHE STRING
                    "Set wxWidgets configuration (${s_wxConfiguration_LIST})" FORCE)
                ENDIF(NOT WX_${wxWidgets_CONFIGURATION}_FOUND)

                # if release config was selected, and both release/debug exist
                IF (WX_${wxWidgets_CONFIGURATION}d_FOUND)
                    # default to false, to not create string with optimized/debug is not possible in caller.
                    OPTION(wxWidgets_USE_REL_AND_DBG
                    "Use release and debug configurations?" FALSE)
                    SET(WX_USE_REL_AND_DBG ${wxWidgets_USE_REL_AND_DBG})
                ELSE (WX_${wxWidgets_CONFIGURATION}d_FOUND)
                    # if the option exists, force it to false
                    IF(wxWidgets_USE_REL_AND_DBG)
                        SET(wxWidgets_USE_REL_AND_DBG FALSE CACHE BOOL
                        "No ${wxWidgets_CONFIGURATION}d found." FORCE)
                    ENDIF(wxWidgets_USE_REL_AND_DBG)
                    SET(WX_USE_REL_AND_DBG FALSE)
                ENDIF (WX_${wxWidgets_CONFIGURATION}d_FOUND)

                # Set wxWidgets main include directory.
                IF (EXISTS ${s_wxRootDir_wx_include}/wx/wx.h)
                    SET(wxWidgets_INCLUDE_DIRS ${s_wxRootDir_wx_include})
                ELSE (EXISTS ${s_wxRootDir_wx_include}/wx/wx.h)
                    ERROR_MSG("WXWIDGET_FOUND FALSE because s_wxRootDir=${s_wxRootDir} has no ${s_wxRootDir}/include/wx/wx.h")
                    SET(wxWidgets_FOUND FALSE)
                ENDIF (EXISTS ${s_wxRootDir_wx_include}/wx/wx.h)
                	
                # Set wxWidgets lib setup include directory.
                IF (EXISTS ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/wx/setup.h)
                    SET(wxWidgets_INCLUDE_DIRS ${s_wxLibDir}/${wxWidgets_CONFIGURATION} ${wxWidgets_INCLUDE_DIRS} )
                ELSE (EXISTS ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/wx/setup.h)
                    ERROR_MSG("WXWIDGET_FOUND FALSE because  ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/wx/setup.h does not exists.")
                    SET(wxWidgets_FOUND FALSE)
                ENDIF (EXISTS ${s_wxLibDir}/${wxWidgets_CONFIGURATION}/wx/setup.h)

     			find_build_variable( "WXVER_MAJOR" )						
				IF ( "A${wxWidgets_WXVER_MAJOR}" STRGREATER "A" )
					find_build_variable( "WXVER_MINOR" )			
					SET( wxWidgets_PFVERSION "${wxWidgets_WXVER_MAJOR}${wxWidgets_WXVER_MINOR}" )
				ENDIF ( "A${wxWidgets_WXVER_MAJOR}" STRGREATER "A" )
			
				find_build_variable( "USE_GDIPLUS" )						
				IF ( "${wxWidgets_USE_GDIPLUS}" STREQUAL "1" )
				    list( APPEND wxWidgets_complibs gdiplus )
				ENDIF ( "${wxWidgets_USE_GDIPLUS}" STREQUAL "1" )
				find_build_variable( "USE_OPENGL" )	
				IF ( "${wxWidgets_USE_OPENGL}" STREQUAL "1" )
				    SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_FIND_COMPONENTS} gl )
				ENDIF ( "${wxWidgets_USE_OPENGL}" STREQUAL "1" )
					           
                IF ( EXISTS ${s_wxRootDir_contrib_include} )
                    SET( wxWidgets_INCLUDE_DIRS ${wxWidgets_INCLUDE_DIRS} ${s_wxRootDir_contrib_include} )
                ENDIF ( EXISTS ${s_wxRootDir_contrib_include} )
            
                # Find wxWidgets libraries.
                WX_FIND_LIBS("${wxWidgets_UNV}" "${wxWidgets_UCD}" "${wxWidgets_DBG}")
                IF (WX_USE_REL_AND_DBG)
                    WX_FIND_LIBS("${wxWidgets_UNV}" "${wxWidgets_UCD}" "d")
                ENDIF (WX_USE_REL_AND_DBG)
                                               
                # Libraries we are interested in.
                IF ( NOT wxWidgets_FIND_COMPONENTS )
                    # Default minimal use setting (i.e., link to only core,base).
                    SET(wxWidgets_FIND_COMPONENTS ${wxWidgets_STD_LIBRARIES} )
                ENDIF ( NOT wxWidgets_FIND_COMPONENTS )
                
                IF (wxWidgets_FIND_COMPONENTS MATCHES std)
                    # replace std by the list of STD libs
                    LIST(APPEND wxWidgets_FIND_COMPONENTS ${wxWidgets_STD_LIBRARIES} )
                    LIST(REMOVE_ITEM wxWidgets_FIND_COMPONENTS std)
                    # TODO: check that "mono"  and base,core aren't added together
                ENDIF (wxWidgets_FIND_COMPONENTS MATCHES std)
            
                # Always add the common required libs.
                LIST(APPEND wxWidgets_FIND_COMPONENTS ${wxWidgets_COMMON_LIBRARIES} )
                
                # Settings for requested libs (i.e., include dir, libraries, etc.).
                WX_SET_LIBRARIES(wxWidgets_FIND_COMPONENTS "${wxWidgets_DBG}")
                
                SET( wxWidgets_RC ${s_wxRootDir_wx_include}/wx/msw/wx.rc )
                # used in ADD_EXECUTABLE is the WIN32GUI to tell it is a typical windows GUI executable.
                SET( WIN32GUI WIN32 )    
            
            ENDIF (s_wxConfiguration)
		ENDIF (s_wxLibDir)
	ENDIF (s_wxRootDir)

    SET( wxWidgets_INCLUDE_DIRS ${wxWidgets_INCLUDE_DIRS} CACHE STRING  "" FORCE)
	MARK_AS_ADVANCED( FORCE wxWidgets_INCLUDE_DIRS )
    SET( wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES} CACHE STRING  "" FORCE)
	MARK_AS_ADVANCED( FORCE wxWidgets_LIBRARIES )
    SET( wxWidgets_LIB_DIRS ${wxWidgets_LIB_DIRS} CACHE STRING  "" FORCE)
	MARK_AS_ADVANCED( FORCE wxWidgets_LIB_DIRS )
    SET( wxWidgets_CXX_FLAGS ${wxWidgets_CXX_FLAGS} CACHE STRING  "" FORCE)
	MARK_AS_ADVANCED( FORCE wxWidgets_CXX_FLAGS )
    SET( wxWidgets_DEFINITIONS ${wxWidgets_DEFINITIONS} CACHE STRING  "" FORCE)
	MARK_AS_ADVANCED( FORCE wxWidgets_DEFINITIONS )
	
    IF (wxWidgets_FOUND)
        IF(NOT wxWidgets_FIND_QUIETLY)
            MESSAGE( STATUS "Configuration from ${wxWidgets_ROOT_DIR}" )
            MESSAGE( STATUS "wxWidgets_USE_DEBUG       : ${wxWidgets_USE_DEBUG}" )
            MESSAGE( STATUS "wxWidgets_USE_UNICODE     : ${wxWidgets_USE_UNICODE}" )
            MESSAGE( STATUS "wxWidgets_USE_STATIC      : ${wxWidgets_USE_STATIC}" )
            MESSAGE( STATUS "wxWidgets_USE_UNIVERSEL   : ${wxWidgets_USE_UNIVERSEL}" )            
            MESSAGE( STATUS "wxWidgets_FOUND           : ${wxWidgets_FOUND}")
            MESSAGE( STATUS "wxWidgets_INCLUDE_DIRS    : ${wxWidgets_INCLUDE_DIRS}")
            MESSAGE( STATUS "wxWidgets_LIBRARY_DIRS    : ${wxWidgets_LIBRARY_DIRS}")
            MESSAGE( STATUS "wxWidgets_CXX_FLAGS       : ${wxWidgets_CXX_FLAGS}")
            MESSAGE( STATUS "wxWidgets_FIND_COMPONENTS : ${wxWidgets_FIND_COMPONENTS}")
        ENDIF(NOT wxWidgets_FIND_QUIETLY)                     
    ENDIF(wxWidgets_FOUND)   

ENDIF ( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )

#=====================================================================
# unix style find
#=====================================================================
IF( ${wxWidgets_FIND_STYLE} STREQUAL "UNIX_STYLE_FIND" )

    SET( wxWidgets_SELECT_OPTIONS )
    
    # WASCANA needed this
    #SET( wxWidgets_SELECT_OPTIONS "${wxWidgets_SELECT_OPTIONS} --prefix=/mingw")

    #-----------------------------------------------------------------
    # UNIX: Helper MACROS
    #-----------------------------------------------------------------
    #
    # Set the default values based on "wx-config --selected-config".
    #
    MACRO(WX_CONFIG_SELECT_GET_DEFAULT)
        if ( NOT DEFINED wxWidgets_SELECTED_CONFIG )
    
            EXECUTE_PROCESS(
            COMMAND sh "${wxWidgets_CONFIG_EXECUTABLE}" --selected-config
            OUTPUT_VARIABLE wxWidgets_SELECTED_CONFIG
            RESULT_VARIABLE _wx_result
            ERROR_QUIET
            )
            IF( _wx_result EQUAL 0)
                SET( wxWidgets_SELECTED_CONFIG ${wxWidgets_SELECTED_CONFIG} CACHE STRING
                                    "Default wxWidgets configuration"  FORCE )
                FOREACH(_opt_name debug static unicode universal)
                    STRING(TOUPPER ${_opt_name} _upper_opt_name)
                    IF( wxWidgets_SELECTED_CONFIG MATCHES ".*${_opt_name}.*")
                        SET(wxWidgets_USE_${_upper_opt_name} ON CACHE  BOOL
                                    "Default wxWidgets configuration"  FORCE )
                    ELSE( wxWidgets_SELECTED_CONFIG MATCHES ".*${_opt_name}.*")
                        SET(wxWidgets_USE_${_upper_opt_name} OFF CACHE BOOL 
                                    "Default wxWidgets configuration"  FORCE )
                    ENDIF( wxWidgets_SELECTED_CONFIG MATCHES ".*${_opt_name}.*")
                ENDFOREACH(_opt_name)
                
                # based on this output figure out the default wxWidgets_PORT, which can be set different by user. 
                STRING( REGEX REPLACE "^([^\\-]+).*" "\\1" wxWidgets_PORT "${wxWidgets_SELECTED_CONFIG}" )
                # gtk|gtk2|x11|motif|dfb|mac|mgl        
                SET( wxWidgets_PORT ${wxWidgets_PORT} CACHE STRING
                                    "Set wxWidgets configuration (${wxWidgets_PORT})"  FORCE )    
                STRING( REGEX MATCH "([0-9].[0-9]).[0-9]*" wxWidgets_PFVERSION "${wxWidgets_SELECTED_CONFIG}" )
                SET( wxWidgets_PFVERSION ${wxWidgets_PFVERSION} CACHE STRING
                                    "Set wxWidgets version (${wxWidgets_PFVERSION})"  FORCE )

            ELSE( _wx_result EQUAL 0)
                FOREACH(_upper_opt_name DEBUG STATIC UNICODE UNIVERSAL)
                    SET(wxWidgets_USE_${_upper_opt_name} OFF)
                ENDFOREACH(_upper_opt_name)
                SET(wxWidgets_FOUND FALSE)
                SET( wxWidgets_PORT "" CACHE STRING
                                "Set wxWidgets configuration"  FORCE )    
                SET( wxWidgets_PFVERSION "" CACHE STRING
                                    "Set wxWidgets version"  FORCE )   
                if ( wxWidgets_FIND_REQUIRED )
                    ERROR_MSG( "no default configuration found: ${wxWidgets_CONFIG_EXECUTABLE} --selected_config failed")                    
                endif ( wxWidgets_FIND_REQUIRED )
            ENDIF( _wx_result EQUAL 0)
        
        endif( NOT DEFINED wxWidgets_SELECTED_CONFIG )
    ENDMACRO(WX_CONFIG_SELECT_GET_DEFAULT)

    # 
    # Set wxWidgets_SELECT_OPTIONS to wx-config options for selecting
    # among multiple builds.
    #
    MACRO(WX_CONFIG_SELECT_SET_OPTIONS)
      SET(wxWidgets_SELECT_OPTIONS "")
      foreach(_opt_name debug static unicode universal)
        STRING(TOUPPER ${_opt_name} _upper_opt_name)
        if( DEFINED wxWidgets_USE_${_upper_opt_name} )
          if( wxWidgets_USE_${_upper_opt_name} )
            list(APPEND wxWidgets_SELECT_OPTIONS --${_opt_name}=yes)
          else( wxWidgets_USE_${_upper_opt_name} )
            list(APPEND wxWidgets_SELECT_OPTIONS --${_opt_name}=no)
          endif( wxWidgets_USE_${_upper_opt_name} )
        else( DEFINED wxWidgets_USE_${_upper_opt_name} )
            list(APPEND wxWidgets_SELECT_OPTIONS --${_opt_name}=no)
        endif( DEFINED wxWidgets_USE_${_upper_opt_name} )
      endforeach(_opt_name)
      list( APPEND wxWidgets_SELECT_OPTIONS --toolkit=${wxWidgets_PORT} )
      list( APPEND wxWidgets_SELECT_OPTIONS --version=${wxWidgets_PFVERSION} )
    ENDMACRO(WX_CONFIG_SELECT_SET_OPTIONS)

    #-----------------------------------------------------------------
    # UNIX: Start actual work.
    #-----------------------------------------------------------------
    # Support cross-compiling, only search in the target platform.
    #FIND_PROGRAM(wxWidgets_CONFIG_EXECUTABLE wx-config
    #  ONLY_CMAKE_FIND_ROOT_PATH
    #  )
   
    # if wxWidgets_ROOT_DIR or WXWIN is set, use them, else via PATH
    FIND_PROGRAM(
        wxWidgets_CONFIG_EXECUTABLE
        NAMES wx-config
        PATHS
        $ENV{wxWidgets_ROOT_DIR}
        $ENV{WXWIN}
    )    

    IF(wxWidgets_CONFIG_EXECUTABLE)

        # get defaults based on "wx-config --selected-config" only the first time
        WX_CONFIG_SELECT_GET_DEFAULT()

        # process selection to set wxWidgets_SELECT_OPTIONS
        WX_CONFIG_SELECT_SET_OPTIONS()
        DBG_MSG("wxWidgets_SELECT_OPTIONS=${wxWidgets_SELECT_OPTIONS}")

        # define the query string for wx-config, to test for the wanted wxWidgets configuration
        IF( wxWidgets_USE_STATIC )
            SET( WXLIBEXT ".a" )
        ELSE( wxWidgets_USE_STATIC )
            SET( WXLIBEXT ".so" )
        ENDIF( wxWidgets_USE_STATIC ) 

        # try to get the default wxWidgets configuration by query to wx-config
        SET(wxWidgets_FOUND TRUE)
                                   
        # do the config test for all the wanted/found options to see if it is available.       
        execute_process(
            COMMAND sh "${wxWidgets_CONFIG_EXECUTABLE}" ${wxWidgets_SELECT_OPTIONS}            
            OUTPUT_VARIABLE wxWidgets_WANTED_WANTED
            RESULT_VARIABLE wxWidgets_WANTED_AVAILABLE
            ERROR_QUIET
        )   
        IF ( wxWidgets_WANTED_AVAILABLE EQUAL 0 )
            # if this went oke, the extra information will be retrieved like flags paths prefix etc.
          
            # run the wx-config program to get cxxflags
            execute_process(
                COMMAND sh "${wxWidgets_CONFIG_EXECUTABLE}" ${wxWidgets_SELECT_OPTIONS} --cxxflags
                OUTPUT_VARIABLE wxWidgets_CXX_FLAGS
                RESULT_VARIABLE RET
                ERROR_QUIET
            )   
            IF( RET EQUAL 0 )
                # parse definitions from cxxflags

                STRING( REGEX REPLACE "[\r\n]+$" "" wxWidgets_CXX_FLAGS  ${wxWidgets_CXX_FLAGS} )
                STRING(REGEX MATCHALL "-D.*[^ ;]+" wxWidgets_DEFINITIONS  ${wxWidgets_CXX_FLAGS})           
                separate_arguments( wxWidgets_DEFINITIONS )
    			DBG_MSG("\nwxWidgets_DEFINITIONS=${wxWidgets_DEFINITIONS}")
    			
                # drop -D* from CXXFLAGS
                # STRING(REGEX REPLACE "-D[^ ;]*" ""  wxWidgets_CXX_FLAGS  ${wxWidgets_CXX_FLAGS})
                #STRING(REGEX REPLACE ";-D[^;]+$" "" wxWidgets_CXX_FLAGS "${wxWidgets_CXX_FLAGS}")
                #STRING(REGEX REPLACE "^-D[^;]+$" "" wxWidgets_CXX_FLAGS "${wxWidgets_CXX_FLAGS}")            

                STRING(REGEX REPLACE "-D.*[^ ;]+" "" wxWidgets_CXX_FLAGS "${wxWidgets_CXX_FLAGS}")
                
                # parse incdirs from cxxflags, drop -I prefix
                STRING(REGEX MATCHALL "-I[^ ;]*" wxWidgets_INCLUDE_DIRS  ${wxWidgets_CXX_FLAGS})
                STRING(REGEX REPLACE "-I" ""  wxWidgets_INCLUDE_DIRS "${wxWidgets_INCLUDE_DIRS}")
                separate_arguments( wxWidgets_INCLUDE_DIRS )
                # drop -I* from CXXFLAGS
                STRING(REGEX REPLACE "-I[^ ;]*" ""  wxWidgets_CXX_FLAGS  ${wxWidgets_CXX_FLAGS})
            ELSE( RET EQUAL 0 )
                ERROR_MSG("${wxWidgets_CONFIG_EXECUTABLE} ${wxWidgets_SELECT_OPTIONS};--cxxflags FAILED with RET=${RET}")
                SET(wxWidgets_FOUND FALSE)
            ENDIF( RET EQUAL 0 )

            # run the wx-config program to get the libs
            # - NOTE: wx-config doesn't verify that the libs requested exist
            #         it just produces the names. Maybe a TRY_COMPILE would
            #         be useful here...
            #STRING(REPLACE ";" "," wxWidgets_FIND_COMPONENTS "${wxWidgets_FIND_COMPONENTS}")
            STRING(REGEX REPLACE ";" "," wxWidgets_FIND_COMPONENTS "${wxWidgets_FIND_COMPONENTS}")
            execute_process(
                COMMAND sh ${wxWidgets_CONFIG_EXECUTABLE} ${wxWidgets_SELECT_OPTIONS} --libs ${wxWidgets_FIND_COMPONENTS}
                OUTPUT_VARIABLE wxWidgets_LIBRARIES
                RESULT_VARIABLE RET
                ERROR_QUIET
            )
            IF( "${RET}" STREQUAL "0" )
                STRING(REGEX REPLACE " " ";" wxWidgets_LIBRARIES "${wxWidgets_LIBRARIES}")
                STRING(REGEX REPLACE "-framework;" "-framework " wxWidgets_LIBRARIES "${wxWidgets_LIBRARIES}")
                
                # extract linkdirs (-L) for rpath (i.e., LINK_DIRECTORIES)
                STRING(REGEX MATCHALL "-L[^ ;]*" wxWidgets_LIBRARY_DIRS  "${wxWidgets_LIBRARIES}")
                STRING(REGEX REPLACE "-L" ""  wxWidgets_LIBRARY_DIRS "${wxWidgets_LIBRARY_DIRS}")
                # convert space to semicolons for list
                STRING(REGEX REPLACE " " ";" wxWidgets_LIBRARY_DIRS "${wxWidgets_LIBRARY_DIRS}")

                STRING(REGEX REPLACE "-L[^ ;]*" ""  wxWidgets_LIBRARIES "${wxWidgets_LIBRARIES}")
                STRING(REGEX MATCHALL "-l[^ ;]*" wxWidgets_LIBRARIES_dynamic  "${wxWidgets_LIBRARIES}")
                STRING(REGEX MATCHALL "/[^ ;]*\\.a" wxWidgets_LIBRARIES_static  "${wxWidgets_LIBRARIES}")
    			# first add static full path next dynamic and/or static using -l option 
                # ( options like -static -Bstatic -Bdynamic or ignored currently )
    			# with -l option can be shared  or static, but shared is searched first.  
                # So a split should be made, but wx-config does not use this -Bstatic etc. flags.
    			SET( wxWidgets_LIBRARIES "${wxWidgets_LIBRARIES_static};${wxWidgets_LIBRARIES_dynamic}" )
                STRING(REGEX REPLACE "-l" ""  wxWidgets_LIBRARIES "${wxWidgets_LIBRARIES}")
            ELSE( "${RET}" STREQUAL "0" )
                ERROR_MSG("${wxWidgets_CONFIG_EXECUTABLE} ${wxWidgets_SELECT_OPTIONS} --libs ${wxWidgets_FIND_COMPONENTS} FAILED with RET=${RET}")
                SET(wxWidgets_FOUND FALSE)
            ENDIF( "${RET}" STREQUAL "0" )
          
            # get the path to the libraries and include files e.g /usr/local/
            execute_process(
                COMMAND sh ${wxWidgets_CONFIG_EXECUTABLE} ${wxWidgets_SELECT_OPTIONS} --prefix
                OUTPUT_VARIABLE wxWidgets_PREFIX
                RESULT_VARIABLE RET
                ERROR_QUIET
            )
            STRING( REGEX REPLACE "[\r\n]+$" "" wxWidgets_PREFIX ${wxWidgets_PREFIX} )

            IF( CYGWIN OR MINGW )
                GET_FILENAME_COMPONENT(wxWidgets_ROOT_DIR ${wxWidgets_CONFIG_EXECUTABLE} PATH)
                SET( wxWidgets_ROOT_DIR ${wxWidgets_ROOT_DIR}/.. )
                #SET( wxWidgets_RC ${wxWidgets_ROOT_DIR}/include/wx/msw/wx.rc )
            ELSE( CYGWIN OR MINGW )
                SET( wxWidgets_RC "" )
            ENDIF( CYGWIN OR MINGW )
            SET( WIN32GUI "" )
          
            SET(wxWidgets_LIB_PATH "${wxWidgets_PREFIX}/lib" )

        ELSE ( wxWidgets_WANTED_AVAILABLE EQUAL 0 )
            SET(wxWidgets_FOUND FALSE)            
            ERROR_MSG( "\n${wxWidgets_SELECT_OPTIONS} failed. \nThe default configuration found with: ${wxWidgets_CONFIG_EXECUTABLE} --selected_config \nis the following: ${wxWidgets_SELECTED_CONFIG}\n" )
        ENDIF ( wxWidgets_WANTED_AVAILABLE EQUAL 0 )
                    
    ENDIF(wxWidgets_CONFIG_EXECUTABLE)
        
    IF (wxWidgets_FOUND)
        IF(NOT wxWidgets_FIND_QUIETLY)
            MESSAGE( STATUS "Configuration ${wxWidgets_CONFIG_EXECUTABLE} ${wxWidgets_SELECT_OPTIONS}" )
            MESSAGE( STATUS "wxWidgets_USE_DEBUG       : ${wxWidgets_USE_DEBUG}" )
            MESSAGE( STATUS "wxWidgets_USE_UNICODE     : ${wxWidgets_USE_UNICODE}" )
            MESSAGE( STATUS "wxWidgets_USE_STATIC      : ${wxWidgets_USE_STATIC}" )
            MESSAGE( STATUS "wxWidgets_USE_UNIVERSEL   : ${wxWidgets_USE_UNIVERSEL}" )            
            MESSAGE( STATUS "wxWidgets_FOUND           : ${wxWidgets_FOUND}")
            MESSAGE( STATUS "wxWidgets_INCLUDE_DIRS    : ${wxWidgets_INCLUDE_DIRS}")
            MESSAGE( STATUS "wxWidgets_LIBRARY_DIRS    : ${wxWidgets_LIBRARY_DIRS}")
            MESSAGE( STATUS "wxWidgets_CXX_FLAGS       : ${wxWidgets_CXX_FLAGS}")
            MESSAGE( STATUS "wxWidgets_DEFINITIONS     : ${wxWidgets_DEFINITIONS}")            
            MESSAGE( STATUS "wxWidgets_FIND_COMPONENTS : ${wxWidgets_FIND_COMPONENTS}")
        ENDIF(NOT wxWidgets_FIND_QUIETLY)                     
    ENDIF(wxWidgets_FOUND)   

ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "UNIX_STYLE_FIND" )

#=====================================================================
# no style error
#=====================================================================
IF( ${wxWidgets_FIND_STYLE} STREQUAL "NOT_DEFINED_STYLE_FIND" )
    IF(NOT wxWidgets_FIND_QUIETLY)
        MESSAGE(STATUS "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): \n"
        "  Platform unknown/unsupported. It's neither WIN32 nor UNIX style find.")
    ENDIF(NOT wxWidgets_FIND_QUIETLY)
ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "NOT_DEFINED_STYLE_FIND" )

# add convenience use file
IF (wxWidgets_FOUND)
    # get dir of this file which may reside in
    # - CMAKE_MAKE_ROOT/Modules  on CMake installation
    # - CMAKE_MODULE_PATH if user prefers his own specialized version
    GET_FILENAME_COMPONENT(wxWidgets_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
    SET(wxWidgets_USE_FILE "${wxWidgets_CURRENT_LIST_DIR}/UsewxWidgets.cmake")
    # check
    IF (NOT EXISTS ${wxWidgets_USE_FILE})
        IF ( NOT  wxWidgets_FIND_QUIETLY)
            MESSAGE( "Your Find/Use wxWidgets installation is wrong. wxWidgets_USE_FILE=${wxWidgets_USE_FILE} not found.")
        ENDIF(NOT  wxWidgets_FIND_QUIETLY)
    ENDIF(NOT EXISTS ${wxWidgets_USE_FILE})    
ELSE(wxWidgets_FOUND)
    # make FIND_PACKAGE friendly
    IF(NOT wxWidgets_FIND_QUIETLY)
        IF(wxWidgets_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR
            "wxWidgets required, please specify it's location.")
        ELSE(wxWidgets_FIND_REQUIRED)
            MESSAGE(STATUS "ERROR: wxWidgets was not found.")
        ENDIF(wxWidgets_FIND_REQUIRED)
    ENDIF(NOT wxWidgets_FIND_QUIETLY)    
ENDIF(wxWidgets_FOUND)

DBG_MSG("wxWidgets_FOUND           : ${wxWidgets_FOUND}")
DBG_MSG("wxWidgets_INCLUDE_DIRS    : ${wxWidgets_INCLUDE_DIRS}")
DBG_MSG("wxWidgets_LIBRARY_DIRS    : ${wxWidgets_LIBRARY_DIRS}")
DBG_MSG("wxWidgets_DEFINITIONS     : ${wxWidgets_DEFINITIONS}")
DBG_MSG("wxWidgets_CXX_FLAGS       : ${wxWidgets_CXX_FLAGS}")
DBG_MSG("wxWidgets_LIBRARIES   	   : ${wxWidgets_LIBRARIES}")
DBG_MSG("wxWidgets_USE_FILE        : ${wxWidgets_USE_FILE}")
DBG_MSG("wxWidgets_FIND_COMPONENTS : ${wxWidgets_FIND_COMPONENTS}")

#=====================================================================
# to find a wxWidgets package
# uses variable values from FindwxWid${wxWidgets_PORT}gets.cmake when it was included
#
# Input:
#   packName = name of the package e.g WXLUA ( converted to lowercase is used to search libraries }
#   packDir = The place where the package is searched for.
#   packWxlikeLibs = libraries named in a wxlike fashion and depend on cached variables wxWidgets_PORT wxWidgets_PFVERSION wxWidgets_UNV wxWidgets_UCD wxWidgets_DBG
#   packLibs = libraries names as is
#   packIncludes = include paths to check
#   packNeedfiles = files to check
#
# The following are set after macro is called:
#
#  ${packName}_FOUND             - Set to TRUE if package was found.
#  ${packName}_INCLUDE_DIRS - Include directories 
#  ${packName}_LIBRARIES        - Path to the package libraries.
#  ${packName}_LIBRARY_DIRS  - compile time link dirs
#  ${packName}_ROOT_DIR  - where is the package installed
#
# Sample usage:
#
#   FIND_WX_PACKAGE( WXLUA $ENV{WXLUA} "wxluadebug;wxluasocket;wxlua;wxbind" "lua5.1" "modules/lua/include;modules/wxbind/setup;modules" "" )
#   IF( WXLUA_FOUND )
#     # and for each of your dependant executable/library targets:
#     INCLUDE_DIRECTORIES( ${WXLUA_INCLUDE_DIRS} )
#     LINK_DIRECTORIES( ${WXLUA_LIBRARY_DIRS} )
#     TARGET_LINK_LIBRARIES(<YourTarget> ${WXLUA_LIBRARIES})
#   ENDIF( WXLUA_FOUND )
#
#=====================================================================
MACRO( FIND_WX_PACKAGE p_packName p_packDir p_packWxlikeLibs p_packLibs p_packIncludes p_packNeedfiles )

    STRING( TOLOWER ${p_packName} s_packNameLower )

    IF( IS_DIRECTORY ${p_packDir})
        SET( s_packFound TRUE )                      
    ELSE( IS_DIRECTORY ${p_packDir} )
        SET( s_packFound FALSE )                      
        MESSAGE( FATAL_ERROR "ERROR: package install path: \"${p_packDir}\" not found or is empty" )    
    ENDIF( IS_DIRECTORY ${p_packDir} )
    SET( ${p_packName}_ROOT_DIR ${p_packDir} CACHE STRING "package install path" )

    FOREACH( NEEDFILE ${p_packNeedfiles} )
        FIND_FILE( 
            FOUND_NEEDFILE
            NAMES
            ${NEEDFILE} 
            PATHS 
            ${p_packDir}
            /usr/include/${s_packNameLower}
            /usr/local/include/${s_packNameLower}
            ${CMAKE_INSTALL_PREFIX}/include/${s_packNameLower};
        )
        IF( NOT FOUND_NEEDFILE )
            MESSAGE( "ERROR: file: ${NEEDFILE} not found" )    
        ENDIF( NOT FOUND_NEEDFILE )
        MARK_AS_ADVANCED( FOUND_NEEDFILE )
    ENDFOREACH( NEEDFILE )    

    # form the naming 
    IF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
        IF( wxWidgets_USE_STATIC )
            SET( s_packLibraryDirs ${p_packDir}/lib/${s_wxCompiler}_lib )
        ELSE( wxWidgets_USE_STATIC )
            SET( s_packLibraryDirs ${p_packDir}/lib/${s_wxCompiler}_dll )
        ENDIF( wxWidgets_USE_STATIC )    
    ELSE( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )    
        SET( s_packLibraryDirs "${p_packDir}/lib;${CMAKE_INSTALL_PREFIX}/lib;/usr/local/lib;/usr/lib" )        
    ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
   
    SET( s_packLibraries "" )     
    # Find package libraries with naming like wxWidgets libraries
    FOREACH( packLib ${p_packWxlikeLibs} )
        IF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )      
            SET( wxlikelib ${s_packNameLower}_${wxWidgets_PORT}${wxWidgets_PFVERSION}${wxWidgets_UNV}${wxWidgets_UCD}${wxWidgets_DBG}_${packLib} )
            SET( wxLikeLibAlt wx${wxWidgets_PORT}${wxWidgets_PFVERSION}${wxWidgets_UNV}${wxWidgets_UCD}${wxWidgets_DBG}_${packLib} )
        ELSE( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )    
            SET( wxlikelib ${s_packNameLower}_${wxWidgets_PORT}${wxWidgets_UNV}${wxWidgets_UCD}${wxWidgets_DBG}_${packLib}-${wxWidgets_PFVERSION} )
            SET( wxLikeLibAlt wx_${wxWidgets_PORT}${wxWidgets_UNV}${wxWidgets_UCD}${wxWidgets_DBG}_${packLib}-${wxWidgets_PFVERSION} )            
        ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
        FIND_LIBRARY( ${p_packName}_${packLib}
            NAMES
            ${wxlikelib}
            PATHS ${s_packLibraryDirs}
            PATHS ${s_packLibraryDirs}/Debug
            PATHS ${s_packLibraryDirs}/Release
            NO_DEFAULT_PATH
        )   
        IF( NOT ${p_packName}_${packLib} )
            # try other naming scheme
            FIND_LIBRARY( ${p_packName}_alt_${packLib}
                NAMES
                ${wxLikeLibAlt}
                PATHS ${p_packDir}/lib
                PATHS ${p_packDir}/lib/Debug
                PATHS ${p_packDir}/lib/Release
                NO_DEFAULT_PATH
            )   
            IF( NOT ${p_packName}_alt_${packLib} )
                MESSAGE( "ERROR: ${packLib} as ${wxlikelib} or ${wxLikeLibAlt} not found in ${s_packLibraryDirs}" )    
            ELSE( NOT ${p_packName}_alt_${packLib} )
                GET_FILENAME_COMPONENT( ${p_packName}_${packLib} ${${p_packName}_alt_${packLib}} NAME_WE )
                SET( s_packLibraries ${s_packLibraries} ${${p_packName}_${packLib}} )                     
                MARK_AS_ADVANCED( ${p_packName}_${packLib} ) 
                SET( s_packLibraryDirs ${s_packLibraryDirs} ${p_packDir}/lib )        
            ENDIF( NOT ${p_packName}_alt_${packLib} )
        ELSE( NOT ${p_packName}_${packLib} )
            IF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
            GET_FILENAME_COMPONENT( ${p_packName}_${packLib} ${${p_packName}_${packLib}} NAME_WE )
            ELSE( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
                STRING(REGEX REPLACE "\\.a" "" ${p_packName}_${packLib} "${${p_packName}_${packLib}}")
                GET_FILENAME_COMPONENT( ${p_packName}_${packLib}  ${${p_packName}_${packLib}} NAME )
                STRING(REGEX REPLACE "^lib" "" ${p_packName}_${packLib} "${${p_packName}_${packLib}}")
            ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )

            SET( s_packLibraries ${s_packLibraries} ${${p_packName}_${packLib}} )     
            MARK_AS_ADVANCED( ${p_packName}_${packLib} )           
        ENDIF( NOT ${p_packName}_${packLib} )
    ENDFOREACH(packLib)        
  
    # Find package libraries with any name
    FOREACH( packLib ${p_packLibs} )
        FIND_LIBRARY( ${p_packName}_${packLib}
            NAMES
            ${packLib}
            PATHS ${s_packLibraryDirs}
            NO_DEFAULT_PATH
        )   
        IF( NOT ${p_packName}_${packLib} )
            MESSAGE( "ERROR: ${packLib} not found in ${s_packLibraryDirs}" )    
        ENDIF( NOT ${p_packName}_${packLib} )
        GET_FILENAME_COMPONENT( ${p_packName}_${packLib} ${${p_packName}_${packLib}} NAME )
        SET( s_packLibraries ${s_packLibraries} ${${p_packName}_${packLib}} )              
        MARK_AS_ADVANCED( ${p_packName}_${packLib} )
    ENDFOREACH( packLib )    

    FOREACH( INCLUDE ${p_packIncludes} )
        IF( EXISTS ${p_packDir}/${INCLUDE} )
            SET( s_packIncludeDirs ${s_packIncludeDirs} ${p_packDir}/${INCLUDE} )                      
        ELSE( EXISTS ${p_packDir}/${INCLUDE} )
            MESSAGE( "ERROR: ${INCLUDE} not found in ${p_packDir}/${INCLUDE}" )    
        ENDIF( EXISTS ${p_packDir}/${INCLUDE} )
    ENDFOREACH( INCLUDE )    

    SET( ${p_packName}_LIBRARY_DIRS ${s_packLibraryDirs} CACHE STRING "package libs directory path" FORCE )
    SET( ${p_packName}_FOUND ${s_packFound} CACHE STRING "package was found" FORCE )                      
    SET( ${p_packName}_LIBRARIES ${s_packLibraries} CACHE STRING "package libs" FORCE )              
    SET( ${p_packName}_INCLUDE_DIRS ${s_packIncludeDirs} CACHE STRING "package include paths" FORCE )    
    MARK_AS_ADVANCED( ${p_packName}_FOUND )
    MARK_AS_ADVANCED( ${p_packName}_ROOT_DIR )
    MARK_AS_ADVANCED( ${p_packName}_LIBRARIES )
    MARK_AS_ADVANCED( ${p_packName}_INCLUDE_DIRS )
    MARK_AS_ADVANCED( ${p_packName}_LIBRARY_DIRS )
    
ENDMACRO( FIND_WX_PACKAGE )

#=====================================================================
# *.rc files are not automatically compiled by cmake on mingw/cygwin
#=====================================================================
MACRO( COMPILE_RC p_srcFile p_dstFile )
    IF( CYGWIN OR MINGW )
        ADD_CUSTOM_COMMAND(
            OUTPUT ${p_dstFile}
            COMMAND windres
            ARGS -i ${p_srcFile} -o ${p_dstFile} --include-dir ${wxWidgets_ROOT_DIR}/include
        )
    ENDIF( CYGWIN OR MINGW )
ENDMACRO( COMPILE_RC )

#=====================================================================
# After an ADD_LIBRARY command, this can be used to set the right properties, to generate wxWidgets like naming
# This is based on information found in the wxWidegts detection fase.
#=====================================================================
MACRO( SET_AS_WXLIKE_LIBRARY p_wxpackage p_libraryname )
    IF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
        SET( OUTPUTLIBPREFIX ${p_wxpackage}_${wxWidgets_PORT}${wxWidgets_UNV}${wxWidgets_PFVERSION}${wxWidgets_UCD}${wxWidgets_DBG}_ )
        SET( OUTPUTLIBPOSTFIX "" )
    ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
    IF( ${wxWidgets_FIND_STYLE} STREQUAL "UNIX_STYLE_FIND" )
        # how will output libraries be  prefixed
        SET( OUTPUTLIBPREFIX ${p_wxpackage}_${wxWidgets_PORT}${wxWidgets_UNV}${wxWidgets_UCD}${wxWidgets_DBG}_ )
        SET( OUTPUTLIBPOSTFIX "-${wxWidgets_PFVERSION}" )      
    ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "UNIX_STYLE_FIND" )
    
    SET_TARGET_PROPERTIES( ${p_libraryname} PROPERTIES OUTPUT_NAME ${OUTPUTLIBPREFIX}${p_libraryname}${OUTPUTLIBPOSTFIX} )
ENDMACRO( SET_AS_WXLIKE_LIBRARY )

#=====================================================================
# To set library output path for wx packages, in the unix or windows manner
#=====================================================================
MACRO( SET_WXLIKE_LIBRARY_PATH p_rootdir )
    IF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
        IF( wxWidgets_USE_STATIC )
            SET ( LIBRARY_OUTPUT_PATH ${p_rootdir}/lib/${s_wxCompiler}_lib CACHE PATH "output directory for building library" FORCE )
        ELSE( wxWidgets_USE_STATIC )
            SET ( LIBRARY_OUTPUT_PATH ${p_rootdir}/lib/${s_wxCompiler}_dll CACHE PATH "output directory for building library" FORCE )
        ENDIF( wxWidgets_USE_STATIC )
    ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
    IF( ${wxWidgets_FIND_STYLE} STREQUAL "UNIX_STYLE_FIND" )
        SET ( LIBRARY_OUTPUT_PATH ${p_rootdir}/lib CACHE PATH "output directory for building library" FORCE )
    ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "UNIX_STYLE_FIND" )
ENDMACRO( SET_WXLIKE_LIBRARY_PATH )

##################################################
# Set flags of wrong configuration to something erroneous, this will give compiler errors when still used.
# Understand that the configuration is always only suitable for one configuration ( meaning choosing
# another configuration then the one wanted in VC IDE does not make sence ).
##################################################
MACRO( SET_WX_SYNCLIBRARY_FLAGS )
    IF( wxWidgets_USE_DEBUG )
        SET( CMAKE_CONFIGURATION_TYPES "Debug" )
        SET (CMAKE_BUILD_TYPE  "Debug" )
        SET( CMAKE_CXX_FLAGS_RELEASE
    "THIS_IS_A_DEBUG_CONFIGURATION_AND_YOU_ARE_DOING_A_RELEASE_BUILD" )
        SET( CMAKE_CXX_FLAGS_MINSIZEREL
    "THIS_IS_A_DEBUG_CONFIGURATION_AND_YOU_ARE_DOING_A_RELEASE_BUILD" )
        SET( CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "THIS_IS_A_DEBUG_CONFIGURATION_AND_YOU_ARE_DOING_A_RELEASE_BUILD" )
    ELSE( wxWidgets_USE_DEBUG )
        SET( CMAKE_CXX_FLAGS_DEBUG
    "THIS_IS_A_RELEASE_CONFIGURATION_AND_YOU_ARE_DOING_A_DEBUG_BUILD" )
        SET( CMAKE_CONFIGURATION_TYPES "Release" )
        SET (CMAKE_BUILD_TYPE  "Release" )
    ENDIF( wxWidgets_USE_DEBUG )
ENDMACRO( SET_WX_SYNCLIBRARY_FLAGS )

##################################################
# Enable precompiled headers for MSVC
##################################################

MACRO( SET_PRECOMPILED_HEADER_FLAGS )
    IF ( MSVC  )
        OPTION( WX_USE_PCH "Use precompiled headers" ON )   
        IF ( WX_USE_PCH )
            SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /GX" )
            SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3" )
        ELSE ( WX_USE_PCH )
            ADD_DEFINITIONS( -DNOPCH )
        ENDIF ( WX_USE_PCH )
    ELSE ( MSVC  )
            ADD_DEFINITIONS( -DNOPCH )	
    ENDIF ( MSVC )
ENDMACRO( SET_PRECOMPILED_HEADER_FLAGS )

