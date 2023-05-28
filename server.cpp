#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include "juego.cpp"
#include <arpa/inet.h>

using namespace std;

class Server {
private:
    int serverSocket;
    int clientSocket;
    mutex mutexServer; // Cambio de nombre de 'mutex' a 'mutexServer'
    vector<thread> threads;

public:
    Server() : serverSocket(-1), clientSocket(-1) {}

    bool Start(int port) {
        // Crear el socket del servidor
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            cerr << "Error al crear el socket" << endl;
            return false;
        }

        // Configurar la dirección del servidor
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        // Vincular el socket a la dirección del servidor
        if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            cerr << "Error al vincular el socket a la dirección del servidor" << endl;
            return false;
        }

        // Escuchar conexiones entrantes
        if (listen(serverSocket, 5) == -1) {
            cerr << "Error al escuchar conexiones entrantes" << endl;
            return false;
        }

        return true;
    }

    bool Accept() {
        // Aceptar la conexión del cliente
        clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cerr << "Error al aceptar la conexión del cliente" << endl;
            return false;
        }

        return true;
    }

    string ReceiveMessage() {
        char buffer[1024];

        // Leer el mensaje del cliente
        memset(buffer, 0, sizeof(buffer));
        if (read(clientSocket, buffer, sizeof(buffer)) == -1) {
            cerr << "Error al leer el mensaje del cliente" << endl;
            return "";
        }

        return string(buffer);
    }

    void SendMessage(const string& message) {
        // Enviar respuesta al cliente
        if (write(clientSocket, message.c_str(), message.size()) == -1) {
            cerr << "Error al enviar la respuesta al cliente" << endl;
            return;
        }
    }

    void Close() {
        // Cerrar el socket del cliente
        close(clientSocket);
    }

    void ClientHandler(string quien) {
        string message;
        BatallaNaval juego;
        juego.iniciarJuego();
        string quien2;
        while (true) {
            message = ReceiveMessage();
            if (message.empty()) {
                break;
            }

            

            size_t delimiterPos = message.find('&');
            if (delimiterPos != string::npos) {
                int num1 = stoi(message.substr(0, delimiterPos));
                int num2 = stoi(message.substr(delimiterPos + 1));
                if(juego.turno == 1)
                    cout << "Juego ["<< quien <<"]: inicia disparos el cliente." << endl;
                //else
                    quien2 = "Juego [" + quien + "]: Cliente";
                string respuesta = juego.disparar(num1, num2, quien2);
                SendMessage(respuesta);
                if (juego.perdio()) {
                    cout << "Juego [" << quien <<"]: gana cliente" << endl;
                    break;
                }
                if(juego.turno == 2)
                    cout << "Juego ["<< quien <<"]: inicia disparos el cliente." << endl;
                //else
                    quien2 = "Juego [" + quien + "]: Servidor";
                juego.disparar(0, 0, quien2);
                if (juego.perdio()) {
                    cout << "Juego [" << quien <<"]: gana servidor" << endl;
                    break;
                }
            } else {
                
                break;
            }
        }

        Close();

        // Bloquear el mutex antes de eliminar el hilo
        lock_guard<mutex> lock(mutexServer);

        // Buscar y eliminar el hilo actual de la lista
        auto it = find_if(threads.begin(), threads.end(), [&](const thread& t) {
            return t.get_id() == this_thread::get_id();
        });
        if (it != threads.end()) {
            it->detach();
            threads.erase(it);
        }
    }

    void HandleConnections() {
        string quien;
        int JugadorN = 1;
        while (true) {
            if (Accept()) {
                sockaddr_in clientAddress;
                socklen_t clientAddressLength = sizeof(clientAddress);
                getpeername(clientSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
                

                uint16_t port = ntohs(clientAddress.sin_port);

                string clientIP = inet_ntoa(clientAddress.sin_addr);
                string clientPort = to_string(port);
                quien = clientIP + ":" + clientPort;

                cout << "Juego nuevo [" << quien << "]" << endl;
                // Crear un nuevo hilo para atender al cliente
                thread thread(&Server::ClientHandler, this, quien);

                // Bloquear el mutex antes de agregar el hilo a la lista
                lock_guard<mutex> lock(mutexServer);

                // Agregar el hilo a la lista
                threads.push_back(move(thread));
               
            }
        }
    }
};

int main() {
    Server server;
    if (server.Start(8080)) {
        cout << "Esperando conexiones ..." << endl;
        server.HandleConnections();
    }

    return 0;
}
