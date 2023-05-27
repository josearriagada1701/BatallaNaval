#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "juego.cpp"

class Server {
private:
    int serverSocket;
    int clientSocket;
    

public:
    BatallaNaval juego;
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
        if (listen(serverSocket, 1) == -1) {
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
        // Cerrar el socket del servidor
        close(serverSocket);

        // Cerrar el socket del cliente
        close(clientSocket);
    }
};

int main() {
    Server server;
    if (server.Start(8080)) {
        std::cout << "Servidor iniciado. Esperando conexiones entrantes..." << std::endl;

        if (server.Accept()) {
            std::cout << "Cliente conectado. Esperando mensajes..." << std::endl;

            server.juego.iniciarJuego(); // Iniciar el juego Batalla Naval

            while (true) {
                
                int num1,num2;
                std::string message = server.ReceiveMessage();
                size_t delimiterPos = message.find('&');

                if (delimiterPos != std::string::npos) {
                    std::string snum1 = message.substr(0, delimiterPos);
                    std::string snum2 = message.substr(delimiterPos + 1);
                    int num1 = std::stoi(snum1);
                    int num2 = std::stoi(snum2);
                } else {
                    continue;
                }

                return 0;            
                std::string respuesta = server.juego.disparar(num1, num2);

                // Enviar respuesta al cliente (puedes ajustar el mensaje según tus necesidades)
                server.SendMessage(respuesta);
                if (server.juego.perdio()) 
                     break;
                server.juego.disparar(-1, -1);
                if(server.juego.perdio())
                    break;

            }
        }

        server.Close();
    }

    return 0;
}