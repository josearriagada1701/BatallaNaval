#include <iostream>
#include <random>
#include <string>
#include <sstream>

using namespace std;

const int tam_tablero = 15;

class BatallaNaval {
private:
    int tablero1[tam_tablero][tam_tablero];
    int tablero2[tam_tablero][tam_tablero];
    int turno = 1;

public:
    BatallaNaval() {
        for (int i = 0; i < tam_tablero; i++) {
            for (int j = 0; j < tam_tablero; j++) {
                tablero1[i][j] = 0;
                tablero2[i][j] = 0;
            }
        }
    }

    //Para imprimir la columna de letras del tablero
    char transformarNumeroALetra(int numero) {
        return 'A' + numero;
    }

    //Recibe el tablero maquina o jugador y imprime el detalle 
    

    void imprimir(int (*tablero)[15], std::string& salida) {
    
    stringstream buffer;

    for (int i = 0; i < 15; i++) {
        buffer << transformarNumeroALetra(i) << " ";
        for (int j = 0; j < 15; j++) {
            // Puse el tamaño de la embarcación y el negativo es cuando están undidos
            switch (tablero[i][j]) {
                case -1:
                    buffer << "L ";
                    break;
                case -3:
                    buffer << "S ";
                    break;
                case -4:
                    buffer << "B ";
                    break;
                case -5:
                    buffer << "P ";
                    break;
                default:
                    buffer << "  ";
                    break;
            }
        }
        buffer << std::endl;
    }
    buffer << "-------------------------------" << std::endl;
    buffer << "  1 2 3 4 5 6 7 8 9 . . . . . 15" << std::endl;

    salida = buffer.str();
}

    int nAlearorio(int n) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, n);
        int numeroAleatorio = dis(gen);
        return numeroAleatorio;
    }

    //Obs la embarcaciones son representadas por el tamaño 5 portaviones...
    void colocar_piezas(int (*tablero)[tam_tablero], int t) {
        int x = nAlearorio(tam_tablero - 1);
        int y = nAlearorio(tam_tablero - 1);
        //Genera si es voy a poner horizontal o vertical la embarcación
        int d = nAlearorio(1);
        int b = 0;
        switch (d) {
            //Para colocar embacación vertical
            case 0:
                //ve si no me salgo de la matriz
                if (y + t > tam_tablero) {
                    colocar_piezas(tablero, t);
                    return;
                }
                //Pregúnta si hay una embarcación ahí. Si es así inicial 
                //el proceso otra vez, otra direción y otro lugar
                for (int i = 0; i < t; i++) {
                    if (tablero[x][y + i] != 0) {
                        colocar_piezas(tablero, t);
                        return;
                    }
                }
                //Coloco la pieza
                for (int i = 0; i < t; i++) {
                    tablero[x][y + i] = t;
                }
                break;

            //Lo mismo pero horizontal
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

    //Es para el server 
    bool perdio(){
        return perdio2(tablero1) || perdio2(tablero2);
    }

    //Pregunta si quedan embarcaciones
    bool perdio2(int (*tablero)[tam_tablero]) {
        for (int i = 0; i < tam_tablero; i++) {
            for (int j = 0; j < tam_tablero; j++) {
                int aux = tablero[i][j];
                //Todas la embarcaciones son mayores que 0
                if (aux > 0) {
                    return false; // no ha perdido
                }
            }
        }
        return true;
    }

    void dispararTablero(int (*tablero)[tam_tablero],int x,int y) {
        

        int aux = tablero[x][y];
        //La emabarciones son mayores que 0, y representadas por su tamaño
        //menor que cero es que disparé ahí 
        //Para diparo en el agua
        if (aux == 0) {
            tablero[x][y] = -2;
        //Para disparo en una embarcación, pongo el tamaño negativo
        } else {
            tablero[x][y] = aux*-1;
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
    }


   string disparar(int n1, int n2) {
    string salida;

    if (!perdio2(tablero1) && !perdio2(tablero2)) {
        if (turno % 2 == 1) {
            cout << "Disparo Jugador";
            dispararTablero(tablero1,n1,n2);
            imprimir(tablero1, salida);
        } else {
            cout << "Disparo Maquina";
            dispararTablero(tablero2,nAlearorio(14),nAlearorio(14));
            imprimir(tablero2, salida);
        }
    } else {
        salida = "Juego Terminado";
    }

    return salida;
    }

};

