#ifndef RSO_ICE_USER_IMPL_H
#define RSO_ICE_USER_IMPL_H

#include <chat.h>

namespace chat {

class UserI : public ::chat::User {
public:
    void sendMessage(std::string message, std::string fromWho, const Ice::Current& current);
    void sendPrivateMessage(std::string message, std::string fromWho, const Ice::Current& current);
};

}

#endif