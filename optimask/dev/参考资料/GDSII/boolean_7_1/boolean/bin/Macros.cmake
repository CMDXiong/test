# Macro to provide an option only if a set of other variables are ON.
# Example invocation:
#
#  WXART2D_DEPENDENT_OPTION(USE_FOO "Use Foo" ON "USE_BAR;USE_ZOT" OFF)
#
# If both USE_BAR and USE_ZOT are true, this provides an option called
# USE_FOO that defaults to ON.  Otherwise, it sets USE_FOO to OFF.  If
# the status of USE_BAR or USE_ZOT ever changes, any value for the
# USE_FOO option is saved so that when the option is re-enabled it
# retains its old value.
#
MACRO(WXART2D_DEPENDENT_OPTION option doc default depends force)
  SET(${option}_AVAILABLE 1)

  # Test for each required variable.
  FOREACH(d ${depends})
    IF(NOT ${d})
      SET(${option}_AVAILABLE 0)
    ENDIF(NOT ${d})
  ENDFOREACH(d)
  IF(${option}_AVAILABLE)
    # The option is available.
    OPTION(${option} "${doc}" "${default}")

    # Show the cache value to the user.
    SET(${option} "${${option}}" CACHE BOOL "${doc}" FORCE)
  ELSE(${option}_AVAILABLE)
    # Force the option value to be that specified.
    SET(${option} ${force})
  ENDIF(${option}_AVAILABLE)
ENDMACRO(WXART2D_DEPENDENT_OPTION)

MACRO( A2D_INCLUDE_DIRECTORIES dirs )
    INCLUDE_DIRECTORIES( ${dirs} )
    FOREACH(d ${dirs})
        SET( WXART2D_INCLUDE_DIRS ${WXART2D_INCLUDE_DIRS} ${d})
    ENDFOREACH(d ${dirs})        
ENDMACRO( A2D_INCLUDE_DIRECTORIES )

MACRO( A2D_LINK_DIRECTORIES dirs )
    LINK_DIRECTORIES( ${dirs} )
    FOREACH(d ${dirs})
        SET( WXART2D_LIBRARY_DIRS ${WXART2D_LIBRARY_DIRS} ${d})
    ENDFOREACH(d ${dirs})        
ENDMACRO( A2D_LINK_DIRECTORIES )

MACRO( A2D_ADD_DEFINITIONS defines )
    FOREACH(d ${defines})
        ADD_DEFINITIONS( ${d} )
    ENDFOREACH(d ${defines})
    SET( WXART2D_FLAGS ${WXART2D_FLAGS} ${defines})
ENDMACRO( A2D_ADD_DEFINITIONS )

MACRO( A2D_THIRDPARTY_LIBRARIES linklibs )

    FOREACH(d ${linklibs})
        SET( LINK TRUE )
        FOREACH(lib ${WXART2D_THIRDPARTY_LIBRARIES})
            IF (${d} STREQUAL "${lib}" )
                SET( LINK FALSE )
            ENDIF (${d} STREQUAL "${lib}" )
        ENDFOREACH(lib ${WXART2D_THIRDPARTY_LIBRARIES})
        
        IF (  LINK MATCHES TRUE )   
                SET( WXART2D_THIRDPARTY_LIBRARIES ${WXART2D_THIRDPARTY_LIBRARIES} ${d} )   
        ENDIF (  LINK MATCHES TRUE )   
    ENDFOREACH(d ${linklibs})
                
ENDMACRO( A2D_THIRDPARTY_LIBRARIES )

MACRO( A2D_LINK_LIBRARIES linklibs )

    FOREACH(d ${linklibs})
        SET( LINK TRUE )
        FOREACH(lib ${WXART2D_LIBRARIES})
            IF (${d} STREQUAL "${lib}" )
                SET( LINK FALSE )
            ENDIF (${d} STREQUAL "${lib}" )
        ENDFOREACH(lib ${WXART2D_LIBRARIES})
        
        IF (  LINK MATCHES TRUE )   
            SET( WXART2D_LIBRARIES ${WXART2D_LIBRARIES} ${d} )   
        ENDIF (  LINK MATCHES TRUE )   
    ENDFOREACH(d ${linklibs})
                
ENDMACRO( A2D_LINK_LIBRARIES )

MACRO( A2D_LINK_LIBRARIES_WXLIKE linklibs )

    IF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
        SET( OUTPUTLIBPREFIX wxart2d_${wxWidgets_PORT}${wxWidgets_UNV}${wxWidgets_PFVERSION}${wxWidgets_UCD}${wxWidgets_DBG}_ )
        SET( OUTPUTLIBPOSTFIX "" )
    ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "WIN32_STYLE_FIND" )
    IF( ${wxWidgets_FIND_STYLE} STREQUAL "UNIX_STYLE_FIND" )
        # how will output libraries be  prefixed
        SET( OUTPUTLIBPREFIX wxart2d_${wxWidgets_PORT}${wxWidgets_UNV}${wxWidgets_UCD}${wxWidgets_DBG}_ )
        SET( OUTPUTLIBPOSTFIX "-${wxWidgets_PFVERSION}" )      
    ENDIF( ${wxWidgets_FIND_STYLE} STREQUAL "UNIX_STYLE_FIND" )

    FOREACH(d ${linklibs})
        SET( LINK TRUE )
        FOREACH( lib ${WXART2D_LIBRARIES_WXLIKE})
            IF (${d} STREQUAL "${lib}" )
                SET( LINK FALSE )
            ENDIF (${d} STREQUAL "${lib}" )
        ENDFOREACH( lib ${WXART2D_LIBRARIES_WXLIKE})
        
        IF (  LINK MATCHES TRUE )   
            SET( WXART2D_LIBRARIES_WXLIKE ${WXART2D_LIBRARIES_WXLIKE} ${d} )
        ENDIF (  LINK MATCHES TRUE )   
    ENDFOREACH(d ${linklibs})
                
