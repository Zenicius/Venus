#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>

#include "Engine/Base.h"
#include "Engine/Log.h"
#include "Utils/Random.h"
#include "Debug/Instrumentor.h"

#ifdef VS_PLATFORM_WINDOWS
#include <Windows.h>
#endif