#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <memory>
#include <array>

#pragma comment(lib, "ws2_32.lib")


std::string getGeminiResponse(const std::string& userMessage) {
    
    std::string apiKey = "AIzaSyBNpHQomCwftI9tfOhHMil5sRAut-QvFow"; 
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=" + apiKey;

    std::string safeMessage = userMessage;
    size_t pos = 0;
    while ((pos = safeMessage.find("\"", pos)) != std::string::npos) {
         safeMessage.replace(pos, 1, "\\\"");
         pos += 2;
    }
    pos = 0;
    while ((pos = safeMessage.find("\n", pos)) != std::string::npos) {
         safeMessage.replace(pos, 1, " ");
         pos += 1;
    }

    std::string jsonPayload = "{\\\"contents\\\":[{\\\"parts\\\":[{\\\"text\\\":\\\"" + safeMessage + "\\\"}]}]}";

    std::string command = "curl -X POST -H \"Content-Type: application/json\" -d \"" + jsonPayload + "\" \"" + url + "\" 2>&1";

    std::array<char, 256> buffer;
    std::string result;
    
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
    if (!pipe) {
        return "System: Failed to connect to internet.";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) 
    {
        result += buffer.data();
    }

    std::string searchKey = "\"text\": \"";
    size_t startPos = result.find(searchKey);
    if (startPos != std::string::npos) {
        startPos += searchKey.length();
        size_t endPos = result.find("\"", startPos);

        while (endPos != std::string::npos && result[endPos - 1] == '\\') {
            endPos = result.find("\"", endPos + 1);
        }

        if (endPos != std::string::npos) {
            std::string reply = result.substr(startPos, endPos - startPos);
            
            
            size_t n_pos = 0;
            while((n_pos = reply.find("\\n", n_pos)) != std::string::npos) {
                reply.replace(n_pos, 2, "\n");
                n_pos += 1;
            }
            
            n_pos = 0;
            while((n_pos = reply.find("\\\"", n_pos)) != std::string::npos) {
                reply.replace(n_pos, 2, "\"");
                n_pos += 1;
            }

            return "Gemini: " + reply;
        }
    }

    return "System Error: Could not understand Gemini's response.";
}

void handleClient(SOCKET clientSocket) {
    char buffer[4096];
      
    std::string welcomeMsg = "Gemini: Hello! I'm connected. Ask me anything.";
    send(clientSocket, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

    while (true) {
        ZeroMemory(buffer, 4096);
        int bytesReceived = recv(clientSocket, buffer, 4096, 0);
        
        if (bytesReceived <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        std::string receivedMessage(buffer, 0, bytesReceived);
        std::cout << "User asked: " << receivedMessage << std::endl;

        
        std::string aiReply = getGeminiResponse(receivedMessage);
        
        
        send(clientSocket, aiReply.c_str(), aiReply.size() + 1, 0);
    }
    
    closesocket(clientSocket);
}

int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    if (WSAStartup(ver, &wsData) != 0) return 1;

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) return 1;

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54321); 
    hint.sin_addr.S_un.S_addr = INADDR_ANY; 

    bind(listening, (sockaddr*)&hint, sizeof(hint));
    listen(listening, SOMAXCONN);

    std::cout << "Gemini AI Server is running on port 54321..." << std::endl;

    std::vector<std::thread> clientThreads;

    while (true) {
        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        
        if (clientSocket != INVALID_SOCKET) {
            std::cout << "New client connected to Gemini!" << std::endl;
            clientThreads.push_back(std::thread(handleClient, clientSocket));
            clientThreads.back().detach(); 
        }
    }

    closesocket(listening);
    WSACleanup();
    return 0;
}