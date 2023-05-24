
#include <iostream>
#include <random>

using namespace std;

const int tam_tablero = 10;

int tablero1[tam_tablero][tam_tablero];
int tablero2[tam_tablero][tam_tablero];

void imprimir_tablero(int (*tablero)[tam_tablero]) {
	
	for(int i=0;i<tam_tablero;i++){
		for(int j=0;j<tam_tablero;j++){
			cout << tablero[i][j] << " ";
		}
		
		cout << std::endl;
	}	
}

int nAlearorio(int n){
	random_device rd;
	mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n);
    int numeroAleatorio = dis(gen);
    return numeroAleatorio;

}

void colocar_piezas(int (*tablero)[tam_tablero], int t) {
	//Portaaviones 5 casillas, buque 4 casillas
	//, submarino 3 casillas, lancha 1 casilla.
	int x = nAlearorio(9);
	int y =nAlearorio(9);
	// 0 abajo, 1 arriaba , 2 derecha y 3 izq
	int d = nAlearorio(3);
	int b = 0;
	d =0;
	
	
	switch (d) {
	        case 0:
	        	for(int i=0;i<t && y+i<10;i++)
	        		if(tablero[x][y+i]!=0)
	        			colocar_piezas(tablero,t);
	        	for(int i=0;i<t;i++)
	        			tablero[x][y+i]=t;
	            break;
	        case 1:
	        	if(y-t>=0 && y+t<10){
	        		for(int i=0;i<t;i++)
	        			if(tablero[x][y-i]!=0){
	        				colocar_piezas(tablero,t);
	        			}
	        		for(int i=0;i<t;i++)
	        			tablero[x][y-i]=t;
	        		}
	            break;
	        case 2:
	        	if(x+t<=9 ){
	        		for(int i=0;i<t;i++)
	        			if(tablero[x+i][y]!=0){
	        				colocar_piezas(tablero,t);
	        			}
	        		for(int i=0;i<t;i++)
	        			tablero[x+i][y]=t;

	        	}
	        	break;
	        case 3:
	        	if(x-t<=9 && x+t<10){
	        		for(int i=0;i<t;i++)
	        			if(tablero[x-i][y]!=0){
	        				colocar_piezas(tablero,t);
	        			}
	        		for(int i=0;i<t;i++)
	        			tablero[x-i][y]=t;

	        	}
	        	break;
	        default:
	            break;
	}

}





int main(){
	colocar_piezas(tablero1,5);
	colocar_piezas(tablero1,4);
	//colocar_piezas(tablero1,3);
	//colocar_piezas(tablero1,1);
	imprimir_tablero(tablero1);
}