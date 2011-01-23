#pragma once

#include <string>

std::string GenerateKey();
BOOL HasRegistered(const std::string &appName);
BOOL Register(const std::string &appName);