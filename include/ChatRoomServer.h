#ifndef RSO_ICE_CHATROOM_SERVER_H
#define RSO_ICE_CHATROOM_SERVER_H

#include <Ice/Ice.h>
#include <chat.h>

namespace chat {

class ChatRoomServer : public Ice::Application {
public:
	virtual int run(int argc, char* argv[]) override;
	virtual void interruptCallback(int signal) override;
private:
	chatServerPrxPtr mainServerPrx;
	chatRoomFactoryPrxPtr factoryPrx;
};

}

#endif
