@echo off
REM -- �ŏ��� Microsoft Visual C++ �ō쐬���� resource.h ����}�b�v �t�@�C�����쐬���܂�
echo // MAKEHELP.BAT generated Help Map file.  Used by GDS.HPJ. >"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // �R�}���h (ID_* , IDM_*) >>"hlp\gds.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // �v�����v�g (IDP_*) >>"hlp\gds.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // ���\�[�X (IDR_*) >>"hlp\gds.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // �_�C�A���O (IDD_*) >>"hlp\gds.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\gds.hm"
echo. >>"hlp\gds.hm"
echo // �t���[�� �R���g���[�� (IDW_*) >>"hlp\gds.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\gds.hm"
REM -- �v���W�F�N�g GDS �̃w���v���쐬


echo Win32 ���� ̧�ق�����ޒ�
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
echo hlp\gds.hpj(1) : error: �w���v�t�@�C�����쐬���鎞�ɖ�肪�������܂����B

:done
echo.
