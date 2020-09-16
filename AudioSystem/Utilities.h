#pragma once

#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <map>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <complex>
#include <exception>
#include <limits>
#include <utility>
#include <memory>
#include <queue>
#include <condition_variable>
#include <iomanip>
#include <array>
#include <filesystem>
#include <numbers>
#include <functional>

#define _USE_MATH_DEFINES
#include <math.h>
//AVX
#include <immintrin.h>

//NameofLib
//#include <nameof.hpp>

std::wstring Convert_Str_to_Wstr(std::string const& _src);
std::string Convert_Wstr_to_Str(std::wstring const& _src);