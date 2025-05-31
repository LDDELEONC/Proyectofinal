#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include "Usuario.h"
#include "utils.h"


using namespace std;
Usuario* usuarioActivo = nullptr;
bool sesion = false;
struct Nodo
{
    string espanol, italiano, frances, aleman, ingles;
    Nodo* izquierda;
    Nodo* derecha;
    int altura;

    Nodo(string esp, string ita, string fra, string ale, string ing)
    {
        espanol = esp;
        italiano = ita;
        frances = fra;
        aleman = ale;
        ingles = ing;
        izquierda = nullptr;
        derecha = nullptr;
        altura = 1;
    }
};

class ArbolTraducciones
{
private:
    Nodo* raiz;

    int obtenerAltura(Nodo* nodo)
    {
        return nodo ? nodo->altura : 0;
    }

    int obtenerBalance(Nodo* nodo)
    {
        return nodo ? obtenerAltura(nodo->izquierda) - obtenerAltura(nodo->derecha) : 0;
    }

    Nodo* rotacionDerecha(Nodo* y)
    {
        Nodo* x = y->izquierda;
        Nodo* T2 = x->derecha;

        x->derecha = y;
        y->izquierda = T2;

        y->altura = max(obtenerAltura(y->izquierda), obtenerAltura(y->derecha)) + 1;
        x->altura = max(obtenerAltura(x->izquierda), obtenerAltura(x->derecha)) + 1;

        return x;
    }

    Nodo* rotacionIzquierda(Nodo* x)
    {
        Nodo* y = x->derecha;
        Nodo* T2 = y->izquierda;

        y->izquierda = x;
        x->derecha = T2;

        x->altura = max(obtenerAltura(x->izquierda), obtenerAltura(x->derecha)) + 1;
        y->altura = max(obtenerAltura(y->izquierda), obtenerAltura(y->derecha)) + 1;

        return y;
    }

    Nodo* insertar(Nodo* nodo, string esp, string ita, string fra, string ale, string ing)
    {
        if (!nodo)
            return new Nodo(esp, ita, fra, ale, ing);

        if (esp < nodo->espanol)
            nodo->izquierda = insertar(nodo->izquierda, esp, ita, fra, ale, ing);
        else if (esp > nodo->espanol)
            nodo->derecha = insertar(nodo->derecha, esp, ita, fra, ale, ing);
        else
            return nodo;

        nodo->altura = 1 + max(obtenerAltura(nodo->izquierda), obtenerAltura(nodo->derecha));

        int balance = obtenerBalance(nodo);

        if (balance > 1 && esp < nodo->izquierda->espanol)
            return rotacionDerecha(nodo);

        if (balance < -1 && esp > nodo->derecha->espanol)
            return rotacionIzquierda(nodo);

        if (balance > 1 && esp > nodo->izquierda->espanol)
        {
            nodo->izquierda = rotacionIzquierda(nodo->izquierda);
            return rotacionDerecha(nodo);
        }

        if (balance < -1 && esp < nodo->derecha->espanol)
        {
            nodo->derecha = rotacionDerecha(nodo->derecha);
            return rotacionIzquierda(nodo);
        }

        return nodo;
    }

    Nodo* buscarPorEspanol(Nodo* nodo, const string& palabra)
    {
        if (!nodo || nodo->espanol == palabra)
            return nodo;

        if (palabra < nodo->espanol)
            return buscarPorEspanol(nodo->izquierda, palabra);
        else
            return buscarPorEspanol(nodo->derecha, palabra);
    }

    Nodo* buscarPorTraduccion(Nodo* nodo, const string& palabra, int idioma)
    {
        if (!nodo)
            return nullptr;

        bool encontrado = false;
        switch (idioma)
        {
        case 1: encontrado = (nodo->italiano == palabra); break;
        case 2: encontrado = (nodo->frances == palabra); break;
        case 3: encontrado = (nodo->aleman == palabra); break;
        case 4: encontrado = (nodo->ingles == palabra); break;
        }

        if (encontrado)
            return nodo;

        Nodo* izq = buscarPorTraduccion(nodo->izquierda, palabra, idioma);
        if (izq)
            return izq;

        return buscarPorTraduccion(nodo->derecha, palabra, idioma);
    }

    void mostrarRecursivo(Nodo* nodo)
    {
        if (!nodo)
            return;

        mostrarRecursivo(nodo->izquierda);
        cout << nodo->espanol << " | " << nodo->italiano << " | " << nodo->frances << " | "
            << nodo->aleman << " | " << nodo->ingles << "\n";
        mostrarRecursivo(nodo->derecha);
    }

