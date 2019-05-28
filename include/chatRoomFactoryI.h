#ifndef RSO_ICE_CHATROOMFACTORY_IMPL_H
#define RSO_ICE_CHATROOMFACTORY_IMPL_H

#include <chat.h>
#include <map>

namespace chat {

class chatRoomFactoryI : public chatRoomFactory {
public:
    std::shared_ptr<chatRoomPrx> newChatRoom(std::string name, const Ice::Current& current);

private:
    std::map<std::string, std::shared_ptr<chatRoom>> chatRooms;
};

}

#endif
