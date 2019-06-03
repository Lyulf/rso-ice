#include <ChatUtils.h>
#include <algorithm>

using namespace std;

namespace chat {

string validateName(string name) {
    transform(name.begin(), name.end(), name.begin(), ::tolower);    
    name.front() = ::toupper(name.front());
    return name;
}

bool ProxyComparator::operator()(Ice::ObjectPrxPtr left, Ice::ObjectPrxPtr right) const {
    return left->ice_getIdentity() == right->ice_getIdentity(); 
}

}