    Nodo* nodoMinimo(Nodo* nodo)
    {
        Nodo* actual = nodo;
        while (actual && actual->izquierda)
            actual = actual->izquierda;
        return actual;
    }

    Nodo* eliminar(Nodo* raiz, string palabra)
    {
        if (!raiz)
            return raiz;

        if (palabra < raiz->espanol)
            raiz->izquierda = eliminar(raiz->izquierda, palabra);
        else if (palabra > raiz->espanol)
            raiz->derecha = eliminar(raiz->derecha, palabra);
        else
        {
            if (!raiz->izquierda || !raiz->derecha)
            {
                Nodo* temp = raiz->izquierda ? raiz->izquierda : raiz->derecha;
                delete raiz;
                return temp;
            }
            else
            {
                Nodo* temp = nodoMinimo(raiz->derecha);
                raiz->espanol = temp->espanol;
                raiz->italiano = temp->italiano;
                raiz->frances = temp->frances;
                raiz->aleman = temp->aleman;
                raiz->ingles = temp->ingles;
                raiz->derecha = eliminar(raiz->derecha, temp->espanol);
            }
        }

        raiz->altura = 1 + max(obtenerAltura(raiz->izquierda), obtenerAltura(raiz->derecha));
        int balance = obtenerBalance(raiz);

        if (balance > 1 && obtenerBalance(raiz->izquierda) >= 0)
            return rotacionDerecha(raiz);

        if (balance > 1 && obtenerBalance(raiz->izquierda) < 0)
        {
            raiz->izquierda = rotacionIzquierda(raiz->izquierda);
            return rotacionDerecha(raiz);
        }

        if (balance < -1 && obtenerBalance(raiz->derecha) <= 0)
            return rotacionIzquierda(raiz);

        if (balance < -1 && obtenerBalance(raiz->derecha) > 0)
        {
            raiz->derecha = rotacionDerecha(raiz->derecha);
            return rotacionIzquierda(raiz);
        }

        return raiz;
    }

    void inOrdenArchivo(Nodo* nodo, ofstream& archivo)
    {
        if (!nodo)
            return;

        inOrdenArchivo(nodo->izquierda, archivo);
        archivo << nodo->espanol << "," << nodo->italiano << "," << nodo->frances << ","
            << nodo->aleman << "," << nodo->ingles << "\n";
        inOrdenArchivo(nodo->derecha, archivo);
    }

    bool existePalabra(Nodo* nodo, const string& palabra)
    {
        if (!nodo) return false;
        if (palabra == nodo->espanol) return true;
        if (palabra < nodo->espanol) return existePalabra(nodo->izquierda, palabra);
        return existePalabra(nodo->derecha, palabra);
    }

public:
    ArbolTraducciones() : raiz(nullptr) {}

    void agregarPalabra(string esp, string ita, string fra, string ale, string ing)
    {
        raiz = insertar(raiz, esp, ita, fra, ale, ing);
    }

    void mostrarTodoEnPantalla()
    {
        cout << "\n DICCIONARIO \n";
        cout << "Espanol | Italiano | Frances | Aleman | Ingles\n";
        cout << "----------------------------------------------\n";
        mostrarRecursivo(raiz);
    }

    void buscarTraduccion(const string& palabra, int idiomaOrigen, int idiomaDestino)
    {
        Nodo* resultado = nullptr;

        if (idiomaOrigen == 0)
        {
            resultado = buscarPorEspanol(raiz, palabra);
            if (resultado)
            {
                string traduccion;
                switch (idiomaDestino)
                {
                case 1: traduccion = resultado->italiano; break;
                case 2: traduccion = resultado->frances; break;
                case 3: traduccion = resultado->aleman; break;
                case 4: traduccion = resultado->ingles; break;
                default: traduccion = "Idioma no valido"; break;
                }
                cout << "\n Traduccion encontrada: " << traduccion << endl;
                hablar(traduccion);
            }
            else
            {
                cout << "\n Palabra no encontrada\n";
            }
        }
        else
        {
            resultado = buscarPorTraduccion(raiz, palabra, idiomaOrigen);
            if (resultado)
            {
                cout << "\n Traduccion al espanol: " << resultado->espanol << endl;
            }
            else {
                cout << "\n Palabra no encontrada\n";
            }
        }
    }

    // Función para hablar (síntesis de voz usando PowerShell)
    void hablar(const string& texto) {
        string comando = "powershell -Command \"Add-Type –AssemblyName System.Speech; "
            "$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer; "
            "$speak.Speak(\\\"" + texto + "\\\");\"";
        system(comando.c_str());
    }

