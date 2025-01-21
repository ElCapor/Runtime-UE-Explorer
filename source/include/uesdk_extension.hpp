//
// @project     sdk-explorer
//
// @author      ElCapor
//
// @license     CC BY-NC-SA 4.0
//

/*
╔══════════════════════╗
║   UESDK Extensions   ║
╚══════════════════════╝
*/
#include <UESDK.hpp>
namespace uesdk {
    /**
     * @brief Get all available class cast flags from SDK enum
     * 
     * @return std::vector<SDK::EClassCastFlags> 
     */
    std::vector<SDK::EClassCastFlags> GetAllAvailableClassCastFlags();
    /**
     * @brief Get all available class cast flags for a specific object
     * 
     * @param obj SDK::UObject*
     * @return std::vector<SDK::EClassCastFlags> 
     */
    std::vector<SDK::EClassCastFlags> GetClassCastFlagsForObjec(SDK::UObject* obj);

    // my attempt at writing a quick dispatch func
    /**
     * @brief Get the string representation of a class cast flag
     * 
     * @tparam Flag 
     * @return const char* 
     */
    template <SDK::EClassCastFlags Flag>
    const char* ClassCastFlagToString();

    /**
     * Check if an object has any of the given class cast flags
     * 
     * @param obj 
     * @param flags 
     * @return true or false
     */
    bool ObjectHasAnyClassCastFlag(SDK::UObject* obj, std::vector<SDK::EClassCastFlags> flags);
    
    /**
     * Check if an object has all of the given class cast flags
     * 
     * @param obj 
     * @param flags 
     * @return true or false
     */
    bool ObjectHasAllClassCastFlags(SDK::UObject* obj, std::vector<SDK::EClassCastFlags> flags);


    std::string ClassCastFlag2Str(SDK::EClassCastFlags Flag);
}

