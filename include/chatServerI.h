#ifndef RSO_ICE_CHATSERVERI_H
#define RSO_ICE_CHATSERVERI_H

#include <chat.h>
#include <map>
#include <unordered_set>
#include <ChatUtils.h>

namespace chat {

class chatServerI : public chatServer {
public:
	chatServerI();
	virtual ~chatServerI();

	virtual RoomList getRooms(const Ice::Current& current);
	std::shared_ptr<chatRoomPrx> getRoom(std::string name, const Ice::Current& current);
	std::shared_ptr<chatRoomPrx> newChatRoom(std::string name, const Ice::Current& current);
	void registerFactory(std::shared_ptr<chatRoomFactoryPrx> crf, const Ice::Current& current);
	void unregisterFactory(std::shared_ptr<chatRoomFactoryPrx> crf, const Ice::Current& current);

private:
	std::map<std::string, chatRoomPrxPtr> chatRooms;
	std::map<chatRoomFactoryPrxPtr, std::unordered_set<std::string>, Ice::ProxyIdentityLess> factories;
};

}

#endif
