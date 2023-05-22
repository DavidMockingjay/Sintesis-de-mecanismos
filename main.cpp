#include <iostream> // Librería base
#include <cmath>    // Librería para funciones matemáticas
#include <vector>   // Librería para utilizar vectores
#include <iomanip>  // Librería para formato de salida
#include <fstream>  //Librería para manejar archivos
#include <sstream>  //Librería para cadenas de caracteres
using namespace std;

// Función de producto vectorial
vector<double> PV(const vector<double>& v1, const vector<double>& v2) {     //Las entradas son dos vectores
    vector<double> Vector(3);
    Vector[0] = v1[1] * v2[2] - v1[2] * v2[1];
    Vector[1] = v1[2] * v2[0] - v1[0] * v2[2];
    Vector[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return Vector;                                  // Se resuelve a la brava y regresa un Vector
}

// Función determinante
double Det(double a, double b, double c, double d) {
    return a * d - b * c; // También se resuelve a la brava y regresa el determinante
}

// Programa principal
int main() {
    double L1, L2, L3, L4, O1, O2, O3, LA, OA, wi;      // Declaramos las variables
    int z=10;                                           // Ancho de columnas de la tabla de salida
    cout << fixed << setprecision(2);                   // Fijamos la precisión de nuestros resultados en 2 decimales

    // Pidiendo los valores de entrada
    cout << "Ingrese las longitudes de los eslabones 1, 2, 3 y el de referencia: " << endl;
    cin >> L1 >> L2 >> L3 >> L4;
    cout << "Ingrese la velocidad angular: ";
    cin >> wi;
    vector<double> w1{0, 0, wi};    // Guardar la velocidad angular como vector nos ayudará en los cálculos

    // Preparamos el archivo donde irán las entradas
    ofstream archivo("Síntesis de mecanismo.txt");          // En esta línea se indica donde está el archivo
    streambuf* respaldo = cout.rdbuf();                     // Respaldo del flujo de salida de texto
    cout.rdbuf(archivo.rdbuf());                            // Redirigimos la salida de texto al archivo

    // Imprimimos el encabezado de la tabla de salida
    cout << left << setw(z) << "ENTRADAS" << setw(16*z) << "SALIDAS" << endl;   // Fila de títulos
    cout << setw(z) << "" << setw(4*z) << "Coordenadas conexiones" << setw(2*z) << "Angulos barras" << setw(4*z) << "V. conexiones";
    cout << setw(2*z) << "V. barras" << setw(4*z) << "A. conexiones" << setw(2*z) << "A. barras" << endl;    //Fila de encabezados
    cout << setw(z) << "O1" << setw(z) << "Ax" << setw(z) << "Ay" << setw(z) << "Bx" << setw(z) << "By" << setw(z) << "O2" ;
    cout << setw(z) << "O3" << setw(2*z) << "VA" << setw(2*z) << "VB" << setw(z) << "w2" << setw(z) << "w3" << setw(2*z) << "AA";
    cout << setw(2*z) << "AB" << setw(z) << "a2" << setw(z) << "a3" << endl;        // Fila de símbolos
    cout << setw(19*z) << setfill('-') << "" << setfill(' ') << std::endl;          // Línea divisora

    for(int i = 0; i <= 359; i++) {    // Entramos en el ciclo que se repetirá 360 veces
        O1 = i * M_PI / 180;

        // Calculamos los ángulos O2 y O3 por trigonometría
        LA = sqrt(L1*L1 + L4*L4 - 2*L1*L4*cos(O1));
        OA = acos((L2*L2 - L3*L3 - LA*LA)/(-2*L3*LA));
        O3 = M_PI - OA - asin(L1*sin(O1)/LA);
        O2 = O3 - acos((LA*LA - L2*L2 - L3*L3)/(-2*L2*L3));     // Estas fórmulas pueden demostrarse por trigonometría

        // Calculando las coordenadas de las conexiones A y B
        vector<double> A{L1 * cos(O1) , L1 * sin(O1) , 0.0};              // Se descompone L1 en sus componentes rectangulares...
        vector<double> B{A[0] + L2*cos(O2) , A[1] + L2*sin(O2) , 0.0};    // ... y se suman las de L2

        // Calcular las velocidades angulares w2 y w3 y la velocidades VA y VB
        vector<double> r1{L1*cos(O1) , L1*sin(O1) , 0.0};       // Convertimos el primer eslabón en un vector
        vector<double> r2{L2*cos(O2) , L2*sin(O2) , 0.0};       // Convertimos el segundo eslabón en un vector
        vector<double> r3{L3*cos(O3) , L3*sin(O3) , 0.0};       // Convertimos el tercer eslabón en un vector
        vector<double> r4{L4 , 0.0 , 0.0};                      // Convertimos el cuarto eslabón en un vector
        vector<double> VA = PV(w1, r1);                         // Obtenemos VA como el producto vectorial de w1 y r1
        double D = Det(-r2[1],r3[1],r2[0],-r3[0]);              // Resolveremos el sistema de ecuaciones utilizando Cramer's rule
        double D2 = Det(-VA[0],r3[1],-VA[1],-r3[0]);
        double D3 = Det(-r2[1],-VA[0],r2[0],-VA[1]);            // Obtenemos las determinantes necesarias
        vector<double> w2{0.0, 0.0, D2/D};
        vector<double> w3{0.0, 0.0, D3/D};                      // Los resultados se almacenan como vectores
        vector<double> VB = PV(w3,r3);                          // Se calcula VB como el producto vectorial de w3 y r3

        // Calcular las aceleraciones en los puntos AA y AB, y las aceleraciones angulares a2 y a3
        vector<double> AA{-wi*wi*r1[0] , -wi*wi*r1[1] , 0.0};           // Se obtiene el vector aceleración AA
        double co1 = -w3[2]*w3[2]*r3[0] + w2[2]*w2[2]*r2[0] - AA[0];    // Se calculan los coeficientes del sistema
        double co2 = -w3[2]*w3[2]*r3[1] + w2[2]*w2[2]*r2[1] - AA[1];
        D2 = Det(co1,r3[1],co2,-r3[0]);                                 // Se calculan las determinantes necesarias
        D3 = Det(-r2[1],co1,r2[0],co2);
        vector<double> a2{0.0, 0.0, D2/D};
        vector<double> a3{0.0, 0.0, D3/D};                              // Se resuelve el sistema igual con Cramer
        vector<double> ABs = PV(a3,r3);
        vector<double> AB{ABs[0] - w3[2]*w3[2]*r3[0] , ABs[1] - w3[2]*w3[2]*r3[1]};      // Se despeja AB con la fórmula

        // Imprimir los resultados
        if (isnan(O2)) {        // Este "if" verifica si el ángulo O2 es inválido, lo cual significaría que la cadena cinemática no existe
            cout << setw(z) << i << setw(10*z) << "Los datos ingresados NO forman una cadena cinemática válida en el ángulo indicado" << endl;
        } else {                // Si no es inválido, entonces la cadena existe:
            ostringstream oss;
            oss << "(" << fixed << setprecision(2) << VA[0] << "," << fixed << setprecision(2) << VA[1] << ")";
            string VAA = oss.str();
            ostringstream ost;
            ost << "(" << fixed << setprecision(2) << VB[0] << "," << fixed << setprecision(2) << VB[1] << ")";
            string VBB = ost.str();
            ostringstream osu;
            osu << "(" << fixed << setprecision(2) << AA[0] << "," << fixed << setprecision(2) << AA[1] << ")";
            string AAA = osu.str();
            ostringstream osv;
            osv << "(" << fixed << setprecision(2) << AB[0] << "," << fixed << setprecision(2) << AB[1] << ")";
            string ABB = osv.str();          // Guardamos las cantidades vectoriales en una sola variable (me daba lata)
            cout << setw(z) << i << setw(z) << A[0] << setw(z) << A[1] << setw(z) << B[0] << setw(z) << B[1] << setw(z) << O2*180/M_PI;
            cout << setw(z) << O3*180/M_PI << setw(2*z) << VAA << setw(2*z) << VBB << setw(z) << w2[2] << setw(z) << w3[2] << setw(2*z) << AAA;
            cout << setw(2*z) << ABB << setw(z) << a2[2] << setw(z) << a3[2] << endl;    //Ahora si imprimimos nuestra fila de resultados
        }
    }
    archivo.close();        //Cerramos el archivo
    return 0;
}
