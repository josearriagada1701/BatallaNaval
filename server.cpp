#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Server {
private:
    int serverSocket;

public:
    Server() : serverSocket(-1) {}

    bool Initialize(int port) {
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
            std::cerr << "Error al vincular el socket" << std::endl;
            return false;
        }

        // Escuchar en el socket del servidor
        if (listen(serverSocket, 5) == -1) {
            std::cerr << "Error al escuchar en el socket" << std::endl;
            return false;
        }

        std::cout << "El servidor está en espera de conexiones..." << std::endl;
        return true;
    }

    void Start() {
        while (true) {
            struct sockaddr_in clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);

            // Aceptar una conexión entrante
            int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
            if (clientSocket == -1) {
                std::cerr << "Error al aceptar la conexión" << std::endl;
                return;
            }

            HandleClient(clientSocket);

            // Cerrar el socket del cliente
            close(clientSocket);
        }
    }

private:
    void HandleClient(int clientSocket) {
        char buffer[1024];

        // Leer datos del cliente
        memset(buffer, 0, sizeof(buffer));
        if (read(clientSocket, buffer, sizeof(buffer)) == -1) {
            std::cerr << "Error al leer los datos del cliente" << std::endl;
            return;
        }

        std::cout << "Mensaje del cliente: " << buffer << std::endl;

        // Enviar respuesta al cliente
        const char *response = "Hola, cliente!";
        if (write(clientSocket, response, strlen(response)) == -1) {
            std::cerr << "Error al enviar la respuesta al cliente" << std::endl;
            return;
        }
    }
};

int main() {
    Server server;
    if (server.Initialize(8080)) {
        server.Start();
    }

    return 0;
}

