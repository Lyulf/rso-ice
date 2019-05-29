#include <MenuUtils.h>
#include <sstream>

using namespace std;

namespace utils {

vector<string> splitString(const string& str, const char delimiter) {
    vector<string> result;
    stringstream ss(str);
    string token;
    
    while(getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

std::ostream& removeNCharacters(std::ostream& os, std::size_t n) {
    for(std::size_t i = 0; i < n; i++) {
        os << "\b \b";
    }
    return os;
}

}