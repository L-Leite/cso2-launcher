#pragma once

#include <stdint.h>

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>

#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#else
#error Include your OS specific headers here
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4819 ) // Save file to unicode to avoid data lost warning
#pragma warning( disable : 4005 ) // Macro redefinion warning from polyhook
#endif 

#include "utilities.hpp"

using namespace std::literals::string_literals;