#pragma once

#include <string>

#include "miniredis/command.hpp"

constexpr char DELIM = ' ';

class RespParser {
public:
  Command parse(const std::string&);

};
