# Microsoft Developer Studio Project File - Name="tnsutil_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tnsutil_dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tnsutil_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tnsutil_dll.mak" CFG="tnsutil_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tnsutil_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tnsutil_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tnsutil_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "built/win32-Release"
# PROP Intermediate_Dir "built/win32-Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TNSUTIL_DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TNS_UTIL_EXPORTS" /D TARGETNAME=\"libtnsutil.dll\" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"built/win32-Release/tnsutil.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /machine:I386 /out:"../Distribution/Release/tnsutil.dll"

!ELSEIF  "$(CFG)" == "tnsutil_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tnsutil_dll___Win32_Debug"
# PROP BASE Intermediate_Dir "tnsutil_dll___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "built/win32-Debug"
# PROP Intermediate_Dir "built/win32-Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TNS_UTIL_EXPORTS" /D TARGETNAME=\"libtnsutil.dll\" /FR /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I ".." /D "WIN32" /D "DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TNS_UTIL_EXPORTS" /D TARGETNAME=\"libtnsutil.dll\" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"built/win32-Release/tnsutil.bsc"
# ADD BSC32 /nologo /o"built/win32-Debug/tnsutil.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /machine:I386 /out:"built/win32-Release/tnsutil.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /incremental:yes /debug /machine:I386 /out:"..\Distribution\Debug\tnsutil.dll" /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "tnsutil_dll - Win32 Release"
# Name "tnsutil_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\astropak.cpp
# End Source File
# Begin Source File

SOURCE=.\daemonic.cpp
# End Source File
# Begin Source File

SOURCE=.\ftape.cpp
# End Source File
# Begin Source File

SOURCE=.\mkopts.cpp
# End Source File
# Begin Source File

SOURCE=.\porting.cpp
# End Source File
# Begin Source File

SOURCE=.\readln.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\t_floating_avg.cpp
# End Source File
# Begin Source File

SOURCE=.\t_Glimpselog.cpp
# End Source File
# Begin Source File

SOURCE=.\t_LogBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\t_ringbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\t_Set.cpp
# End Source File
# Begin Source File

SOURCE=.\t_sorter.cpp
# End Source File
# Begin Source File

SOURCE=.\t_versioninfo.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\tns_util.cpp
# End Source File
# Begin Source File

SOURCE=.\utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\astropak.h
# End Source File
# Begin Source File

SOURCE=.\copyright.h
# End Source File
# Begin Source File

SOURCE=.\daemonic.h
# End Source File
# Begin Source File

SOURCE=.\ftape.h
# End Source File
# Begin Source File

SOURCE=.\getreginfo.h
# End Source File
# Begin Source File

SOURCE=.\mkopts.h
# End Source File
# Begin Source File

SOURCE=.\netutils.h
# End Source File
# Begin Source File

SOURCE=.\porting.h
# End Source File
# Begin Source File

SOURCE=.\readln.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\t_floating_avg.h
# End Source File
# Begin Source File

SOURCE=.\t_Glimpselog.h
# End Source File
# Begin Source File

SOURCE=.\t_LogBuffer.h
# End Source File
# Begin Source File

SOURCE=.\t_peer.h
# End Source File
# Begin Source File

SOURCE=.\t_ringbuf.h
# End Source File
# Begin Source File

SOURCE=.\t_Set.h
# End Source File
# Begin Source File

SOURCE=.\t_sorter.h
# End Source File
# Begin Source File

SOURCE=.\t_versioninfo.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# Begin Source File

SOURCE=.\tns_util.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# Begin Source File

SOURCE=.\win32_export.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\tnsutil.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
