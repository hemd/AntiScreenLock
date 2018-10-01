#pragma once
#include <windows.h>

#include "InjectImpl.h"

HRESULT __stdcall CreateInstance(IInject** ppInject)
{
	if (ppInject)
	{
		CInjectImpl* pImpl = new CInjectImpl;
		if (pImpl)
		{
			*ppInject = pImpl;
			return S_OK;
		}
	}
	return E_FAIL;
}


