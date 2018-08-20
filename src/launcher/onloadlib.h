#pragma once

#include <map>

#define ON_LOAD_LIB(module) static void _On##module##Loaded(uintptr_t dwModuleBase); \
							static CLoadLibCallbacks _##module##Callback(#module ".dll", _On##module##Loaded); \
							static void _On##module##Loaded(uintptr_t dwModuleBase)

#define ON_LOAD_LIB_EXTENSION(module, ext) void _On##module##Loaded(uintptr_t dwModuleBase); \
							CLoadLibCallbacks _##module##Callback(#module #ext, &_On##module##Loaded); \
							void _On##module##Loaded(uintptr_t dwModuleBase)

#define GET_LOAD_LIB_MODULE() dwModuleBase

using LoadLibCallbackfn_t = void(*)(uintptr_t);

class CLoadLibCallbacks
{	
public:
	CLoadLibCallbacks(std::string szModuleName, LoadLibCallbackfn_t pCallback)
	{
		printf("CLoadLibCallback: adding %p to %s callbacks\n", pCallback, szModuleName.c_str());
		GetCallbacks()[szModuleName] = pCallback;
	}

	~CLoadLibCallbacks() = default;

	// forces the static map to be initialized before any other static objects
	inline static std::map<std::string, LoadLibCallbackfn_t>& GetCallbacks()
	{
		static std::map<std::string, LoadLibCallbackfn_t> s_Callbacks;
		return s_Callbacks;
	}

	inline static void OnLoadLibrary(std::string szModuleName, uintptr_t dwModuleBase)
	{
		std::map<std::string, LoadLibCallbackfn_t>& callbacks = GetCallbacks();

		if (callbacks.find(szModuleName) != callbacks.end())
		{
			callbacks[szModuleName]((uintptr_t)dwModuleBase);
			callbacks.erase(szModuleName);
		}
	}
};
