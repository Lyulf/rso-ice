#include <chatServerI.h>

namespace chat {

// returns RoomList of all existing rooms
RoomList chatServerI::getRooms(const Ice::Current& current) {

}

// returns room with given name if it exists
std::shared_ptr<chatRoomPrx> chatServerI::getRoom(std::string name, const Ice::Current& current) {

}

// creates chat room using least used factory
std::shared_ptr<chatRoomPrx> chatServerI::newChatRoom(std::string name, const Ice::Current& current) {

}

// registers new factory
void chatServerI::registerFactory(std::shared_ptr<chatRoomFactoryPrx> crf, const Ice::Current& current) {

} 

// unregisters new factory
void chatServerI::unregisterFactory(std::shared_ptr<chatRoomFactoryPrx> crf, const Ice::Current& current) {

}

}
