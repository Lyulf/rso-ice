#ifndef RSO_ICE_CHATUTILS_H
#define RSO_ICE_CHATUTILS_H  

#include <string>
#include <MenuUtils.h>
#include <Ice/Ice.h>

#define UNUSED(x) (void)(x)

namespace chat {
using namespace utils;

std::string validateName(std::string name);

struct ProxyComparator {
    bool operator()(Ice::ObjectPrxPtr left, Ice::ObjectPrxPtr right) const;
};

}

#endif