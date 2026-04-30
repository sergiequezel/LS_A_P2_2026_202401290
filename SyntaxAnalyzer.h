#pragma once
#include "Token.h"
#include "ErrorManager.h"
#include <string>
#include <vector>
#include <memory>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>

struct AtributoNode {
    std::string nombre;
    std::string valor;
};

struct TareaNode {
    std::string nombre;
    std::string prioridad;
    std::string responsable;
    std::string fechaLimite;
};

struct ColumnaNode {
    std::string nombre;
    std::vector<TareaNode> tareas;
};

struct ProgramaNode {
    std::string nombreTablero;
    std::vector<ColumnaNode> columnas;
    bool valido = false;
};

struct NodoDot {
    int id;
    std::string etiqueta;
    bool esTerminal;
    std::vector<int> hijos;
};

class SyntaxAnalyzer {
public:
    SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& em);
    ProgramaNode parsear();
    const std::vector<NodoDot>& getNodosDot() const;

private:
    std::vector<Token> tokens;
    int pos;
    ErrorManager& errMgr;
    std::vector<NodoDot> nodosDot;
    int contadorNodos;

    Token& actual();
    Token& siguiente(int offset = 1);
    bool match(Token_Type tipo);
    Token consume(Token_Type tipo, const std::string& esperado);
    bool esFin();
    void sincronizar(const std::vector<Token_Type>& conjunto);

    int nuevoNodo(const std::string& etiqueta, bool esTerminal = false);
    int nodoTerminal(const Token& t);

    ProgramaNode parsePrograma();
    ColumnaNode parseColumna(int padreId);
    TareaNode parseTarea(int padreId);
    void parseAtributos(TareaNode& tarea, int padreId);
    void parseAtributo(TareaNode& tarea, int padreId);
};