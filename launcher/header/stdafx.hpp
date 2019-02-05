#pragma once

#include <stdint.h>

#include <array>
#include <string>
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

using namespace std::literals::string_literals;