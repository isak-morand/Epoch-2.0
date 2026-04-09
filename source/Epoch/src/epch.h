#pragma once

#ifdef EPOCH_PLATFORM_WINDOWS
#include <Windows.h>
#include <shellapi.h>
#endif

#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Epoch/Debug/Log.h"
#include "Epoch/Debug/Assert.h"
