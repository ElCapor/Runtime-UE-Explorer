#include <Windows.h>
#include <detours.h>
#include <HookManager.hpp>
#include <iostream>
#include <dx11hook.hpp>
#include <imgui.h>
#include <UESDK.hpp>
#include <imgui_stdlib.h>
#include <format>

enum sdk_state : uint8_t
{
	NOT_INIT,
	WAITING,
	INIT
};

sdk_state &SDKState()
{
	static sdk_state state = sdk_state::NOT_INIT;
	return state;
}

class FStructProperty : public SDK::FProperty
{
	public:
	SDK::UStruct* Struct;
};
std::string EPropertyFlag2Str(SDK::EPropertyFlags Flag)
{
	switch (Flag)
	{
		case SDK::CPF_None:
			return "None";
		case SDK::CPF_Edit:
			return "Edit";
		case SDK::CPF_ConstParm:
			return "ConstParm";
		case SDK::CPF_BlueprintVisible:
			return "BlueprintVisible";
		case SDK::CPF_ExportObject:
			return "ExportObject";
		case SDK::CPF_BlueprintReadOnly:
			return "BlueprintReadOnly";
		case SDK::CPF_Net:
			return "Net";
		case SDK::CPF_EditFixedSize:
			return "EditFixedSize";
		case SDK::CPF_Parm:
			return "Parm";
		case SDK::CPF_OutParm:
			return "OutParm";
		case SDK::CPF_ZeroConstructor:
			return "ZeroConstructor";
		case SDK::CPF_ReturnParm:
			return "ReturnParm";
		case SDK::CPF_DisableEditOnTemplate:
			return "DisableEditOnTemplate";
		case SDK::CPF_NonNullable:
			return "NonNullable";
		case SDK::CPF_Transient:
			return "Transient";
		case SDK::CPF_Config:
			return "Config";
		case SDK::CPF_RequiredParm:
			return "RequiredParm";
		case SDK::CPF_DisableEditOnInstance:
			return "DisableEditOnInstance";
		case SDK::CPF_EditConst:
			return "EditConst";
		case SDK::CPF_GlobalConfig:
			return "GlobalConfig";
		case SDK::CPF_InstancedReference:
			return "InstancedReference";
		case SDK::CPF_DuplicateTransient:
			return "DuplicateTransient";
		case SDK::CPF_SaveGame:
			return "SaveGame";
		case SDK::CPF_NoClear:
			return "NoClear";
		case SDK::CPF_ReferenceParm:
			return "ReferenceParm";
		case SDK::CPF_BlueprintAssignable:
			return "BlueprintAssignable";
		case SDK::CPF_Deprecated:
			return "Deprecated";
		case SDK::CPF_IsPlainOldData:
			return "IsPlainOldData";
		case SDK::CPF_RepSkip:
			return "RepSkip";
		case SDK::CPF_RepNotify:
			return "RepNotify";
		case SDK::CPF_Interp:
			return "Interp";
		case SDK::CPF_NonTransactional:
			return "NonTransactional";
		case SDK::CPF_EditorOnly:
			return "EditorOnly";
		case SDK::CPF_NoDestructor:
			return "NoDestructor";
		case SDK::CPF_AutoWeak:
			return "AutoWeak";
		case SDK::CPF_ContainsInstancedReference:
			return "ContainsInstancedReference";
		case SDK::CPF_AssetRegistrySearchable:
			return "AssetRegistrySearchable";
		case SDK::CPF_SimpleDisplay:
			return "SimpleDisplay";
		case SDK::CPF_AdvancedDisplay:
			return "AdvancedDisplay";
		case SDK::CPF_Protected:
			return "Protected";
		case SDK::CPF_BlueprintCallable:
			return "BlueprintCallable";
		case SDK::CPF_BlueprintAuthorityOnly:
			return "BlueprintAuthorityOnly";
		case SDK::CPF_TextExportTransient:
			return "TextExportTransient";
		case SDK::CPF_NonPIEDuplicateTransient:
			return "NonPIEDuplicateTransient";
		case SDK::CPF_ExposeOnSpawn:
			return "ExposeOnSpawn";
		case SDK::CPF_PersistentInstance:
			return "PersistentInstance";
		case SDK::CPF_UObjectWrapper:
			return "UObjectWrapper";
		case SDK::CPF_HasGetValueTypeHash:
			return "HasGetValueTypeHash";
		case SDK::CPF_NativeAccessSpecifierPublic:
			return "NativeAccessSpecifierPublic";
		case SDK::CPF_NativeAccessSpecifierProtected:
			return "NativeAccessSpecifierProtected";
		case SDK::CPF_NativeAccessSpecifierPrivate:
			return "NativeAccessSpecifierPrivate";
		case SDK::CPF_SkipSerialization:
			return "SkipSerialization";
		case SDK::CPF_TObjectPtr:
			return "TObjectPtr";
		case SDK::CPF_ExperimentalOverridableLogic:
			return "ExperimentalOverridableLogic";
		case SDK::CPF_ExperimentalAlwaysOverriden:
			return "ExperimentalAlwaysOverriden";
		default:
			return "Unknown";
	}
}

