#include <Windows.h>
#include <dx11hook.hpp>
#include <uesdk_extension.hpp>
#include <ui_extensions.hpp>
#include <iostream>

struct FilterClassCastFlag : ui::FilterItem<int32_t>
{
    SDK::EClassCastFlags internalValue;
    FilterClassCastFlag(SDK::EClassCastFlags value = SDK::EClassCastFlags::CASTCLASS_None) : internalValue(value) {}
    

    const char* GetName() override
    {
        return uesdk::ClassCastFlag2Str(internalValue).c_str();
    }

    ImGuiID UniqueID() override
    {
        uint64_t value = internalValue;
        value ^= value >> 16;
        value *= 0x85ebca6b; // A prime number for mixing
        value ^= value >> 13;
        value *= 0xc2b2ae35; // Another prime number for mixing
        value ^= value >> 16;
        return value;
    }

};

void SDKTest()
{
    if (ImGui::Begin("Filter Test"))
    {
        static ui::DualListFilterBox<FilterClassCastFlag> FilterBox;
        FilterBox.Draw();
        ImGui::End();
    }
}

DWORD WINAPI V2()
{
    FILE *fp;
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	IDXGISwapChain *swp = dx11::CreateSwapChain();
	if (swp == nullptr)
		std::cout << "WARNING : " << " no swap chain" << std::endl;
	std::cout << std::hex << swp << std::endl;
	// add sdk
	dx11::m_drawList.push_back(SDKTest);
	dx11::HookSwapChain(swp, dx11::HookedPresent);
	std::cin.ignore();
	return 0;
}

#ifdef V1_VERSION

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL, // handle to DLL module
	DWORD fdwReason,	// reason for calling function
	LPVOID lpvReserved) // reserved
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)V2, NULL, NULL, NULL);
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}

#endif