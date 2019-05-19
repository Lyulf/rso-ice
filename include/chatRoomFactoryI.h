#ifndef RSO_ICE_CHATROOMFACTORY_IMPL_H
#define RSO_ICE_CHATROOMFACTORY_IMPL_H

#include <chat.h>

namespace chat {

class chatRoomFactoryI : public ::chat::chatRoomFactory {
public:
    std::shared_ptr<chatRoomPrx> newChatRoom(std::string name, const Ice::Current& current);
};

}

#endif
