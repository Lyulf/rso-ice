#include <chatRoomI.h>
#include <algorithm>
#include <ChatUtils.h>
#include <sstream>

using namespace std;

namespace chat {

chatRoomI::chatRoomI(const string& roomName)
    : chatRoom(), roomName(roomName), users() { }

chatRoomI::~chatRoomI() = default;

// lists all users currently connected
userList chatRoomI::listUsers(const Ice::Current& current) {
    UNUSED(current);
    userList currentUsers;
    transform(users.begin(), users.end(), back_inserter(currentUsers), [](auto userPair) { return userPair.first; });
    return currentUsers;
}

// adds user to the room 
void chatRoomI::join(string nick, shared_ptr<UserPrx> who, const Ice::Current& current) {
    UNUSED(current);
    static long long userNo = 0;
    if(nick.empty()) {
        nick = "User" + to_string(userNo + 1);
    }
    nick = validateName(nick);
    if(users.find(nick) != users.end()) {
        throw NickNotAvailable();
    }
    userNo++;
    users[nick] = who;
    ostringstream ss;
    ss << "User \"" << nick << "\" joined the room [" << roomName << "]";
    string msg = ss.str();
    cout << msg << endl;
    postMessage(msg, "Server", current);
}

// posts message to every user in the room
void chatRoomI::postMessage(string message, string fromWho, const Ice::Current& current) {
    UNUSED(current);
    fromWho = validateName(fromWho);
    for(auto userPair : users) {
        auto user = userPair.second;
        user->sendMessage(message, fromWho);
    }
}

// gets user with the given name
shared_ptr<UserPrx> chatRoomI::getUser(string name, const Ice::Current& current) {
    UNUSED(current);
    name = validateName(name);
    auto user = users.find(name);
    if(user == users.end()) {
        throw NoSuchUser();
    }
    return user->second;
}

// removes user from the room
void chatRoomI::Leave(string name, const Ice::Current& current) {
    UNUSED(current);
    name = validateName(name);
    users.erase(name);
    ostringstream ss;
    ss << "User \"" << name << "\" left the room [" << roomName << "]";
    string msg = ss.str();
    cout << msg << endl;
    postMessage(msg, "Server", current);
}

}
