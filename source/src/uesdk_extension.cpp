//
// @project     sdk-explorer
//
// @author      ElCapor
//
// @license     CC BY-NC-SA 4.0
//

#include "uesdk_extension.hpp"

/*
╔══════════════════════╗
║   Public Functions   ║
╚══════════════════════╝
*/
std::vector<SDK::EClassCastFlags> uesdk::GetAllAvailableClassCastFlags()
{
    return std::vector<SDK::EClassCastFlags>();
}

std::vector<SDK::EClassCastFlags> uesdk::GetClassCastFlagsForObjec(SDK::UObject *obj)
{
    return std::vector<SDK::EClassCastFlags>();
}

bool uesdk::ObjectHasAnyClassCastFlag(SDK::UObject *obj, std::vector<SDK::EClassCastFlags> flags)
{
    for (auto& flag: flags)
    {
        if (obj->HasTypeFlag(flag))
            return true;
    }
    return false;
}

bool uesdk::ObjectHasAllClassCastFlags(SDK::UObject *obj, std::vector<SDK::EClassCastFlags> flags)
{
    for (auto& flag: flags)
    {
        if (!obj->HasTypeFlag(flag))
            return false;
    }
    return true;
}

template <SDK::EClassCastFlags Flag>
const char *uesdk::ClassCastFlagToString()
{
    static const char* name = ClassCastFlag2Str(Flag);
    return name;
}



/*
╔════════════════════════╗
║   Internal Functions   ║
╚════════════════════════╝
*/

std::string uesdk::ClassCastFlag2Str(SDK::EClassCastFlags Flag)
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