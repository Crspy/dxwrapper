/**
 * Copyright (C) 2015 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/d3d8to9#license
 *
 * Updated 2017 by Elisha Riedlinger
 */

//********** Begin Edit *************
#include "d3d8to9.hpp"
typedef LPDIRECT3D9(WINAPI *PFN_Direct3DCreate9)(UINT SDKVersion);
namespace D3d8to9
{
	FARPROC Direct3DCreate9;
}
//********** End Edit ***************

extern "C" Direct3D8 *WINAPI _Direct3DCreate8(UINT SDKVersion)
{
	//********** Begin Edit *************
	LOG << "Enabling D3d8to9 function (" << SDKVersion << ")";

	// Declare Direct3DCreate9
	static PFN_Direct3DCreate9 Direct3DCreate9 = reinterpret_cast<PFN_Direct3DCreate9>(D3d8to9::Direct3DCreate9);
	if (!Direct3DCreate9)
	{
		LOG << "Failed to get 'Direct3DCreate9' ProcAddress of d3d9.dll!";
		return nullptr;
	}
	//********** End Edit ***************

	IDirect3D9 *const d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (d3d == nullptr)
	{
		return nullptr;
	}

	return new Direct3D8(d3d);
}