    bool eliminarPalabra(const string& palabra)
    {
        if (!existePalabra(raiz, palabra))
        {
            cout << "\n La palabra no existe en el diccionario.\n";
            return false;
        }

        raiz = eliminar(raiz, palabra);
        guardarEnArchivo();  
        cout << "\n Palabra eliminada correctamente.\n";
        return true;
    }

    void guardarEnArchivo()
    {
        if (usuarioActivo) {
            ofstream archivo(usuarioActivo->nombre + ".txt");
            if (!archivo.is_open())
            {
                cerr << "Error al abrir el archivo para guardar\n";
                return;
            }
            inOrdenArchivo(raiz, archivo);
            archivo.close();
        }
        else {
            ofstream archivo("Traducciones.txt");
            if (!archivo.is_open())
            {
                cerr << "Error al abrir el archivo para guardar\n";
                return;
            }
            inOrdenArchivo(raiz, archivo);
            archivo.close();
        }
    }
};

void leerArchivo(const string& nombreArchivo, ArbolTraducciones& arbol)
{
    ifstream archivo(nombreArchivo);
    if (!archivo)
    {
        cout << "Archivo no encontrado. Se creara uno nuevo al guardar.\n";
        return;
    }

    string linea;
    while (getline(archivo, linea))
    {
        stringstream ss(linea);
        string esp, ita, fra, ale, ing;

        getline(ss, esp, ',');
        getline(ss, ita, ',');
        getline(ss, fra, ',');
        getline(ss, ale, ',');
        getline(ss, ing, ',');

        arbol.agregarPalabra(esp, ita, fra, ale, ing);
    }

    archivo.close();
}

void limpiarBuffer()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void iniciarSesion() {
    std::string nombre, clave;
    std::cout << "\n--- INICIAR SESION ---\n";
    std::cout << "NOMBRE DE USUARIO : ";
    std::getline(std::cin, nombre);
    std::cout << "CLAVE  : ";
    std::getline(std::cin, clave);

    std::ifstream archivo("usuarios.txt");
    std::string linea;
    bool encontrado = false;

    while (std::getline(archivo, linea)) {
        size_t coma = linea.find(',');
        std::string nombreGuardado = linea.substr(0, coma);
        std::string claveGuardada = linea.substr(coma + 1);

        if (nombreGuardado == nombre && Decode(claveGuardada) == clave) {
            usuarioActivo = new Usuario(nombre, clave);
            std::cout << "SESION INICIADA COMO : '" << nombre << "'\n";
            encontrado = true;
            sesion = true;
            break;
        }
    }

    archivo.close();

}

void saveFileEncodeUser() {
    std::ifstream archivo( usuarioActivo->nombre + ".txt");
    std::ofstream archivoUsuarioEncode(usuarioActivo->nombre + "_Encode.txt", std::ios::app);
    std::string linea;    

    while (std::getline(archivo, linea)) {
        archivoUsuarioEncode << Encode(linea) << "\n";        
    }
    archivo.close();
    archivoUsuarioEncode.close();
}

void crearUsuario() {
    std::string nombre, clave;
    std::cout << "\n--------------- CREAR USUARIO -----------------\n";
    std::cout << "NOMBRE DE USUARIO ";
    std::getline(std::cin, nombre);
    std::cout << "CLAVE : ";
    std::getline(std::cin, clave);

    usuarioActivo = new Usuario(nombre, clave);
    std::cout << "USUARIO '" << nombre << "' CREADO EXITOSAMENTTE\n";

    //PARA GUARDAR USUARIO
    std::ofstream archivoUsuarios("usuarios.txt", std::ios::app);
    archivoUsuarios << usuarioActivo->nombre << "," << usuarioActivo->claveEncriptada << "\n";
    archivoUsuarios.close();
}

void mostrarMenu()
{
    cout << "  \n";
    cout << "----------Menu---------- \n";
    cout << "1. Mostrar todas las palabras\n";
    cout << "2. Buscar traduccion\n";
    cout << "3. Agregar nueva palabra\n";
    cout << "4. Eliminar palabra\n";
    cout << "5. Guardar en archivo\n";
    cout << "6. Limpiar Consola\n";
    cout << "7. Guardar Archivo Encriptado por Usuario\n";
    cout << "8. Salir\n";
    cout << "Seleccione una opcion: ";
}

