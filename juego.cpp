#include <iostream>
#include <random>

using namespace std;

const int tam_tablero = 15;

class BattleshipGame {
private:
    int tablero1[tam_tablero][tam_tablero];
    int tablero2[tam_tablero][tam_tablero];

public:
    BattleshipGame() {
        // Inicializar los tableros
        for (int i = 0; i < tam_tablero; i++) {
            for (int j = 0; j < tam_tablero; j++) {
                tablero1[i][j] = 0;
                tablero2[i][j] = 0;
            }
        }
    }

    void imprimir_tablero(int (*tablero)[tam_tablero]) {
        for (int i = 0; i < tam_tablero; i++) {
            for (int j = 0; j < tam_tablero; j++) {
                cout << tablero[i][j] << " ";
            }
            cout << std::endl;
        }
    }

    int nAleatorio(int n) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, n);
        int numeroAleatorio = dis(gen);
        return numeroAleatorio;
    }

    void colocar_piezas(int (*tablero)[tam_tablero], int t) {
        int x = nAleatorio(tam_tablero - 1);
        int y = nAleatorio(tam_tablero - 1);
        int d = nAleatorio(1);
        int b = 0;
        switch (d) {
            case 0:
                if (y + t > tam_tablero) {
                    colocar_piezas(tablero, t);
                    return;
                }
                for (int i = 0; i < t; i++) {
                    if (tablero[x][y + i] != 0) {
                        colocar_piezas(tablero, t);
                        return;
                    }
                }
                for (int i = 0; i < t; i++) {
                    tablero[x][y + i] = t;
                }
                break;

            case 1:
                if (x + t > tam_tablero) {
                    colocar_piezas(tablero, t);
                    return;
                }
                for (int i = 0; i < t; i++) {
                    if (tablero[x + i][y] != 0) {
                        colocar_piezas(tablero, t);
                        return;
                    }
                }
                for (int i = 0; i < t; i++) {
                    tablero[x + i][y] = t;
                }
                break;

            default:
                break;
        }
    }

    bool perdio(int (*tablero)[tam_tablero]) {
        for (int i = 0; i < tam_tablero; i++) {
            for (int j = 0; j < tam_tablero; j++) {
                int aux = tablero[i][j];
                if (aux > 0) {
                    return false; // si quedan piezas
                }
            }
        }
        return true;
    }

    void disparar(int (*tablero)[tam_tablero]) {
        int x = nAleatorio(14);
        int y = nAleatorio(14);
        int aux = tablero[x][y];
        if (aux < 0) {
            disparar(tablero);
            return;
        }
        if (aux == 0) {
            tablero[x][y] = -1;
        } else {
            tablero[x][y] = -2;
        }
        switch (aux) {
            case 0:
                cout << "Haz dado al agua" << endl;
                break;
            case 5:
                cout << "Haz dado a un Portaviones" << endl;
                break;
            case 4:
                cout << "Haz dado a un Buque" << endl;
                break;
            case 3:
                cout << "Haz dado a un Submarino" << endl;
                break;
            case 1:
                cout << "Haz dado a una Lancha" << endl;
            default:
                break;
        }
    }

    void iniciarJuego() {
        cout << "Colocando Piezas Jugador" << endl;
        colocar_piezas(tablero1, 5);
        colocar_piezas(tablero1, 4);
        colocar_piezas(tablero1, 4);
        colocar_piezas(tablero1, 3);
        colocar_piezas(tablero1, 3);
        colocar_piezas(tablero1, 1);
        colocar_piezas(tablero1, 1);
        colocar_piezas(tablero1, 1);

        cout << "Colocando Piezas Maquina" << endl;
        colocar_piezas(tablero2, 5);
        colocar_piezas(tablero2, 4);
        colocar_piezas(tablero2, 4);
        colocar_piezas(tablero2, 3);
        colocar_piezas(tablero2, 3);
        colocar_piezas(tablero2, 1);
        colocar_piezas(tablero2, 1);
        colocar_piezas(tablero2, 1);

        int b = 1;
        while (b != 0) {
            if (b % 2 == 1) {
                cout << "Turno " << b << endl;
                cout << "Disparo Jugador" << endl;
                disparar(tablero1);
            } else {
                cout << "Turno " << b << endl;
                cout << "Disparo Maquina" << endl;
                disparar(tablero2);
            }
            if (perdio(tablero1) || perdio(tablero2)) {
                if (perdio(tablero1)) {
                    cout << "Haz Perdido" << endl;
                } else {
                    cout << "Haz ganado" << endl;
                }
                b = 0;
            } else {
                b++;
            }
        }
        cout << "Tablero Jugador" << endl;
        imprimir_tablero(tablero1);
        cout << "Tablero Maquina" << endl;
        imprimir_tablero(tablero2);
    }
};

int main() {
    BattleshipGame juego;
    juego.iniciarJuego();
    return 0;
}