
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

class Client {
private:
    int clientSocket;

public:
    Client() : clientSocket(-1) {}

    bool Connect(const string& serverIP, int serverPort) {
        // Crear el socket del cliente
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cerr << "Error al crear el socket" << endl;
            return false;
        }

        // Configurar la dirección del servidor
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
            cerr << "Dirección IP inválida" << endl;
            return false;
        }

        // Conectar al servidor
        if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            cerr << "Error al conectar al servidor" << endl;
            return false;
        }

        return true;
    }

    void SendMessage(const string& message) {
        // Enviar mensaje al servidor
        if (write(clientSocket, message.c_str(), message.size()) == -1) {
            cerr << "Error al enviar el mensaje" << endl;
            return;
        }

        char buffer[2048];

        // Leer respuesta del servidor
        memset(buffer, 0, sizeof(buffer));
        if (read(clientSocket, buffer, sizeof(buffer)) == -1) {
            cerr << "Error al leer la respuesta del servidor" << endl;
            return;
        }

        cout << "Respuesta del servidor: " << buffer << endl;
    }

    void Close() {
        // Cerrar el socket del cliente
        close(clientSocket);
    }
};

int main(int argc, char* argv[]) {

    //127.0.0.1
    string ip;
    int puerto;
    if (argc == 3) {
        ip = argv[1];
        puerto = stoi(argv[2]);
        }
    else{
        cout << "Uso: programa <ip> <puerto>"<< endl;
        return 0;
    }

    Client client;
    if (client.Connect(ip, puerto)) {

        while(true){
            int numero1, numero2;

            // Pedir el primer número
            cout << "Posición X(0,15): ";
            cin >> numero1;

            // Verificar si está dentro del rango
            while (numero1 < 0 || numero1 > 15) {
                cout << "El número ingresado está fuera del rango.";
                cin >> numero1;
            }

            // Pedir el segundo número
            cout << "Posición Y:(0,15): ";
            cin >> numero2;

            // Verificar si está dentro del rango
            while (numero2 < 0 || numero2 > 15) {
                cout << "El número ingresado está fuera del rango.";
                cin >> numero2;
            }
            string mensaje = to_string(numero1) + "&" + to_string(numero2);

            client.SendMessage(mensaje);
        }
        client.Close();
    }

    return 0;
}
