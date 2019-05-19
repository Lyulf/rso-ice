#include <chatRoomI.h>

namespace chat {

// lists all users currently connected
userList listUsers(const Ice::Current& current) {

}

// adds user to the room 
void join(std::string nick, std::shared_ptr<UserPrx> who, const Ice::Current& current) {

}

// posts message from fromWho to other users in the room
void postMessage(std::string message, std::string fromWho, const Ice::Current& current) {

}

// gets user with the given name
std::shared_ptr<UserPrx> getUser(std::string name, const Ice::Current& current) {

}

// removes user from the room
void Leave(std::string name, const Ice::Current& current) {

}

}