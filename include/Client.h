#ifndef RSO_ICE_CLIENT_H
#define RSO_ICE_CLIENT_H

#include <Ice/Ice.h>
#include <memory>

namespace chat {

class Client : public Ice::Application {
public:
    virtual int run(int argc, char* argv[]) override;
};

}

#endif
