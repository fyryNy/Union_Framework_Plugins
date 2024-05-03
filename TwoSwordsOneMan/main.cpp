#include <vector>
#include <map>

#pragma warning(disable: 4005)

#include <Union/Hook.h>
#include <ZenGin/zGothicAPI.h>
#include <Union/Dll.h>

/*typedef void* (LPUniversalLoadLibrary)(const char* name, bool asResource);
auto module = LoadLibraryA("union-universal-loader.dll");
auto pUniversalLoadLibrary = (LPUniversalLoadLibrary*)GetProcAddress(module, (char*)1); // ordinal = 1
void* dllPtr1 = pUniversalLoadLibrary("JOYSHOCKLIBRARY.dll", false);
void* dllPtr2 = pUniversalLoadLibrary("ZGAMEPAD.dll", false);*/

enum class LoadReason : size_t
{
		NONE,
		NEW_GAME,
		LOAD_SAVE,
		WORLD_CHANGE
};

struct TGameSaveLoadInfo
{
		int Slot;
		LoadReason Reason;

		static ::Union::StringANSI GetSaveSlotName(int id)
		{
				if (id > 0)
				{
						return "savegame" + id;
				}
				if (id == 0)
				{
						return "quicksave";
				}
				return "current";
		}
} slInfo;

#ifdef __G1
#define GOTHIC_NAMESPACE Gothic_I_Classic
#define ENGINE Engine_G1
HOOKSPACE(Gothic_I_Classic, GetGameVersion() == ENGINE);
#include "Headers.h"
#include "Plugin.hpp"
#include "GameEvents.hpp"
#endif

#ifdef __G1A
#define GOTHIC_NAMESPACE Gothic_I_Addon
#define ENGINE Engine_G1A
HOOKSPACE(Gothic_I_Addon, GetGameVersion() == ENGINE);
#include "Headers.h"
#include "Plugin.hpp"
#include "GameEvents.hpp"
#endif

#ifdef __G2
#define GOTHIC_NAMESPACE Gothic_II_Classic
#define ENGINE Engine_G2
HOOKSPACE(Gothic_II_Classic, GetGameVersion() == ENGINE);
#include "Headers.h"
#include "Plugin.hpp"
#include "GameEvents.hpp"
#endif

#ifdef __G2A
#define GOTHIC_NAMESPACE Gothic_II_Addon
#define ENGINE Engine_G2A
HOOKSPACE(Gothic_II_Addon, GetGameVersion() == ENGINE);
#include "Headers.h"
#include "Plugin.hpp"
#include "GameEvents.hpp"
#endif

#undef GOTHIC_NAMESPACE
#undef ENGINE

HOOKSPACE(Global, true);