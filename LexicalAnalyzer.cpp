#include "LexicalAnalyzer.h"
#include <stdexcept>

LexicalAnalyzer::LexicalAnalyzer(const std::string& fuente, ErrorManager& em)
    : src(fuente), pos(0), linea(1), columna(1), errMgr(em) {}

char LexicalAnalyzer::peek(int offset) {
    int idx = pos + offset;
    if (idx < (int)src.size()) return src[idx];
    return '\0';
}

char LexicalAnalyzer::consume() {
    char c = src[pos++];
    if (c == '\n') { linea++; columna = 1; }
    else { columna++; }
    return c;
}

bool LexicalAnalyzer::esDigito(char c) { return c >= '0' && c <= '9'; }
bool LexicalAnalyzer::esLetra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
bool LexicalAnalyzer::esAlnum(char c) { return esLetra(c) || esDigito(c); }

void LexicalAnalyzer::skipWhitespaceAndComments() {
    while (pos < (int)src.size()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            consume();
        } else if (c == '/' && peek(1) == '/') {
            while (pos < (int)src.size() && peek() != '\n') consume();
        } else {
            break;
        }
    }
}

bool LexicalAnalyzer::esFecha(const std::string& s) {
    if (s.size() != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (s[i] != '-') return false;
        } else {
            if (!esDigito(s[i])) return false;
        }
    }
    int mes = std::stoi(s.substr(5, 2));
    int dia = std::stoi(s.substr(8, 2));
    return mes >= 1 && mes <= 12 && dia >= 1 && dia <= 31;
}

Token LexicalAnalyzer::leerCadena() {
    int lIni = linea, cIni = columna;
    consume();
    std::string val;
    bool cerrada = false;
    while (pos < (int)src.size()) {
        char c = peek();
        if (c == '"') { consume(); cerrada = true; break; }
        if (c == '\n') break;
        val += consume();
    }
    if (!cerrada) {
        errMgr.agregarError("\"" + val, TipoError::LEXICO,
            "Cadena no cerrada antes del fin de linea.", lIni, cIni, Gravedad::CRITICO);
        return {Token_Type::ERROR_LEXICO, "\"" + val, lIni, cIni};
    }
    return {Token_Type::CADENA, val, lIni, cIni};
}

Token LexicalAnalyzer::leerNumeroOFecha() {
    int lIni = linea, cIni = columna;
    std::string val;
    while (pos < (int)src.size() && (esDigito(peek()) || peek() == '-')) {
        char c = peek();
        if (c == '-' && val.size() != 4 && val.size() != 7) break;
        val += consume();
    }
    if (esFecha(val)) return {Token_Type::FECHA, val, lIni, cIni};
    bool soloDigitos = true;
    for (char ch : val) if (!esDigito(ch)) { soloDigitos = false; break; }
    if (soloDigitos) return {Token_Type::ENTERO, val, lIni, cIni};
    errMgr.agregarError(val, TipoError::LEXICO,
        "Literal numerico o de fecha invalido: '" + val + "'.", lIni, cIni);
    return {Token_Type::ERROR_LEXICO, val, lIni, cIni};
}

Token LexicalAnalyzer::leerIdentificadorOReservada() {
    int lIni = linea, cIni = columna;
    std::string val;
    while (pos < (int)src.size() && esAlnum(peek())) val += consume();

    if (val == "TABLERO")     return {Token_Type::TABLERO,      val, lIni, cIni};
    if (val == "COLUMNA")     return {Token_Type::COLUMNA,      val, lIni, cIni};
    if (val == "tarea")       return {Token_Type::TAREA,        val, lIni, cIni};
    if (val == "prioridad")   return {Token_Type::PRIORIDAD,    val, lIni, cIni};
    if (val == "responsable") return {Token_Type::RESPONSABLE,  val, lIni, cIni};
    if (val == "fecha_limite")return {Token_Type::FECHA_LIMITE, val, lIni, cIni};
    if (val == "ALTA")        return {Token_Type::ALTA,         val, lIni, cIni};
    if (val == "MEDIA")       return {Token_Type::MEDIA,        val, lIni, cIni};
    if (val == "BAJA")        return {Token_Type::BAJA,         val, lIni, cIni};

    errMgr.agregarError(val, TipoError::LEXICO,
        "Identificador desconocido: '" + val + "'.", lIni, cIni);
    return {Token_Type::ERROR_LEXICO, val, lIni, cIni};
}

Token LexicalAnalyzer::nextToken() {
    skipWhitespaceAndComments();
    if (pos >= (int)src.size()) return {Token_Type::FIN_ARCHIVO, "", linea, columna};

    int lIni = linea, cIni = columna;
    char c = peek();

    if (c == '"') return leerCadena();
    if (esDigito(c)) return leerNumeroOFecha();
    if (esLetra(c)) return leerIdentificadorOReservada();

    consume();
    switch (c) {
        case '{': return {Token_Type::LLAVE_ABR,     "{", lIni, cIni};
        case '}': return {Token_Type::LLAVE_CER,     "}", lIni, cIni};
        case '[': return {Token_Type::CORCHETE_ABR,  "[", lIni, cIni};
        case ']': return {Token_Type::CORCHETE_CER,  "]", lIni, cIni};
        case ':': return {Token_Type::DOS_PUNTOS,    ":", lIni, cIni};
        case ',': return {Token_Type::COMA,          ",", lIni, cIni};
        case ';': return {Token_Type::PUNTO_COMA,    ";", lIni, cIni};
        default: break;
    }

    std::string bad(1, c);
    errMgr.agregarError(bad, TipoError::LEXICO,
        std::string("Caracter no reconocido '") + c + "'.", lIni, cIni);
    return {Token_Type::ERROR_LEXICO, bad, lIni, cIni};
}

std::vector<Token> LexicalAnalyzer::tokenizarTodo() {
    std::vector<Token> tokens;
    while (true) {
        Token t = nextToken();
        tokens.push_back(t);
        if (t.tipo == Token_Type::FIN_ARCHIVO) break;
    }
    return tokens;
}