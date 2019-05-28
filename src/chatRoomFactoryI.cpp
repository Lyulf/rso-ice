#include <chatRoomFactoryI.h>
#include <chatRoomI.h>
#include <Ice/Ice.h>
#include <ChatUtils.h>

using namespace std;

namespace chat {

// creates new chat room
shared_ptr<chatRoomPrx> chatRoomFactoryI::newChatRoom(string name, const Ice::Current& current) {
    UNUSED(current);
    auto room = make_shared<chatRoomI>();
    chatRooms[name] = room;
    cout << "Room [" << name << "] created\n";
    return Ice::uncheckedCast<chatRoomPrx>(current.adapter->addWithUUID(room));
}

}