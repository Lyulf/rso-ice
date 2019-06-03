#include <chatServerI.h>
#include <algorithm>
#include <Ice/Ice.h>
#include <ChatUtils.h>
#include <unordered_set>

using namespace std;

namespace chat {

chatServerI::chatServerI()
    : chatServer(), chatRooms(), factories() { }

chatServerI::~chatServerI() = default;

// returns RoomList of all existing rooms
RoomList chatServerI::getRooms(const Ice::Current& current) {
    UNUSED(current);
    RoomList currentRooms;

    for(auto room : chatRooms) {
        RoomInfo info;
        info.name = room.first;
        info.activeUsers = room.second->listUsers().size();
        currentRooms.push_back(info);
    }
    return currentRooms;
}

// returns room with given name if it exists
shared_ptr<chatRoomPrx> chatServerI::getRoom(string name, const Ice::Current& current) {
    UNUSED(current);
    name = validateName(name);
    if(chatRooms.find(name) == chatRooms.end()) {
        throw NoSuchRoom();
    }
    return chatRooms[name];
}

// creates chat room using least used factory
shared_ptr<chatRoomPrx> chatServerI::newChatRoom(string name, const Ice::Current& current) {
    UNUSED(current);
    static long long roomNo = 0;
    if(name.empty()) {
        name = "Room" + to_string(roomNo + 1);
    }
    name = validateName(name);
    if(chatRooms.find(name) != chatRooms.end()) {
        throw NameAlreadyExists();
    }
    shared_ptr<chatRoomFactoryPrx> factory;
    size_t minUsers = numeric_limits<size_t>::max();
    for(auto factoryPair : factories) {
        size_t users = 0;
        for(auto chatRoomName : factoryPair.second) {
            users += chatRooms[chatRoomName]->listUsers().size();
        }
        if(users < minUsers) {
            factory = factoryPair.first;
            minUsers = users;
        }
    }
    auto room = chatRooms[name] = factory->newChatRoom(name);
    factories[factory].insert(name);
    cout << "Created room [" << name << "] using factory " << factory->ice_getIdentity().name << endl;
    roomNo++;
    return room;
}

// registers new factory
void chatServerI::registerFactory(chatRoomFactoryPrxPtr crf, const Ice::Current& current) {
    UNUSED(current);
    factories[crf];
    cout << "Registered factory " << crf->ice_getIdentity().name << endl;
} 

// unregisters factory
void chatServerI::unregisterFactory(chatRoomFactoryPrxPtr crf, const Ice::Current& current) {
    UNUSED(current);
    auto rooms = factories[crf];
    for(auto iter = chatRooms.begin(); iter != chatRooms.end(); iter++) {
       if(rooms.find(iter->first) != rooms.end()) {
           chatRooms.erase(iter);
       } 
    }
    factories.erase(crf);
    cout << "Unregistered factory " << crf->ice_getIdentity().name << endl;
}

}