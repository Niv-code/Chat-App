#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() 
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(54321);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    std::cout << "Chat Server is running on port 54321..." << std::endl;
    std::cout << "Waiting for a client to connect..." << std::endl;

    sockaddr_in clientAddr;
    int clientSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

    std::cout << "A client has connected successfully!" << std::endl;

    char buffer[1024] = {0};
    while (true) 
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (bytesReceived <= 0) 
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        std::cout << "Message from client: " << buffer << std::endl;
        
        const char* response = "Message received by server!";
        send(clientSocket, response, strlen(response), 0);
        
        memset(buffer, 0, sizeof(buffer)); 
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    
    std::cout << "Server shutting down." << std::endl;
    return 0;
}