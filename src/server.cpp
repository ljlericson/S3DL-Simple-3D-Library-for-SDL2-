
#include "engine.h"

int main(int argv, char** args)
{
    // init all SDL subsystems (I'm lazy)
    engine::initSDL();

    bool running = true;
    
    IPaddress ip;
    SDLNet_ResolveHost(&ip, nullptr, 4305);
    TCPsocket server = SDLNet_TCP_Open(&ip);
    packet serverPacket = {{0, 0, 0}, "VideoShip", 2};

    TCPsocket client;

    while(running)
    {
        client = SDLNet_TCP_Accept(server);
        if(client)
        {
            SDLNet_TCP_Send(client, &serverPacket, 100);
            SDLNet_TCP_Close(client);
            std::cout << "Connection established\n";
        }
    }

    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
