#pragma once
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>

enum class TipoError { LEXICO, SINTACTICO };
enum class Gravedad { ERR, CRITICO };

struct ErrorEntry {
    int numero;
    std::string lexema;
    TipoError tipo;
    std::string descripcion;
    int linea;
    int columna;
    Gravedad gravedad;
};

class ErrorManager {
public:
    void agregarError(const std::string& lexema, TipoError tipo,
                      const std::string& descripcion, int linea, int col,
                      Gravedad gravedad = Gravedad::ERR);
    bool hayErrores() const;
    const std::vector<ErrorEntry>& getErrores() const;
    void limpiar();
private:
    std::vector<ErrorEntry> errores;
    int contador = 0;
};