void MenuArchivos() {    
    ArbolTraducciones arbol;
    cout << " ---------------------------    Sistema de traducciones Iniciado ------------------------------------------ \n";
    

    if (usuarioActivo) {
        std::cout << " SESION INICIADA COMO : '" << usuarioActivo->nombre << "'\n";
        leerArchivo(usuarioActivo->nombre + ".txt", arbol);
        
    }
    else {
        std::cout << " INGRESO SIN SESION \n";
        leerArchivo("Traducciones.txt", arbol);
        
    }

    int opcion;
    do
    {
        mostrarMenu();
        cin >> opcion;
        limpiarBuffer();

        switch (opcion)
        {
        case 1:
            arbol.mostrarTodoEnPantalla();
            break;
        case 2:
        {
            int tipoBusqueda;
            cout << "\n Tipo de busqueda:\n";
            cout << "0. Espanol a otro idioma\n";
            cout << "1. Italiano a espanol\n";
            cout << "2. Frances a espanol\n";
            cout << "3. Aleman a espanol\n";
            cout << "4. Ingles a espanol\n";
            cout << "Seleccione: ";
            cin >> tipoBusqueda;
            limpiarBuffer();

            string palabra;
            cout << "Ingrese la palabra: ";
            getline(cin, palabra);

            if (tipoBusqueda == 0)
            {
                int idiomaDestino;
                cout << "Seleccione un idioma destino (1: Italiano, 2: Frances, 3: Aleman, 4: Ingles): ";
                cin >> idiomaDestino;
                limpiarBuffer();
                arbol.buscarTraduccion(palabra, 0, idiomaDestino);
            }
            else
            {
                arbol.buscarTraduccion(palabra, tipoBusqueda, 0);
            }
            break;
        }
        case 3:
        {
            string esp, ita, fra, ale, ing;
            cout << "\n Agregar nueva palabra:\n";
            cout << "Ingrese la palabra en espanol: ";
            getline(cin, esp);
            cout << "Ingrese la traduccion en italiano: ";
            getline(cin, ita);
            cout << "Ingrese la traduccion en frances: ";
            getline(cin, fra);
            cout << "Ingrese la traduccion en aleman: ";
            getline(cin, ale);
            cout << "Ingrese la traduccion en ingles: ";
            getline(cin, ing);
            arbol.agregarPalabra(esp, ita, fra, ale, ing);
            cout << " Palabra agregada correctamente.\n";
            break;
        }
        case 4:
        {
            string palabra;
            cout << "\n Eliminar palabra:\n";
            cout << "Ingrese la palabra en espanol a eliminar: ";
            getline(cin, palabra);
            arbol.eliminarPalabra(palabra);
            break;
        }
        case 5:
            arbol.guardarEnArchivo();
            cout << "\n Cambios guardados correctamente.\n";
            break;
        case 6:
            system("cls");
            break;
        case 7:
            saveFileEncodeUser();
            cout << "\n Archivo guardado correctamente.\n";
            break;
        case 8:
            cout << "\n Saliendo del programa.\n";
            break;
        default:
            cout << "\n Opcion no valida.\n";
        }
    } while (opcion != 8);
}

void crearArchivo() {
    if (!usuarioActivo) {
        std::cout << "DEBES INICIAR SESION PRIMERO\n";
        return;
    }

    std::string contenido;
    std::cout << "\n------------------ CREAR EL ARCHIVO PROTEGIDO -------------------\n";
    std::cout << "ESCRIBE EL CONTENIDO DEL ARCHIVO";
    std::getline(std::cin, contenido);

    std::string codificado = codificarTexto(contenido);
    std::string firma = generarFirma(codificado);

    std::string nombreArchivo = usuarioActivo->nombre + "_archivo.seg";
    std::ofstream archivo(nombreArchivo);
    archivo << codificado << "\n" << firma;
    archivo.close();

    std::cout << "ARCHIVO GUARDADO COMO " << nombreArchivo << "\n";
}

void MenuSesion() {
    int opcion;
    do {
        std::cout << "\n=========== MENU PRINCIPAL ===========\n";
        std::cout << "1. CREAR USUARIO\n";
        std::cout << "2. INICIAR SESION\n";
        std::cout << "3. INICIAR SIN SESION\n";
        std::cout << "0. SALIR\n";
        std::cout << "SELECCIONE UNA OPCION: ";
        std::cin >> opcion;
        std::cin.ignore(); // PARA LIMPIAR EL BUFFER DEL ENTER

        switch (opcion) {
        case 1: crearUsuario(); break;
        case 2: iniciarSesion();  return; break;
        //case 3: crearArchivo(); break;
        case 3: sesion = true; return;
        //case 0: std::cout << "SALIENDO......\n"; break;
        default: std::cout << "OPCION INVALIDA.\n"; break;
        }
    } while (opcion != 0);

    //delete usuarioActivo;
    
}


int main()
{
    MenuSesion();
    if (usuarioActivo && sesion) {
        system("cls");        
        MenuArchivos();
    }
    else {
        if (sesion) {
            system("cls");
            MenuArchivos();
        }
        else {
            std::cout << "USUARIO O CLAVE INCORRECTOS.\n";            
        }
    }
    return 0;
}

