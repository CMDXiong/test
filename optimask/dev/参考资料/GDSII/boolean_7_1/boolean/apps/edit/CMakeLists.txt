PROJECT( edit )

INCLUDE_DIRECTORIES( 
    ${kbool_SOURCE_DIR}/include 
    ${edit_SOURCE_DIR}/include  
    ${edit_SOURCE_DIR}/include/list 
    ${edit_SOURCE_DIR}/include/msw 
)

SET( header_files
    about.h         debugdlg.h      gbrhier.h       group.h         mainwin.h       referenc.h      units.h
    alias.h         distdlg.h       gdsapp.h        groupdlg.h      misc.h          scrndrv.h       unitsdlg.h
    arc.h           doubledc.h     segment.h       
    bbox.h          drivout.h       gdsdef.h        identdlg.h      newfiledlg.h    settingsdlg.h   valuesvc.h
    boundary.h      element.h       gdsdrin.h       insdlg.h        orderdlg.h      shape.h         wait.h
    chandbas.h      elemlst.h       gdsdrout.h      keydrin.h       patdlg.h        b_statusb.h       windrv.h
    circleditdlg.h  emaskin.h       gdserr.h        keydrout.h      path.h          strucdlg.h      winmess.h
    cnchier.h       emaskout.h      gdsii.h         layerinf.h      point.h         structur.h      winstatb.h
    command.h       eventids.h      gdsio.h         layprops.h      polyeditdlg.h   text.h          wxmatrix.h
    copylist.h      execdlg.h       gdsmes.h        linepars.h      polyline.h      textinsdlg.h    wxsvg.h
    cparser.h       exlout.h        genhier.h       liner.h         prcspars.h      tool.h          wxwin.h
    cursordlg.h     facdlg.h        printer.h       toolcont.h
    dcclbuf.h       flashin.h         lpdsizes.h      propedit.h      trans.h
    ddehand.h       flashout.h      griddlg.h       lsetup.h        proper.h        transdlg.h
    list/b_dl_itr.h
    list/b_lnk_itr.h
    prop.h proplist.h propform.h
)
        
SET( src_files
    src/parser/cparser.cpp
    src/gdssourc/elemlst.cpp
    src/gdssourc/liner.cpp
    src/intrface/b_statusb.cpp
    src/intrface/gdsapp.cpp
    src/props/propform.cpp
    src/misc/chandbas.cpp
    src/props/prop.cpp
    src/props/proplist.cpp
    src/intrface/mainwin.cpp
    src/tools/tool.cpp
    src/tools/toolcont.cpp
    src/misc/units.cpp
    src/misc/wxmatrix.cpp
    src/parser/linepars.cpp
    src/parser/prcspars.cpp
    src/misc/ddehand.cpp
    src/misc/dummy.cpp
    src/misc/gdserr.cpp
    src/misc/gdsmes.cpp
    src/misc/group.cpp
    src/misc/layerinf.cpp
    src/misc/lsetup.cpp
    src/misc/misc.cpp
    src/intrface/winmess.cpp
    src/intrface/winstatb.cpp
    src/intrface/wxwin.cpp
    src/list/instance.cpp
    src/misc/alias.cpp
    src/misc/command.cpp
    src/intrface/settingsdlg.cpp
    src/intrface/strucdlg.cpp
    src/intrface/textinsdlg.cpp
    src/intrface/transdlg.cpp
    src/intrface/unitsdlg.cpp
    src/intrface/wait.cpp
    src/intrface/layprops.cpp
    src/intrface/newfiledlg.cpp
    src/intrface/orderdlg.cpp
    src/intrface/patdlg.cpp
    src/intrface/propedit.cpp
    src/intrface/execdlg.cpp
    src/intrface/facdlg.cpp
    src/intrface/griddlg.cpp
    src/intrface/groupdlg.cpp
    src/intrface/identdlg.cpp
    src/intrface/insdlg.cpp
    src/gdssourc/text.cpp
    src/intrface/about.cpp
    src/intrface/circleditdlg.cpp
    src/intrface/copylist.cpp
    src/intrface/cursordlg.cpp
    src/intrface/debugdlg.cpp
    src/intrface/distdlg.cpp
    src/gdssourc/path.cpp
    src/gdssourc/point.cpp
    src/gdssourc/polyline.cpp
    src/gdssourc/proper.cpp
    src/gdssourc/referenc.cpp
    src/gdssourc/segment.cpp
    src/gdssourc/shape.cpp
    src/gdssourc/structur.cpp
    src/drivers/wxsvg.cpp
    src/gdssourc/arc.cpp
    src/gdssourc/bbox.cpp
    src/gdssourc/boundary.cpp
    src/gdssourc/element.cpp
    src/gdssourc/gdsii.cpp
    src/gdssourc/gdsio.cpp
    src/drivers/keydrin.cpp
    src/drivers/keydrout.cpp
    src/drivers/printer.cpp
    src/drivers/scrndrv.cpp
    src/drivers/trans.cpp
    src/drivers/windrv.cpp
    src/drivers/emaskin.cpp
    src/drivers/emaskout.cpp
    src/drivers/exlout.cpp
    src/drivers/flashin.cpp
    src/drivers/flashout.cpp
    src/drivers/gbrhier.cpp
    src/drivers/gdsdrin.cpp
    src/drivers/gdsdrout.cpp
    src/drivers/cnchier.cpp
    src/drivers/dcclbuf.cpp
    src/drivers/doubledc.cpp
    src/drivers/drivout.cpp
)
        
        
FOREACH(file ${header_files})
    SET( headerwithpath  ${headerwithpath} ${edit_SOURCE_DIR}/include/${file}  )
ENDFOREACH(file)   

IF(WIN32)
    IF(NOT BORLAND)
		ADD_DEFINITIONS( -D_MSWVC_  )
    ENDIF(NOT BORLAND)  
	ADD_EXECUTABLE( edit WIN32 ${src_files} win32/boolean.rc ${headerwithpath} ) 
ELSE(WIN32)
    ADD_DEFINITIONS( -D__UNIX__  )
	ADD_EXECUTABLE( edit ${src_files} ${headerwithpath} ) 
ENDIF(WIN32)	

TARGET_LINK_LIBRARIES( edit  ${WXART2D_LIBRARIES} kbool )


