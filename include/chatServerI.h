#ifndef RSO_ICE_CHATSERVERI_H
#define RSO_ICE_CHATSERVERI_H

#include <chat.h>

namespace chat {

class chatServerI : public chat::chatServer {
public:
	virtual RoomList getRooms(const Ice::Current& current);
	std::shared_ptr<chatRoomPrx> getRoom(std::string name, const Ice::Current& current);
	std::shared_ptr<chatRoomPrx> newChatRoom(std::string name, const Ice::Current& current);
	void registerFactory(std::shared_ptr<chatRoomFactoryPrx> crf, const Ice::Current& current);
	void unregisterFactory(std::shared_ptr<chatRoomFactoryPrx> crf, const Ice::Current& current);
};

}

#endif
