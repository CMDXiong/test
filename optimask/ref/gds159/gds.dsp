# Microsoft Developer Studio Project File - Name="gds" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=gds - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "gds.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "gds.mak" CFG="gds - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "gds - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "gds - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gds - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /Gi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x411 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib winmm.lib gdiplus.lib zlibstat.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libc.lib libcmt.lib"
# SUBTRACT LINK32 /profile /incremental:yes /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=if not exist Distrib mkdir Distrib	copy Res\ReadMe*.txt Distrib	copy Release\*.exe Distrib	copy Release\*.cnt Distrib	copy Release\*.hlp Distrib	if exist Debug copy Release\*.cnt Debug	if exist Debug copy Release\*.hlp Debug
# End Special Build Tool

!ELSEIF  "$(CFG)" == "gds - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x411 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib winmm.lib gdiplus.lib zlibstat.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib libcmt.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /map

!ENDIF 

# Begin Target

# Name "gds - Win32 Release"
# Name "gds - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AngleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\DefLayerSet.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\FillPatDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FontSizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\gds.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\gds.hpj

!IF  "$(CFG)" == "gds - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__GDS_H="hlp\GDSIIviewer.rtf"	"hlp\$(TargetName).hm"	
# Begin Custom Build - ﾍﾙﾌﾟ ﾌｧｲﾙを作成しています...
OutDir=.\Release
TargetName=gds
InputPath=.\hlp\gds.hpj
InputName=gds

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "gds - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__GDS_H="hlp\GDSIIviewer.rtf"	"hlp\$(TargetName).hm"	
# Begin Custom Build - ﾍﾙﾌﾟ ﾌｧｲﾙを作成しています...
OutDir=.\Debug
TargetName=gds
InputPath=.\hlp\gds.hpj
InputName=gds

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gds.odl
# End Source File
# Begin Source File

SOURCE=.\gds.rc
# End Source File
# Begin Source File

SOURCE=.\GDS2MultiDocTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\gdsDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\GDShcy.cpp
# End Source File
# Begin Source File

SOURCE=.\gdsView.cpp
# End Source File
# Begin Source File

SOURCE=.\Grid.cpp
# End Source File
# Begin Source File

SOURCE=.\GridDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HatchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LayerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MemoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MTRender.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintScaleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Scale.cpp
# End Source File
# Begin Source File

SOURCE=.\SelViewSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\SetCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\ShrinkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StrLvlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StructureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StructureInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TBComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TextDumpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeStampDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewSetCommentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ZipSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoomAreaDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AngleDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\DefLayerSet.h
# End Source File
# Begin Source File

SOURCE=.\DlgLayer.h
# End Source File
# Begin Source File

SOURCE=.\FillPatDialog.h
# End Source File
# Begin Source File

SOURCE=.\FontSizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\gds.h
# End Source File
# Begin Source File

SOURCE=.\GDS2MultiDocTemplate.h
# End Source File
# Begin Source File

SOURCE=.\gdsDoc.h
# End Source File
# Begin Source File

SOURCE=.\GDShcy.h
# End Source File
# Begin Source File

SOURCE=.\GDSRead.h
# End Source File
# Begin Source File

SOURCE=.\gdsView.h
# End Source File
# Begin Source File

SOURCE=.\Grid.h
# End Source File
# Begin Source File

SOURCE=.\GridDlg.h
# End Source File
# Begin Source File

SOURCE=.\HatchDlg.h
# End Source File
# Begin Source File

SOURCE=.\ioapi.h
# End Source File
# Begin Source File

SOURCE=.\LayerInfo.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MemoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MTRender.h
# End Source File
# Begin Source File

SOURCE=.\PrintScaleDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "gds - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - ﾍﾙﾌﾟのｲﾝｸﾙｰﾄﾞ ﾌｧｲﾙを作成中です...
TargetName=gds
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "gds - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - ﾍﾙﾌﾟのｲﾝｸﾙｰﾄﾞ ﾌｧｲﾙを作成中です...
TargetName=gds
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scale.h
# End Source File
# Begin Source File

SOURCE=.\SelViewSetting.h
# End Source File
# Begin Source File

SOURCE=.\SetCursor.h
# End Source File
# Begin Source File

SOURCE=.\ShrinkDlg.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StrLvlDlg.h
# End Source File
# Begin Source File

SOURCE=.\StructureDlg.h
# End Source File
# Begin Source File

SOURCE=.\StructureInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\TBComboBox.h
# End Source File
# Begin Source File

SOURCE=.\TextDumpDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimeStampDlg.h
# End Source File
# Begin Source File

SOURCE=.\unzip.h
# End Source File
# Begin Source File

SOURCE=.\ViewSetCommentDlg.h
# End Source File
# Begin Source File

SOURCE=.\zconf.h
# End Source File
# Begin Source File

SOURCE=.\ZipSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\zlib.h
# End Source File
# Begin Source File

SOURCE=.\ZoomAreaDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cursor_z.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor_zd.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor_zu.cur
# End Source File
# Begin Source File

SOURCE=.\res\dia.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dummy_st.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fdiag1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fdiag_da.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fill1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gds.ico
# End Source File
# Begin Source File

SOURCE=.\res\gds.rc2
# End Source File
# Begin Source File

SOURCE=.\res\gdsDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\gdsToolBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hdash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hhatch1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hwave.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hzigzag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon_dum.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_str.ico
# End Source File
# Begin Source File

SOURCE=.\res\light.bmp
# End Source File
# Begin Source File

SOURCE=.\res\light2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rdiag1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rdiag_da.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ReadMeEnglish.txt
# End Source File
# Begin Source File

SOURCE=.\res\ReadMeJapanese.txt
# End Source File
# Begin Source File

SOURCE=.\res\solid1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\vdash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\vwave.bmp
# End Source File
# Begin Source File

SOURCE=.\res\vzigzag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xdiag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xdiag1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xhatch1.bmp
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "cnt;rtf"
# Begin Source File

SOURCE=.\hlp\ButtonFill.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonFitWindow.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonFlipH.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonFlipV.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonLayer.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonMemo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonMemoDel.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonPreview.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonPrint.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonRedraw.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonRotate.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonRuler.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonRulerDel.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonStructure.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonText.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonZoomIn.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonZoomMode.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ButtonZoomOut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CursorZoom.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CursorZoomIn.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CursorZoomOut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FillLimit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\gds.cnt
# End Source File
# Begin Source File

SOURCE=.\hlp\gds.cnt

!IF  "$(CFG)" == "gds - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - 目次ﾌｧｲﾙを作成中です...
OutDir=.\Release
InputPath=.\hlp\gds.cnt
InputName=gds

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "gds - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - 目次ﾌｧｲﾙを作成中です...
OutDir=.\Debug
InputPath=.\hlp\gds.cnt
InputName=gds

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\GDS.GID
# End Source File
# Begin Source File

SOURCE=.\hlp\GDS.HLP
# End Source File
# Begin Source File

SOURCE=.\hlp\gds.hm
# End Source File
# Begin Source File

SOURCE=.\hlp\GDSIIviewer.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\GridMeasure.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\GridMeasure2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Launch.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\LayerSetup.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Memo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\MoveToXY.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Open.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecurseLevel.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Rotate.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Ruler.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\SetupPrint.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Shrink.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Structure.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\TextSize.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\WriteEmf.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\gds.reg
# End Source File
# End Target
# End Project
# Section gds : {72ADFD78-2C39-11D0-9903-00A0C91BC942}
# 	1:10:IDB_SPLASH:102
# 	2:21:SplashScreenInsertKey:4.0
# End Section
