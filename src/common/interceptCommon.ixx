module;

// Headers included in the global module fragment can be used by the module implementation but are not exported.

#pragma warning(push)
#pragma warning(disable : 4005) // macro redefinition


#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#endif

#include <type_traits>
#include <filesystem>
#include <string_view>
#include <vector>
#include <array>
#include <charconv>
#include <cstring>
#include <initializer_list>
#include <cstdint>
#include <functional>
#include <typeinfo>
#include <optional>
#include <span>
#include <cmath>
#include <ranges>

#ifdef _WIN32
#include <format>
#endif

#if defined(__linux__)
#include <signal.h>
#include <dlfcn.h>
#endif

#pragma warning(pop)


export module InterceptCommon;




#include "util.hpp"
#include "dllInterface.hpp"
#include "genericTypes.hpp"
#include "interceptTypes.hpp"
#include "enfusionContainers.hpp"
#include "enfusionTypes.hpp"
