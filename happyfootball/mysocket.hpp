#include <string>

class HappyTCP {
    public:
        HappyTCP(std::string ip, int port);
        ~HappyTCP();
        void send(const char* data, int size);
        void receive();
};