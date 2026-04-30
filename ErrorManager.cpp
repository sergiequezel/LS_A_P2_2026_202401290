#include "ErrorManager.h"

void ErrorManager::agregarError(const std::string& lexema, TipoError tipo,
                                const std::string& descripcion, int linea, int col,
                                Gravedad gravedad) {
    errores.push_back({++contador, lexema, tipo, descripcion, linea, col, gravedad});
}

bool ErrorManager::hayErrores() const {
    return !errores.empty();
}

const std::vector<ErrorEntry>& ErrorManager::getErrores() const {
    return errores;
}

void ErrorManager::limpiar() {
    errores.clear();
    contador = 0;
}