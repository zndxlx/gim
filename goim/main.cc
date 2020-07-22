#include "evpp/event_loop.h"
#include "evpp/event_loop_thread.h"
//#include <vector>
#ifdef _WIN32
#include "winmain-inl.h"
#endif

#include "GConnection.h"

void Print() {
    std::cout << "Hello, world!\n";
}

int main() {
    evpp::EventLoopThread loop;
    loop.Start(true);

    goim::GConnection client(loop.loop(), goim::GConnection::Option());
    client.Connect();

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "quit") {
            client.Close();
            break;
        }
        std::cout<< "not support" << std::endl;
    }
    loop.Stop(true);
   // loop.RunAfter(evpp::Duration(5.0), &Print);
   // loop.RunEvery(evpp::Duration(1.0), []() { std::cout << "Hello, world!\n"; });
   // loop.Run();
    return 0;
}