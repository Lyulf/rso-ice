#include <UserI.h>
#include <iostream>
#include <ChatUtils.h>

using namespace std;

namespace chat {

// sends message to everyone in the chat room
void UserI::sendMessage(string message, string fromWho, const Ice::Current& current) {
    UNUSED(current);
    cout << fromWho << ": " << message << endl;
}

// sends private message to ???
void UserI::sendPrivateMessage(string messaage, string fromWho, const Ice::Current& current) {
    UNUSED(current);
    cout << "[private] " << fromWho << ": " << messaage << endl;
}

}