std::string ObjectSearchFiler = "";

std::vector<SDK::EClassCastFlags> GetAllAvailableCastFlags()
{
	std::vector<SDK::EClassCastFlags> CastFlags;
	for (uint64_t flag = 1; flag <= SDK::CASTCLASS_FFieldPathProperty; flag <<= 1)
	{
		CastFlags.push_back(static_cast<SDK::EClassCastFlags>(flag));
	}
	return CastFlags;
}

std::vector<SDK::EClassCastFlags> GetCastFlags(SDK::UObject *Obj)
{
	std::vector<SDK::EClassCastFlags> CastFlags;

	for (uint64_t flag = 1; flag <= SDK::CASTCLASS_FFieldPathProperty; flag <<= 1)
	{
		if (Obj->HasTypeFlag(static_cast<SDK::EClassCastFlags>(flag)))
		{
			CastFlags.push_back(static_cast<SDK::EClassCastFlags>(flag));
		}
	}

	return CastFlags;
}

std::vector<SDK::EClassCastFlags> GetCastFlags(SDK::FField* field)
{
	std::vector<SDK::EClassCastFlags> CastFlags;
	for (uint64_t flag = 1; flag <= SDK::CASTCLASS_FFieldPathProperty; flag <<= 1)
	{
		if (field->HasTypeFlag(static_cast<SDK::EClassCastFlags>(flag)))
		{
			CastFlags.push_back(static_cast<SDK::EClassCastFlags>(flag));
		}
	}

	return CastFlags;
}

std::vector<SDK::EPropertyFlags> GetPropertyFlags(SDK::FProperty* property)
{
	std::vector<SDK::EPropertyFlags> PropertyFlags;
	for (uint64_t flag = 1; flag <= SDK::CPF_TObjectPtr; flag <<= 1)
	{
		if (property->HasPropertyFlag(static_cast<SDK::EPropertyFlags>(flag)))
		{
			PropertyFlags.push_back(static_cast<SDK::EPropertyFlags>(flag));
		}
	}
	return PropertyFlags;
}

std::string BuildPropertyFlagsStr(std::vector<SDK::EPropertyFlags> PropertyFlags)
{
	std::string PropertyFlagsStr = "";
	for (auto &PropertyFlag : PropertyFlags)
	{
		PropertyFlagsStr += EPropertyFlag2Str(PropertyFlag) + " | ";
	}
	return PropertyFlagsStr;
}