ENDMACRO( A2D_LINK_LIBRARIES_WXLIKE )

# This macro is called by modules that use precompiled headers. It sets
# a2d_PCH_SOURCE_FILE to the source file used as precompiled header source
# by MSVC, or empty if target is not MSVC.
MACRO( A2D_PCH )
    IF ( MSVC AND WXART2D_USE_PCH )
        # disable precompiled headers for vs.net, to be solved.
        IF (CMAKE_GENERATOR MATCHES "NET")
            #SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} /Yu"${wxart2d_SOURCE_DIR}\\modules\\general\\include\\a2dprec.h" )
            #SET( a2d_PCH_SOURCE_FILE
            #    ${wxart2d_SOURCE_DIR}/modules/general/src/dummy.cpp
            #)
            #SET_SOURCE_FILES_PROPERTIES( ${wxart2d_SOURCE_DIR}/modules/general/src/dummy.cpp PROPERTIES COMPILE_FLAGS "/Yc\"general/include/a2dprec.h\"")
        ELSE (CMAKE_GENERATOR MATCHES "NET")
            ADD_DEFINITIONS( /Yu"general/include/a2dprec.h" )
            SET( a2d_PCH_SOURCE_FILE
                ${wxart2d_SOURCE_DIR}/modules/general/src/dummy.cpp
            )
            SET_SOURCE_FILES_PROPERTIES( ${wxart2d_SOURCE_DIR}/modules/general/src/dummy.cpp PROPERTIES COMPILE_FLAGS "/Yc\"general/include/a2dprec.h\"")
        ENDIF (CMAKE_GENERATOR MATCHES "NET")
    ELSE ( MSVC AND WXART2D_USE_PCH  )
        SET( a2d_PCH_SOURCE_FILE ""
        )
    ENDIF ( MSVC AND WXART2D_USE_PCH  )
ENDMACRO( A2D_PCH )

# *.rc files are not automatically compiled by cmake on mingw/cygwin
MACRO( COMPILE_RC SRCFILE DSTFILE )
    IF( CYGWIN OR MINGW )
        ADD_CUSTOM_COMMAND(
            OUTPUT ${DSTFILE}
            COMMAND windres
            ARGS -i ${SRCFILE} -o ${DSTFILE} --include-dir ${wxWidgets_ROOT_DIR}/include
        )
    ENDIF( CYGWIN OR MINGW )
ENDMACRO( COMPILE_RC )

# copy files in FILES from SRCDIR to DESTDIR, unless they are those same
MACRO( COPYFILES SRCDIR DESTDIR FILES )
    IF("${SRCDIR}" MATCHES "^${DESTDIR}$" )
    ELSE("${SRCDIR}" MATCHES "^${DESTDIR}$" )
        
        FOREACH(file ${FILES})
            SET(src "${SRCDIR}/${file}")
            SET(tgt "${DESTDIR}/${file}")
            CONFIGURE_FILE(${src} ${tgt} COPYONLY )
        ENDFOREACH(file)  
      
    ENDIF("${SRCDIR}" MATCHES "^${DESTDIR}$" )
ENDMACRO( COPYFILES SRCDIR DESTDIR FILES )

# copy files from SRCDIR to DESTDIR, unless they are those same
MACRO( COPYFILESALL SRCDIR DESTDIR  FILTER )
    IF("${SRCDIR}" MATCHES "^${DESTDIR}$" )
    ELSE("${SRCDIR}" MATCHES "^${DESTDIR}$" )
        
        FILE(GLOB FILES "${SRCDIR}/${FILTER}" )
                                
        FOREACH(file ${FILES})        
            GET_FILENAME_COMPONENT(filename ${file} NAME)
            IF ( "${SRCDIR}/${filename}" MATCHES "CVS" )
            ELSE( "${SRCDIR}/${filename}" MATCHES "CVS" )
            SET(src "${SRCDIR}/${filename}")
            SET(tgt "${DESTDIR}/${filename}")
            CONFIGURE_FILE(${src} ${tgt} COPYONLY)
            ENDIF ( "${SRCDIR}/${filename}" MATCHES "CVS" )
        ENDFOREACH(file)  
      
    ENDIF("${SRCDIR}" MATCHES "^${DESTDIR}$" )
ENDMACRO( COPYFILESALL SRCDIR DESTDIR  )


