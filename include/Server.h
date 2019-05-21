#ifndef RSO_ICE_SERVER_H
#define RSO_ICE_SERVER_H

#include <Ice/Ice.h>
#include <memory>

namespace chat {

class Server : public Ice::Application {
public:
    virtual int run(int argc, char* argv[]) override;
};

}

#endif
