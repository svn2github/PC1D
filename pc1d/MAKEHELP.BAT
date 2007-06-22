REM -- First make map file from App Studio generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by pc1d.HPJ. >hlp\pc1d.hm
echo. >>hlp\pc1d.hm
echo // Commands (ID_* and IDM_*) >>hlp\pc1d.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>hlp\pc1d.hm
echo. >>hlp\pc1d.hm
echo // Prompts (IDP_*) >>hlp\pc1d.hm
makehm IDP_,HIDP_,0x30000 resource.h >>hlp\pc1d.hm
echo. >>hlp\pc1d.hm
echo // Resources (IDR_*) >>hlp\pc1d.hm
makehm IDR_,HIDR_,0x20000 resource.h >>hlp\pc1d.hm
echo. >>hlp\pc1d.hm
echo // Dialogs (IDD_*) >>hlp\pc1d.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\pc1d.hm
echo. >>hlp\pc1d.hm
echo // Frame Controls (IDW_*) >>hlp\pc1d.hm
makehm IDW_,HIDW_,0x50000 resource.h >>hlp\pc1d.hm
REM -- Make help for Project pc1d
call hc31 pc1d.hpj
echo.
