// ServerApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;

//Initialize winsock lib
bool Initialize() {
    WSAData data;
    return WSAStartup(MAKEWORD(2,2),&data)==0;
}
//Create socket
//Get ip and port
//bind the ip/port with the socket
//listen ont he socket
//accept
//recieve and send


void InteractWithClient(SOCKET clientSocket, vector<SOCKET> &clients) {
    //send /recv
    cout << "client conencted" << endl;
    //recieve
    char buffer[4096];
    while (1) {
        int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesrecvd <= 0) {
            cout << "client disconnected" << endl;
            break;
        }
        string message(buffer, bytesrecvd);
        cout << "message from cleint: " << message << endl;

        for (auto client : clients) {
            if (client != clientSocket) {
                send(client, message.c_str(), message.length(), 0);
            }
            
        }
    }

    auto it = find(clients.begin(), clients.end(),clientSocket);
    if (it != clients.end()) {
        clients.erase(it);
    }
    closesocket(clientSocket);
}

int main()
{
    if (!Initialize()) {
        cout << "winosck intiitalization failed" << endl;
        return 1;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket == INVALID_SOCKET) {
        cout << "socket creation failed" << endl;
        return 1;
    }

    //create address structure
    int port = 12345;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);

    //convert IP (0.0.0.0) to binary format and put it in sin_family
    if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
        cout << "setting adress structure failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
   }

    //bind code
    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "bind failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    //listen

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "server has startd listening on port:" << port << endl;
    vector<SOCKET> clients;
    while (1) {
        //accept

        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cout << "invalid client socket" << endl;
        }
        clients.push_back(clientSocket);
        thread t1(InteractWithClient, clientSocket,ref(clients));
        t1.detach();
    }
   /* cout << "client conencted" << endl;
    //recieve
    char buffer[4096];
   int bytesrecvd= recv(clientSocket,buffer,sizeof(buffer),0);

   string message(buffer, bytesrecvd);
   cout << "message from cleint: " << message << endl;

   closesocket(clientSocket);*/
   

   closesocket(listenSocket);
   
    WSACleanup();
    return 0;
    
}


