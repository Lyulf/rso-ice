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
    {
        lock_guard lck(mtx);
        transform(users.begin(), users.end(), back_inserter(currentUsers), [](auto userPair) { return userPair.first; });
    }
    return currentUsers;
}

// adds user to the room 
void chatRoomI::join(string nick, shared_ptr<UserPrx> who, const Ice::Current& current) {
    UNUSED(current);
    {
        lock_guard lck(mtx);
        nick = validateName(nick);
        if(users.find(nick) != users.end()) {
            throw NickNotAvailable();
        }
        users[nick] = who;
    }
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

    lock_guard lck(mtx);
    for(auto iter : users) {
        auto user = iter.second;
        user->sendMessage(message, fromWho);
    }
}

// gets user with the given name
shared_ptr<UserPrx> chatRoomI::getUser(string name, const Ice::Current& current) {
    UNUSED(current);
    name = validateName(name);
    decltype(users)::iterator user;
    {
        lock_guard lck(mtx);
        user = users.find(name);
    }
    if(user == users.end()) {
        throw NoSuchUser();
    }
    return user->second;
}

// removes user from the room
void chatRoomI::Leave(string name, const Ice::Current& current) {
    UNUSED(current);
    name = validateName(name);
    {
        lock_guard lck(mtx);
        users.erase(name);
    }
    ostringstream ss;
    ss << "User \"" << name << "\" left the room [" << roomName << "]";
    string msg = ss.str();
    cout << msg << endl;
    postMessage(msg, "Server", current);
}

}