std::string CastFlag2Str(SDK::EClassCastFlags Flag)
{
	switch (Flag)
	{
	case SDK::CASTCLASS_None:
		return "None";
	case SDK::CASTCLASS_UField:
		return "UField";
	case SDK::CASTCLASS_FInt8Property:
		return "FInt8Property";
	case SDK::CASTCLASS_UEnum:
		return "UEnum";
	case SDK::CASTCLASS_UStruct:
		return "UStruct";
	case SDK::CASTCLASS_UScriptStruct:
		return "UScriptStruct";
	case SDK::CASTCLASS_UClass:
		return "UClass";
	case SDK::CASTCLASS_FByteProperty:
		return "FByteProperty";
	case SDK::CASTCLASS_FIntProperty:
		return "FIntProperty";
	case SDK::CASTCLASS_FFloatProperty:
		return "FFloatProperty";
	case SDK::CASTCLASS_FUInt64Property:
		return "FUInt64Property";
	case SDK::CASTCLASS_FClassProperty:
		return "FClassProperty";
	case SDK::CASTCLASS_FUInt32Property:
		return "FUInt32Property";
	case SDK::CASTCLASS_FInterfaceProperty:
		return "FInterfaceProperty";
	case SDK::CASTCLASS_FNameProperty:
		return "FNameProperty";
	case SDK::CASTCLASS_FStrProperty:
		return "FStrProperty";
	case SDK::CASTCLASS_FProperty:
		return "FProperty";
	case SDK::CASTCLASS_FObjectProperty:
		return "FObjectProperty";
	case SDK::CASTCLASS_FBoolProperty:
		return "FBoolProperty";
	case SDK::CASTCLASS_FUInt16Property:
		return "FUInt16Property";
	case SDK::CASTCLASS_UFunction:
		return "UFunction";
	case SDK::CASTCLASS_FStructProperty:
		return "FStructProperty";
	case SDK::CASTCLASS_FArrayProperty:
		return "FArrayProperty";
	case SDK::CASTCLASS_FInt64Property:
		return "FInt64Property";
	case SDK::CASTCLASS_FDelegateProperty:
		return "FDelegateProperty";
	case SDK::CASTCLASS_FNumericProperty:
		return "FNumericProperty";
	case SDK::CASTCLASS_FMulticastDelegateProperty:
		return "FMulticastDelegateProperty";
	case SDK::CASTCLASS_FObjectPropertyBase:
		return "FObjectPropertyBase";
	case SDK::CASTCLASS_FWeakObjectProperty:
		return "FWeakObjectProperty";
	case SDK::CASTCLASS_FLazyObjectProperty:
		return "FLazyObjectProperty";
	case SDK::CASTCLASS_FSoftObjectProperty:
		return "FSoftObjectProperty";
	case SDK::CASTCLASS_FTextProperty:
		return "FTextProperty";
	case SDK::CASTCLASS_FInt16Property:
		return "FInt16Property";
	case SDK::CASTCLASS_FDoubleProperty:
		return "FDoubleProperty";
	case SDK::CASTCLASS_FSoftClassProperty:
		return "FSoftClassProperty";
	case SDK::CASTCLASS_UPackage:
		return "UPackage";
	case SDK::CASTCLASS_ULevel:
		return "ULevel";
	case SDK::CASTCLASS_AActor:
		return "AActor";
	case SDK::CASTCLASS_APlayerController:
		return "APlayerController";
	case SDK::CASTCLASS_APawn:
		return "APawn";
	case SDK::CASTCLASS_USceneComponent:
		return "USceneComponent";
	case SDK::CASTCLASS_UPrimitiveComponent:
		return "UPrimitiveComponent";
	case SDK::CASTCLASS_USkinnedMeshComponent:
		return "USkinnedMeshComponent";
	case SDK::CASTCLASS_USkeletalMeshComponent:
		return "USkeletalMeshComponent";
	case SDK::CASTCLASS_UBlueprint:
		return "UBlueprint";
	case SDK::CASTCLASS_UDelegateFunction:
		return "UDelegateFunction";
	case SDK::CASTCLASS_UStaticMeshComponent:
		return "UStaticMeshComponent";
	case SDK::CASTCLASS_FMapProperty:
		return "FMapProperty";
	case SDK::CASTCLASS_FSetProperty:
		return "FSetProperty";
	case SDK::CASTCLASS_FEnumProperty:
		return "FEnumProperty";
	case SDK::CASTCLASS_USparseDelegateFunction:
		return "USparseDelegateFunction";
	case SDK::CASTCLASS_FMulticastInlineDelegateProperty:
		return "FMulticastInlineDelegateProperty";
	case SDK::CASTCLASS_FMulticastSparseDelegateProperty:
		return "FMulticastSparseDelegateProperty";
	case SDK::CASTCLASS_FFieldPathProperty:
		return "FFieldPathProperty";
	default:
		return "Unknown";
	}
}

std::string BuildCastFlagsStr(std::vector<SDK::EClassCastFlags> CastFlags)
{
	std::string CastFlagsStr = "";
	for (auto &CastFlag : CastFlags)
	{
		CastFlagsStr += CastFlag2Str(CastFlag) + " | ";
	}
	return CastFlagsStr;
}

