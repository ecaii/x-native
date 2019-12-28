#pragma once
#include <unordered_map>

using item_group_map = std::unordered_map<std::uint32_t, ItemRow>;
using item_database = std::unordered_map<std::string, item_group_map>;