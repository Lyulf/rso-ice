#include <ChatUtils.h>
#include <algorithm>

using namespace std;

namespace chat {

string validateName(string name) {
    transform(name.begin(), name.end(), name.begin(), ::tolower);    
    name.front() = ::toupper(name.front());
    return name;
}

}