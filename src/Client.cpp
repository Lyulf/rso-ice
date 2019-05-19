#include <Client.h>
#include <Ice/Ice.h>

namespace chat {

struct Client::ClientImpl : public virtual Ice::Application {
    virtual int run(int argc, char* argv[]) {
        (void) argc;
        (void) argv;
        return 0;
    }
};

Client::Client() 
    : impl(std::make_unique<ClientImpl>()) { }

Client::~Client() = default;

int Client::main(int argc, char* argv[]) {
    return impl->main(argc, argv); 
}

}
