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

class Server {
private:
    int serverSocket;
    int clientSocket;
    std::mutex mutex;
    std::vector<std::thread> threads;

public:
    Server() : serverSocket(-1), clientSocket(-1) {}

    bool Start(int port) {
        // Crear el socket del servidor
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "Error al crear el socket" << std::endl;
            return false;
        }

        // Configurar la dirección del servidor
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        // Vincular el socket a la dirección del servidor
        if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Error al vincular el socket a la dirección del servidor" << std::endl;
            return false;
        }

        // Escuchar conexiones entrantes
        if (listen(serverSocket, 5) == -1) {
            std::cerr << "Error al escuchar conexiones entrantes" << std::endl;
            return false;
        }

        return true;
    }

    bool Accept() {
        // Aceptar la conexión del cliente
        clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "Error al aceptar la conexión del cliente" << std::endl;
            return false;
        }

        return true;
    }

    std::string ReceiveMessage() {
        char buffer[1024];

        // Leer el mensaje del cliente
        memset(buffer, 0, sizeof(buffer));
        if (read(clientSocket, buffer, sizeof(buffer)) == -1) {
            std::cerr << "Error al leer el mensaje del cliente" << std::endl;
            return "";
        }

        return std::string(buffer);
    }

    void SendMessage(const std::string& message) {
        // Enviar respuesta al cliente
        if (write(clientSocket, message.c_str(), message.size()) == -1) {
            std::cerr << "Error al enviar la respuesta al cliente" << std::endl;
            return;
        }
    }

    void Close() {
        // Cerrar el socket del cliente
        close(clientSocket);
    }

    void ClientHandler() {
        std::string message;
        BatallaNaval juego;
        juego.iniciarJuego();

        while (true) {
            std::cout << "Turno " << juego.turno << std::endl;
            message = ReceiveMessage();
            if (message.empty()) {
                std::cout << "Error al recibir mensaje. Finalizando conexión..." << std::endl;
                break;
            }

            std::cout << "Mensaje recibido: " << message << std::endl;

            size_t delimiterPos = message.find('&');
            if (delimiterPos != std::string::npos) {
                int num1 = std::stoi(message.substr(0, delimiterPos));
                int num2 = std::stoi(message.substr(delimiterPos + 1));
                std::cout << "Disparando Jugador: " << num1 << " " << num2 << std::endl;
                std::string respuesta = juego.disparar(num1, num2);
                SendMessage(respuesta);
                if (juego.perdio()) {
                    std::cout << "Jugador perdió. Finalizando conexión..." << std::endl;
                    break;
                }
                std::cout << "Disparando Máquina..." << std::endl;
                juego.disparar(0, 0);
                if (juego.perdio()) {
                    std::cout << "Máquina perdió. Finalizando conexión..." << std::endl;
                    break;
                }
            } else {
                std::cout << "Mensaje inválido. Se esperaba formato 'num1&num2'" << std::endl;
                break;
            }
        }

        Close();

        // Bloquear el mutex antes de eliminar el hilo
        std::lock_guard<std::mutex> lock(mutex);

        // Buscar y eliminar el hilo actual de la lista
        auto it = std::find_if(threads.begin(), threads.end(), [&](const std::thread& t) {
            return t.get_id() == std::this_thread::get_id();
        });
        if (it != threads.end()) {
            it->detach();
            threads.erase(it);
        }
    }

    void HandleConnections() {
        while (true) {
            if (Accept()) {
                std::cout << "Cliente conectado. Esperando mensajes..." << std::endl;

                // Crear un nuevo hilo para atender al cliente
                std::thread thread(&Server::ClientHandler, this);

                // Bloquear el mutex antes de agregar el hilo a la lista
                std::lock_guard<std::mutex> lock(mutex);

                // Agregar el hilo a la lista
                threads.push_back(std::move(thread));
            }
        }
    }
};

int main() {
    Server server;
    if (server.Start(8080)) {
        std::cout << "Servidor iniciado. Esperando conexiones entrantes..." << std::endl;
        server.HandleConnections();
    }

    return 0;
}
