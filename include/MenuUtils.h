#ifndef UTILS_MENU_UTILS_H
#define UTILS_MENU_UTILS_H

#include <ostream>
#include <vector>

namespace utils {

std::ostream& removeNCharacters(std::ostream& os, std::size_t n);
std::vector<std::string> splitString(const std::string& str, const char delimiter);

}

#endif