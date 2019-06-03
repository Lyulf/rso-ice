#ifndef RSO_ICE_CHATROOM_IMPL_H
#define RSO_ICE_CHATROOM_IMPL_H

#include <chat.h>
#include <map>

namespace chat {

class chatRoomI : public ::chat::chatRoom {
public:
    chatRoomI(const std::string& roomName);
    virtual ~chatRoomI();

    userList listUsers(const Ice::Current& current);
    void join(std::string nick, std::shared_ptr<UserPrx> who, const Ice::Current& current);
    void postMessage(std::string message, std::string fromWho, const Ice::Current& current);
    std::shared_ptr<UserPrx> getUser(std::string name, const Ice::Current& current);
    void Leave(std::string name, const Ice::Current& current);

private:
    std::string roomName;
    std::map<std::string, std::shared_ptr<UserPrx>> users;
};

}
#endif
