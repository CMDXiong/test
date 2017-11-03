#------------------------------------------------
# Optimask.pro -- Optimask Qt Project File.
# Project created by QtCreator 2016-10-27T22:04:11
# Optixera Modified Subfolders 2017-10-01T10:00:00
#-------------------------------------------------

QT       += core gui
qtHaveModule(printsupport): QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Optimask
TEMPLATE = app

## Include Paths
INCLUDEPATH += "../src"

## Include Resources
RC_FILE += Icon/myico.rc

## Include Header Files
HEADERS  += \
    optimask.h \
    Item/alt_key.hpp \
    Item/aqp.hpp \
    Item/boxitem.hpp \
    Item/brushwidget.hpp \
    Item/global.hpp \
    Item/itemtypes.hpp \
    Item/kuhn_munkres.hpp \
    Item/penwidget.hpp \
    Item/swatch.hpp \
    Item/textedit.hpp \
    Item/textitem.hpp \
    Item/textitemdialog.hpp \
    base/Geo.h \
    base/GeoAttribute.h \
    base/GeoDxfFile.h \
    base/GeoFile.h \
    base/GeoGDS2File.h \
    base/Referenced.h \
    base/SceneRender.h \
#    base/Thread.h \
    com/comcnst.h \
    com/commath.h \
    com/comgeom.h \
    Cell/celltreewidget.h \
#    com/compoly.h \
#    com/comdefn.h \
    File/gds2reader.h \
    File/gds2writer.h \
#    Edit/CMacroCommand.h \
    Edit/ccommandmanagerbase.h \
    Edit/ccommandrecevierbase.h \
    Edit/ccommand.h \
    Edit/cutil.h \
    Edit/CObserver.h \
    Edit/Subject.h \
    Edit/CommandManager.h \
    Edit/Singleton.h \
    Edit/CDrawWithMouseCommand.h \
    Edit/ccommandbase.h \
    Edit/factory.h \
    Edit/pastetodlg.h \
    Draw/graphicscene.h \
    Draw/gdsdocument.h \
    Draw/graphicsview.h \
    Draw/matrix.h \
    Draw/librarywidget.h \
    Draw/multiline.h \
    Alter/alterdockwidget.h \
    Alter/booleanwidget.h \
    Alter/regeneratewidget.h \
    Alter/transformwidget.hpp \
    Layer/viewportboxx.h \
    Layer/extractor.h \
    Layer/laycolorpalette.h \
    Layer/layditherpattern.h \
    Layer/laystipplepalette.h \
    Layer/layertoolbox.h \
    Layer/laynavigator.h \
    Layer/viewportbox.h \
    Script/command.h \
    Script/fuzzyquery.h \
    Script/info.h \
    Script/hint.h \
    Layer/laytreenode.h \
    Layer/laytreemodel.h \
    Layer/laytreeview.h \
    Cell/ccelldockwidget.h

## Include Sources Files
SOURCES += main.cpp \
    optimask.cpp \
    Item/alt_key.cpp \
    Item/aqp.cpp \
    Item/boxitem.cpp \
    Item/brushwidget.cpp \
    Item/global.cpp \
    Item/kuhn_munkres.cpp \
    Item/penwidget.cpp \
    Item/swatch.cpp \
    Item/textedit.cpp \
    Item/textitem.cpp \
    Item/textitemdialog.cpp \
    base/Geo.cpp \
    base/GeoAttribute.cpp \
    base/GeoDxfFile.cpp \
    base/GeoFile.cpp \
    base/GeoGDS2File.cpp \
    base/SceneRender.cpp \
#    base/Thread.cpp \
#    com/q1rim.c \
#    com/qdrim.c \
#    com/qdcvx.c \
    Cell/celltreewidget.cpp \
    File/gds2reader.cpp \
    File/gds2writer.cpp \
#    Edit/CMacroCommand.cpp \
    Edit/ccommand.cpp \
    Edit/CObserver.cpp \
    Edit/Subject.cpp \
    Edit/CommandManager.cpp \
    Edit/CDrawWithMouseCommand.cpp \
    Edit/pastetodlg.cpp \
    Draw/graphicscene.cpp \
    Draw/gdsdocument.cpp \
    Draw/graphicsview.cpp \
    Draw/matrix.cpp \
    Draw/librarywidget.cpp \
    Draw/multiline.cpp \
    Alter/alterdockwidget.cpp \
    Alter/booleanwidget.cpp \
    Alter/regeneratewidget.cpp \
    Alter/transformwidget.cpp \
    Layer/extractor.cpp \
    Layer/laycolorpalette.cpp \
    Layer/layditherpattern.cpp \
    Layer/laystipplepalette.cpp \
    Layer/layertoolbox.cpp \
    Layer/laynavigator.cpp \
    Layer/viewportbox.cpp \
    Layer/viewportboxx.cpp \
    Script/command.cpp \
    Script/fuzzyquery.cpp \
    Script/info.cpp \
    Script/hint.cpp \
    Layer/laytreenode.cpp \
    Layer/laytreemodel.cpp \
    Layer/laytreeview.cpp \
    Cell/ccelldockwidget.cpp



## Include Forms (UI)
FORMS += \
    Draw/matrix.ui \
    Draw/librarywidget.ui \
    Draw/multiline.ui

