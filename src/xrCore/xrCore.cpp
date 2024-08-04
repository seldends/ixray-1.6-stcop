// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "XmlParser/Expression.h"
#pragma hdrstop

#ifdef IXR_WINDOWS
#include <mmsystem.h>
#include <objbase.h>
#endif
#include "xrCore.h"
#include "discord/discord.h"

#ifdef DEBUG
#	include	<malloc.h>
#endif // DEBUG

XRCORE_API		xrCore	Core;
XRCORE_API		u32		build_id;
XRCORE_API		LPCSTR	build_date;

namespace CPU
{
	extern	void			Detect	();
};

static u32	init_counter	= 0;

char g_application_path[256];

//. extern xr_vector<shared_str>*	LogFile;

void xrCore::_initialize	(LPCSTR _ApplicationName, xrLogger::LogCallback cb, BOOL init_fs, LPCSTR fs_fname)
{
	xr_strcpy					(ApplicationName,_ApplicationName);
	if (0==init_counter) 
	{
#ifdef XRCORE_STATIC	
		_clear87	();
		_control87	( _PC_53,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		_control87	( _RC_NEAR, MCW_RC );
		_control87	( _MCW_EM,  MCW_EM );
#endif
		// Init COM so we can use CoCreateInstance
#ifdef IXR_WINDOWS
        CoInitializeEx	(nullptr, COINIT_MULTITHREADED);

		xr_strcpy			(Params,sizeof(Params),GetCommandLineA());
		_strlwr_s			(Params,sizeof(Params));

		LoadParams();
#endif

		// application path
		std::string ApplicationPath = Platform::GetBinaryFolderPath().string();
		std::string WorkingPath = std::filesystem::current_path().string();

#ifndef _EDITOR
		xr_strcpy(g_application_path, sizeof(g_application_path), ApplicationPath.c_str());
#endif

		// User/Comp Name
		std::string user_name = Platform::GetUsrName();
		std::string comp_name = Platform::GetCompName();

		xr_strcpy(UserName, sizeof(UserName), user_name.c_str());
		xr_strcpy(CompName, sizeof(CompName), comp_name.c_str());

		// Mathematics & PSI detection
		CPU::Detect			();
		
		Memory._initialize	(Core.ParamsData.test(ECoreParams::mem_debug));

		xrLogger::InitLog();
		_initialize_cpu		();

//		Debug._initialize	();

		rtc_initialize		();

		xr_FS				= new CLocatorAPI	();

		xr_EFS				= new EFS_Utils		();
		g_uiExpressionMgr = new CExpressionManager();

		g_Discord.Init();
	}

	if (init_fs)
	{
		u32 flags = 0;
		if (Core.ParamsData.test(ECoreParams::build))	
			flags |= CLocatorAPI::flBuildCopy;

		if (Core.ParamsData.test(ECoreParams::ebuild))
			flags |= CLocatorAPI::flBuildCopy|CLocatorAPI::flEBuildCopy;

#ifdef _EDITOR // for EDITORS - no cache
		flags 				&=~ CLocatorAPI::flCacheFiles;
#endif // _EDITOR
		flags |= CLocatorAPI::flScanAppRoot;

		FS._initialize		(flags,0,fs_fname);
		Msg					("'%s' build %d, %s\n","xrCore",build_id, build_date);
		EFS._initialize		();
#if defined(DEBUG) && defined(IXR_WINDOWS)
    #ifndef	_EDITOR
		Msg					("CRT heap 0x%08x",_get_heap_handle());
		Msg					("Process heap 0x%08x",GetProcessHeap());
    #endif
#endif // DEBUG
	}
	xrLogger::AddLogCallback(cb);
	init_counter++;
}

#ifndef	_EDITOR
#include "compression_ppmd_stream.h"
extern compression::ppmd::stream	*trained_model;
#endif
void xrCore::_destroy		()
{
	--init_counter;
	if (0==init_counter){
		FS._destroy			();
		EFS._destroy		();
		xr_delete			(xr_FS);
		xr_delete			(xr_EFS);

#ifndef	_EDITOR
		if (trained_model) {
			void			*buffer = trained_model->buffer();
			xr_free			(buffer);
			xr_delete		(trained_model);
		}
#endif

		Memory._destroy		();
	}
}

#ifndef XRCORE_STATIC

//. why ??? 
#ifdef IXR_WINDOWS
	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpvReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			_clear87		();
			_control87		( _PC_53,   MCW_PC );
			_control87		( _RC_CHOP, MCW_RC );
			_control87		( _RC_NEAR, MCW_RC );
			_control87		( _MCW_EM,  MCW_EM );
		}
//.		LogFile.reserve		(256);
		break;
	case DLL_THREAD_ATTACH:
		timeBeginPeriod	(1);
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif
#endif