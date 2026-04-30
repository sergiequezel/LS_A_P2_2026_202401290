#pragma once
#include "Token.h"
#include "ErrorManager.h"
#include <string>
#include <vector>

class LexicalAnalyzer {
public:
    LexicalAnalyzer(const std::string& fuente, ErrorManager& em);
    Token nextToken();
    std::vector<Token> tokenizarTodo();
private:
    std::string src;
    int pos;
    int linea;
    int columna;
    ErrorManager& errMgr;

    char peek(int offset = 0);
    char consume();
    void skipWhitespaceAndComments();
    Token leerCadena();
    Token leerNumeroOFecha();
    Token leerIdentificadorOReservada();
    bool esFecha(const std::string& s);
    bool esDigito(char c);
    bool esLetra(char c);
    bool esAlnum(char c);
};