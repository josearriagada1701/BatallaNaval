#include <iostream>
#include <winsock2.h>
#include <random>
#include <vector>
#include <algorithm>

using namespace std;
const int tam_tablero = 15;

int nAleatorio(int n)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);
    int numeroAleatorio = dis(gen);
    return numeroAleatorio;
}

class Server
{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    Server()
    {
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5555);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;
        int clientAddrSize = sizeof(clientAddr);
        if ((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
        }
    }

    std::pair<int, int> Recibir()
    {
        recv(client, buffer, sizeof(buffer), 0);
        cout << "El cliente dice: " << buffer << endl;
        std::string buf = buffer;
        memset(buffer, 0, sizeof(buffer));

        size_t commaPos = buf.find(',');
        if (commaPos != std::string::npos)
        {
            int x = std::stoi(buf.substr(0, commaPos));
            int y = std::stoi(buf.substr(commaPos + 1));
            return std::make_pair(x, y);
        }

        // Si el mensaje recibido no contiene un par de coordenadas válidas, devuelve (-1, -1)
        return std::make_pair(-1, -1);
    }
    void Enviar(const std::pair<int, int> &mensaje)
    {
        std::string coordenadas = std::to_string(mensaje.first) + "," + std::to_string(mensaje.second);
        const char *msg = coordenadas.c_str();
        send(client, msg, strlen(msg), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;
    }
    void CerrarSocket()
    {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
    }
};

class Juego
{
public:
    int tablero[tam_tablero][tam_tablero];
    vector<pair<int, int>> coordenadasAtacadas;
    Juego()
    {
        for (int i = 0; i < 15; i++)
        {
            for (int j = 0; j < 15; j++)
            {
                tablero[i][j] = 0;
            }
        }
    }

    void imprimir_tablero(int (*tablero)[tam_tablero])
    {
        for (int i = 0; i < tam_tablero; i++)
        {
            for (int j = 0; j < tam_tablero; j++)
            {
                cout << tablero[i][j] << " ";
            }
            cout << std::endl;
        }
    }

    void colocar_piezas(int (*tablero)[tam_tablero], int t)
    {
        int x = nAleatorio(tam_tablero - 1);
        int y = nAleatorio(tam_tablero - 1);
        int d = nAleatorio(1);
        int b = 0;
        switch (d)
        {
        case 0:
            if (y + t > tam_tablero)
            {
                colocar_piezas(tablero, t);
                return;
            }
            for (int i = 0; i < t; i++)
            {
                if (tablero[x][y + i] != 0)
                {
                    colocar_piezas(tablero, t);
                    return;
                }
            }
            for (int i = 0; i < t; i++)
            {
                tablero[x][y + i] = t;
            }
            break;

        case 1:
            if (x + t > tam_tablero)
            {
                colocar_piezas(tablero, t);
                return;
            }
            for (int i = 0; i < t; i++)
            {
                if (tablero[x + i][y] != 0)
                {
                    colocar_piezas(tablero, t);
                    return;
                }
            }
            for (int i = 0; i < t; i++)
            {
                tablero[x + i][y] = t;
            }
            break;

        default:
            break;
        }
    }

    bool perdio()
    {
        for (int i = 0; i < tam_tablero; i++)
        {
            for (int j = 0; j < tam_tablero; j++)
            {
                int aux = tablero[i][j];
                if (aux > 0)
                {
                    return false; // si quedan piezas
                }
            }
        }
        return true;
    }

    void recibirDisparo(int (*tablero)[tam_tablero], int x, int y)
    {
        int aux = tablero[x][y];
        if (aux < 0)
        {
            cout << "Ya has disparado a esta casilla" << endl;
            return;
        }
        if (aux == 0)
        {
            tablero[x][y] = -1;
        }
        else
        {
            tablero[x][y] = -2;
        }
        switch (aux)
        {
        case 0:
            cout << "Cliente ha dado al agua" << endl;
            break;
        case 5:
            cout << "Cliente ha dado al Portaviones" << endl;
            break;
        case 4:
            cout << "Cliente ha dado al Buque" << endl;
            break;
        case 3:
            cout << "Cliente ha dado al Submarino" << endl;
            break;
        case 1:
            cout << "Cliente ha dado al Lancha" << endl;
        default:
            break;
        }
    }

    void iniciarJuego()
    {
        cout << "Colocando Piezas Jugador" << endl;
        colocar_piezas(tablero, 5);
        colocar_piezas(tablero, 4);
        colocar_piezas(tablero, 4);
        colocar_piezas(tablero, 3);
        colocar_piezas(tablero, 3);
        colocar_piezas(tablero, 1);
        colocar_piezas(tablero, 1);
        colocar_piezas(tablero, 1);
    }
    bool coordenadaAtacada(int x, int y)
    {
        return find(coordenadasAtacadas.begin(), coordenadasAtacadas.end(), make_pair(x, y)) != coordenadasAtacadas.end();
    }

    void agregarCoordenadaAtacada(int x, int y)
    {
        coordenadasAtacadas.push_back(make_pair(x, y));
    }
};

int main()
{
    Server *Servidor = new Server();
    Juego *juego = new Juego();
    juego->iniciarJuego();
    juego->imprimir_tablero(juego->tablero);

    while (!juego->perdio())
    {

        std::pair<int, int> coordenadas = Servidor->Recibir();
        juego->recibirDisparo(juego->tablero, coordenadas.first, coordenadas.second);
        juego->imprimir_tablero(juego->tablero);
        cout << "Coordenadas recibidas" << endl;
        cout << coordenadas.first << " " << coordenadas.second << endl;
        int x, y;
        do
        {
            cout << "Ingrese coordenadas de disparo: " << endl;
            /* coordenadas aleatorias */
            x = nAleatorio(tam_tablero);
            y = nAleatorio(tam_tablero);
            cout << x << " " << y << endl;
        } while (juego->coordenadaAtacada(x, y));

        Servidor->Enviar(std::make_pair(x, y));
        juego->agregarCoordenadaAtacada(x, y);
        cout << "Coordenadas enviadas" << endl
             << endl
             << endl;
    }
    cout << "Perdiste" << endl;

    system("pause");
}