bool ObjectHasAnyCastFlag(SDK::UObject *Obj, std::vector<SDK::EClassCastFlags> CastFlags, bool bHardMatch = false)
{
	if (!bHardMatch)
	{
		for (auto &CastFlag : CastFlags)
		{
			if (Obj->HasTypeFlag(CastFlag))
				return true;
		}
		return false;
	} else{
		for (auto &CastFlag : CastFlags)
		{
			if (!Obj->HasTypeFlag(CastFlag))
				return false;
		}
		return true;
	}
}

std::vector<SDK::EClassCastFlags> AllCastFlags = GetAllAvailableCastFlags();

struct DualListBox
{
	ImVector<ImGuiID> Items[2];
	ImGuiSelectionBasicStorage Selections[2];
	bool OptKeepSorted = true;

	void MoveAll(int src, int dst)
	{
		IM_ASSERT((src == 0 && dst == 1) || (src == 1 && dst == 0));
		for (ImGuiID item_id : Items[src])
			Items[dst].push_back(item_id);
		Items[src].clear();
		SortItems(dst);
		Selections[src].Swap(Selections[dst]);
		Selections[src].Clear();
	}
	void MoveSelected(int src, int dst)
	{
		for (int src_n = 0; src_n < Items[src].Size; src_n++)
		{
			ImGuiID item_id = Items[src][src_n];
			if (!Selections[src].Contains(item_id))
				continue;
			Items[src].erase(&Items[src][src_n]); // FIXME-OPT: Could be implemented more optimally (rebuild src items and swap)
			Items[dst].push_back(item_id);
			src_n--;
		}
		if (OptKeepSorted)
			SortItems(dst);
		Selections[src].Swap(Selections[dst]);
		Selections[src].Clear();
	}
	void ApplySelectionRequests(ImGuiMultiSelectIO *ms_io, int side)
	{
		// In this example we store item id in selection (instead of item index)
		Selections[side].UserData = Items[side].Data;
		Selections[side].AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage *self, int idx)
		{ ImGuiID* items = (ImGuiID*)self->UserData; return items[idx]; };
		Selections[side].ApplyRequests(ms_io);
	}
	static int CompareItemsByValue(const void *lhs, const void *rhs)
	{
		const int *a = (const int *)lhs;
		const int *b = (const int *)rhs;
		return (*a - *b) > 0 ? +1 : -1;
	}
	void SortItems(int n)
	{
		qsort(Items[n].Data, (size_t)Items[n].Size, sizeof(Items[n][0]), CompareItemsByValue);
	}
	void Show()
	{
		// ImGui::Checkbox("Sorted", &OptKeepSorted);
		if (ImGui::BeginTable("split", 3, ImGuiTableFlags_None))
		{
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch); // Left side
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);	 // Buttons
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch); // Right side
			ImGui::TableNextRow();

			int request_move_selected = -1;
			int request_move_all = -1;
			float child_height_0 = 0.0f;
			for (int side = 0; side < 2; side++)
			{
				// FIXME-MULTISELECT: Dual List Box: Add context menus
				// FIXME-NAV: Using ImGuiWindowFlags_NavFlattened exhibit many issues.
				ImVector<ImGuiID> &items = Items[side];
				ImGuiSelectionBasicStorage &selection = Selections[side];

				ImGui::TableSetColumnIndex((side == 0) ? 0 : 2);
				ImGui::Text("%s (%d)", (side == 0) ? "Available" : "Basket", items.Size);

				// Submit scrolling range to avoid glitches on moving/deletion
				const float items_height = ImGui::GetTextLineHeightWithSpacing();
				ImGui::SetNextWindowContentSize(ImVec2(0.0f, items.Size * items_height));

				bool child_visible;
				if (side == 0)
				{
					// Left child is resizable
					ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetFrameHeightWithSpacing() * 4), ImVec2(FLT_MAX, FLT_MAX));
					child_visible = ImGui::BeginChild("0", ImVec2(-FLT_MIN, ImGui::GetFontSize() * 20), ImGuiChildFlags_FrameStyle | ImGuiChildFlags_ResizeY);
					child_height_0 = ImGui::GetWindowSize().y;
				}
				else
				{
					// Right child use same height as left one
					child_visible = ImGui::BeginChild("1", ImVec2(-FLT_MIN, child_height_0), ImGuiChildFlags_FrameStyle);
				}
				if (child_visible)
				{
					ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_None;
					ImGuiMultiSelectIO *ms_io = ImGui::BeginMultiSelect(flags, selection.Size, items.Size);
					ApplySelectionRequests(ms_io, side);

					for (int item_n = 0; item_n < items.Size; item_n++)
					{
						ImGuiID item_id = items[item_n];
						bool item_is_selected = selection.Contains(item_id);
						ImGui::SetNextItemSelectionUserData(item_n);
						ImGui::Selectable(CastFlag2Str(AllCastFlags[item_id]).c_str(), item_is_selected, ImGuiSelectableFlags_AllowDoubleClick);
						if (ImGui::IsItemFocused())
						{
							// FIXME-MULTISELECT: Dual List Box: Transfer focus
							if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
								request_move_selected = side;
							if (ImGui::IsMouseDoubleClicked(0)) // FIXME-MULTISELECT: Double-click on multi-selection?
								request_move_selected = side;
						}
					}

					ms_io = ImGui::EndMultiSelect();
					ApplySelectionRequests(ms_io, side);
				}
				ImGui::EndChild();
			}

			// Buttons columns
			ImGui::TableSetColumnIndex(1);
			ImGui::NewLine();
			// ImVec2 button_sz = { ImGui::CalcTextSize(">>").x + ImGui::GetStyle().FramePadding.x * 2.0f, ImGui::GetFrameHeight() + padding.y * 2.0f };
			ImVec2 button_sz = {ImGui::GetFrameHeight(), ImGui::GetFrameHeight()};

			// (Using BeginDisabled()/EndDisabled() works but feels distracting given how it is currently visualized)
			if (ImGui::Button(">>", button_sz))
				request_move_all = 0;
			if (ImGui::Button(">", button_sz))
				request_move_selected = 0;
			if (ImGui::Button("<", button_sz))
				request_move_selected = 1;
			if (ImGui::Button("<<", button_sz))
				request_move_all = 1;

			// Process requests
			if (request_move_all != -1)
				MoveAll(request_move_all, request_move_all ^ 1);
			if (request_move_selected != -1)
				MoveSelected(request_move_selected, request_move_selected ^ 1);

			// FIXME-MULTISELECT: Support action from outside
			/*
			if (OptKeepSorted == false)
			{
				ImGui::NewLine();
				if (ImGui::ArrowButton("MoveUp", ImGuiDir_Up)) {}
				if (ImGui::ArrowButton("MoveDown", ImGuiDir_Down)) {}
			}
			*/

			ImGui::EndTable();
		}
	}

	std::vector<SDK::EClassCastFlags> GetSelectedCastFlags()
	{
		std::vector<SDK::EClassCastFlags> CastFlags;
		for (int i = 0; i < Items[1].Size; i++)
		{
			ImGuiID item_id = Items[1][i];
			CastFlags.push_back(static_cast<SDK::EClassCastFlags>(AllCastFlags[item_id]));
		}
		return CastFlags;
	}
};

