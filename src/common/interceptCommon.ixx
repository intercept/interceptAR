module;

// Headers included in the global module fragment can be used by the module implementation but are not exported.
#include <Windows.h>
#include <Psapi.h>

export module InterceptCommon;


#include "util.hpp"
#include "dllInterface.hpp"
#include "genericTypes.hpp"
#include "interceptTypes.hpp"
#include "enfusionContainers.hpp"
#include "enfusionTypes.hpp"
