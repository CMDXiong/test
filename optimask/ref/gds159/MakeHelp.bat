@echo off
REM -- 最初に Microsoft Visual C++ で作成した resource.h からマップ ファイルを作成します
echo // MAKEHELP.BAT generated Help Map file.  Used by GDS.HPJ. >"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // コマンド (ID_* , IDM_*) >>"hlp\gds.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // プロンプト (IDP_*) >>"hlp\gds.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // リソース (IDR_*) >>"hlp\gds.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // ダイアログ (IDD_*) >>"hlp\gds.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // フレーム コントロール (IDW_*) >>"hlp\gds.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\gds.hm"
REM -- プロジェクト GDS のヘルプを作成


echo Win32 ﾍﾙﾌﾟ ﾌｧｲﾙのﾋﾞﾙﾄﾞ中
start /wait hcw /C /E /M "hlp\gds.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\gds.hlp" goto :Error
if not exist "hlp\gds.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\gds.hlp" Debug
if exist Debug\nul copy "hlp\gds.cnt" Debug
if exist Release\nul copy "hlp\gds.hlp" Release
if exist Release\nul copy "hlp\gds.cnt" Release
echo.
goto :done

:Error
echo hlp\gds.hpj(1) : error: ヘルプファイルを作成する時に問題が発生しました。

:done
echo.
