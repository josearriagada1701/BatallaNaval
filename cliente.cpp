#include <iostream>
#include <winsock2.h>
#include <random>
#include <vector>
#include <algorithm>

using namespace std;

const int tam_tablero = 15;

int nAleatorio(int n)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n);
    int numeroAleatorio = dis(gen);
    return numeroAleatorio;
}

class Client
{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    Client()
    {
        cout << "Conectando al servidor..." << endl
             << endl;
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr("192.168.31.177");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5555);
        connect(server, (SOCKADDR *)&addr, sizeof(addr));
        cout << "Conectado al Servidor!" << endl;
    }
    void Enviar(const std::pair<int, int> &mensaje)
    {
        std::string coordenadas = std::to_string(mensaje.first) + "," + std::to_string(mensaje.second);
        const char *msg = coordenadas.c_str();
        send(server, msg, strlen(msg), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;
    }
    std::pair<int, int> Recibir()
    {
        recv(server, buffer, sizeof(buffer), 0);
        cout << "El Servidor dice: " << buffer << endl;
        std::string buf = buffer;
        memset(buffer, 0, sizeof(buffer));

        size_t commaPos = buf.find(',');
        if (commaPos != std::string::npos)
        {
            int x = std::stoi(buf.substr(0, commaPos));
            int y = std::stoi(buf.substr(commaPos + 1));
            return std::make_pair(x, y);
        }

        return std::make_pair(-1, -1);
    }
    void CerrarSocket()
    {
        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado." << endl
             << endl;
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
            cout << "Servidor ha dado al agua" << endl;
            break;
        case 5:
            cout << "Servidor ha dado un Portaviones" << endl;
            break;
        case 4:
            cout << "Servidor ha dado un Buque" << endl;
            break;
        case 3:
            cout << "Servidor ha dado un Submarino" << endl;
            break;
        case 1:
            cout << "Servidor ha dado una Lancha" << endl;
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
    Client *Cliente = new Client();
    Juego *juego = new Juego();

    juego->iniciarJuego();
    juego->imprimir_tablero(juego->tablero);

    while (!juego->perdio())
    {
        std::pair<int, int> coordenadas;
        do
        {
            cout << "Ingrese las coordenadas a atacar: ";
            cin >> coordenadas.first >> coordenadas.second;
        } while (juego->coordenadaAtacada(coordenadas.first, coordenadas.second));

        Cliente->Enviar(coordenadas);

        std::pair<int, int> coordenadasRecibidas = Cliente->Recibir();
        juego->recibirDisparo(juego->tablero, coordenadasRecibidas.first, coordenadasRecibidas.second);
        juego->imprimir_tablero(juego->tablero);

        cout << endl
             << endl;
             
        system("cls");
    }

    cout << "Perdiste" << endl;

    /* juego->iniciarJuego();
    juego->imprimir_tablero(juego->tablero);
    Cliente->Enviar(std::make_pair(1, 2));
    Cliente->Recibir();
    system("pause"); */
}
