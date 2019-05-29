#include <chatRoomI.h>
#include <algorithm>
#include <ChatUtils.h>
#include <sstream>

using namespace std;

namespace chat {

chatRoomI::chatRoomI()
    : chatRoom(), users() { }

chatRoomI::~chatRoomI() = default;

// lists all users currently connected
userList chatRoomI::listUsers(const Ice::Current& current) {
    UNUSED(current);
    userList currentUsers;
    transform(users.begin(), users.end(), back_inserter(currentUsers), [](auto userPair) { return userPair.first; });
    cout << "Returning list of users\n";
    return currentUsers;
}

// adds user to the room 
void chatRoomI::join(string nick, shared_ptr<UserPrx> who, const Ice::Current& current) {
    UNUSED(current);
    nick = validateName(nick);
    if(users.find(nick) != users.end()) {
        throw NickNotAvailable();
    }
    users[nick] = who;
    ostringstream ss;
    ss << "User \"" << nick << "\" joined the room";
    string msg = ss.str();
    cout << msg << endl;
    postMessage(msg, "Server", current);
}

// ????
void chatRoomI::postMessage(string message, string fromWho, const Ice::Current& current) {
    UNUSED(current);
    fromWho = validateName(fromWho);
    for(auto userPair : users) {
        auto user = userPair.second;
        user->sendMessage(message, fromWho);
    }
    cout << "Message \"" << message << "\" from user \"" << fromWho << "\" sent\n";
}

// gets user with the given name
shared_ptr<UserPrx> chatRoomI::getUser(string name, const Ice::Current& current) {
    UNUSED(current);
    name = validateName(name);
    auto user = users.find(name);
    if(user == users.end()) {
        cout << "Couldn't find user \"" << name << "\"\n";
        throw NoSuchUser();
    }
    cout << "Returning user \"" << name << "\"\n";
    return user->second;
}

// removes user from the room
void chatRoomI::Leave(string name, const Ice::Current& current) {
    UNUSED(current);
    name = validateName(name);
    users.erase(name);
    ostringstream ss;
    ss << "User \"" << name << "\" left the room";
    string msg = ss.str();
    cout << msg << endl;
    postMessage(msg, "Server", current);
}

}
