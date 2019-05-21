#ifndef RSO_ICE_CHATSERVERI_H
#define RSO_ICE_CHATSERVERI_H

#include <chat.h>
#include <map>
#include <unordered_set>

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
	std::map<std::string, std::shared_ptr<chatRoomPrx>> chatRooms;
	std::map<std::shared_ptr<chatRoomFactoryPrx>, std::unordered_set<std::string>> factories;
};

}

#endif