bool eclasscastflags_bhardmatch = false;
DualListBox eclasscastflags_filter;

void SDKOverlay()
{
	if (ImGui::Begin("SDK Explorer"))
	{
		if (SDKState() == sdk_state::NOT_INIT)
		{
			ImGui::Text("SDK Is not ready yet , you want to initialize it ?");
			if (ImGui::Button("Yes"))
			{
				SDKState() = sdk_state::WAITING;
				std::thread([]()
							{
				if (SDK::Init() == SDK::Status::Success)
				{
					SDKState() = sdk_state::INIT;
				} else {
					std::cout << "SDK::Init failed " << std::endl;
				}
				return 0; })
					.detach();
			}
		}
		else if (SDKState() == sdk_state::WAITING)
		{
			ImGui::Text("Waiting for sdk....");
		}
		else
		{
			if (ImGui::BeginTabBar("##tab"))
			{
				if (ImGui::BeginTabItem("Main"))
				{
					ImGui::Text("SDK is ready");
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Objects"))
				{
					ImGui::InputText("Search", &ObjectSearchFiler);
					for (int i = 0; i < SDK::GObjects->Num(); i++)
					{
						SDK::UObject *Obj = SDK::GObjects->GetByIndex(i);
						if (!Obj || Obj->IsDefaultObject() || Obj->GetName().find(ObjectSearchFiler) == std::string::npos)
							continue;
						if (!ObjectHasAnyCastFlag(Obj, eclasscastflags_filter.GetSelectedCastFlags(), eclasscastflags_bhardmatch))
							continue;

						if (ImGui::TreeNode(std::format("{}##{}", Obj->GetName(), i).c_str()))
						{
							ImGui::Text("Class : %s", Obj->Class()->Name().ToString().c_str());
							ImGui::Text("Address : %p", Obj);
							ImGui::Text("CastFlags : %s", BuildCastFlagsStr(GetCastFlags(Obj)).c_str());
							if (Obj->HasTypeFlag(SDK::CASTCLASS_UStruct))
							{
								SDK::UStruct *ObjStruct = reinterpret_cast<SDK::UStruct *>(Obj);
								if (SDK::State::UsesFProperty)
								{
									for (SDK::FField *Field = ObjStruct->ChildProperties(); Field; Field = Field->Next)
									{
										if (!Field->HasTypeFlag(SDK::CASTCLASS_FProperty))
											continue;
										SDK::FProperty *Property = reinterpret_cast<SDK::FProperty *>(Field);
										if (ImGui::TreeNode(Property->Name.ToString().c_str()))
										{
											ImGui::Text("Flags: %s", BuildPropertyFlagsStr(GetPropertyFlags(Property)).c_str());
											//
											ImGui::Text("Type %s", Property->ClassPrivate->Name.ToString().c_str());
											ImGui::Text("Class cast flags %s", BuildCastFlagsStr(GetCastFlags(Field)).c_str());
											if (Field->HasTypeFlag(SDK::EClassCastFlags::CASTCLASS_FStructProperty))
											{
												FStructProperty* test = reinterpret_cast<FStructProperty*>(Field);
												ImGui::Text("Struct %s", test->Struct->Name().ToString().c_str());
											}
											ImGui::TreePop();
										}
									}
								}
								else
								{
									for (SDK::UField *Child = ObjStruct->Children(); Child; Child = Child->Next())
									{
										if (!Child->HasTypeFlag(SDK::CASTCLASS_FProperty))
											continue;

										SDK::UProperty *Property = reinterpret_cast<SDK::UProperty *>(Child);
										if (ImGui::TreeNode(Property->Name().ToString().c_str()))
										{
											ImGui::TreePop();
										}
									}
								}
							}
							if (Obj->HasTypeFlag(SDK::EClassCastFlags::CASTCLASS_UEnum))
							{
								// the object is a UEnum
								SDK::UEnum* Enum = reinterpret_cast<SDK::UEnum*>(Obj);
								for (int i = 0; i < Enum->Names().Num(); i++)
								{
									ImGui::Text("%s %d", Enum->Names()[i].Key().ToString().c_str(), Enum->Names()[i].Value());
								}
							}
							ImGui::TreePop();
						}
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Filter DEV Test 1"))
				{
					if (ImGui::TreeNode("Search Filters"))
					{
						ImGui::InputText("Search", &ObjectSearchFiler);
						if (ImGui::TreeNode("ClassCastFlags"))
						{
							if (eclasscastflags_filter.Items[0].Size == 0 && eclasscastflags_filter.Items[1].Size == 0)
								for (int item_id = 0; item_id < AllCastFlags.size(); item_id++)
									eclasscastflags_filter.Items[0].push_back((ImGuiID)item_id);

							eclasscastflags_filter.Show();
							ImGui::Checkbox("bHardMatch", &eclasscastflags_bhardmatch);
							ImGui::Text("Selected CastFlags : %s", BuildCastFlagsStr(eclasscastflags_filter.GetSelectedCastFlags()).c_str());
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
	}
	ImGui::End();
}

DWORD WINAPI MainThread()
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
	dx11::m_drawList.push_back(SDKOverlay);
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
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, NULL, NULL, NULL);
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}
#endif