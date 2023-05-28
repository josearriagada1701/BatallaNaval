
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class Client {
private:
    int clientSocket;

public:
    Client() : clientSocket(-1) {}

    bool Connect(const std::string& serverIP, int serverPort) {
        // Crear el socket del cliente
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            std::cerr << "Error al crear el socket" << std::endl;
            return false;
        }

        // Configurar la dirección del servidor
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
            std::cerr << "Dirección IP inválida" << std::endl;
            return false;
        }

        // Conectar al servidor
        if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Error al conectar al servidor" << std::endl;
            return false;
        }

        return true;
    }

    void SendMessage(const std::string& message) {
        // Enviar mensaje al servidor
        if (write(clientSocket, message.c_str(), message.size()) == -1) {
            std::cerr << "Error al enviar el mensaje" << std::endl;
            return;
        }

        char buffer[1024];

        // Leer respuesta del servidor
        memset(buffer, 0, sizeof(buffer));
        if (read(clientSocket, buffer, sizeof(buffer)) == -1) {
            std::cerr << "Error al leer la respuesta del servidor" << std::endl;
            return;
        }

        std::cout << "Respuesta del servidor: " << buffer << std::endl;
    }

    void Close() {
        // Cerrar el socket del cliente
        close(clientSocket);
    }
};

int main() {
    Client client;
    if (client.Connect("127.0.0.1", 8080)) {

        while(true){
            int numero1, numero2;

            // Pedir el primer número
            std::cout << "Posición X(0,15): ";
            std::cin >> numero1;

            // Verificar si está dentro del rango
            while (numero1 < 0 || numero1 > 15) {
                std::cout << "El número ingresado está fuera del rango.";
            }

            // Pedir el segundo número
            std::cout << "Posición Y:(0,15): ";
            std::cin >> numero2;

            // Verificar si está dentro del rango
            while (numero2 < 0 || numero2 > 15) {
                std::cout << "El número ingresado está fuera del rango.";
                std::cin >> numero2;
            }
            std::string mensaje = std::to_string(numero1) + "&" + std::to_string(numero2);

            client.SendMessage(mensaje);
    }
        client.Close();
    }

    return 0;
}
        

