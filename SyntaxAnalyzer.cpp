#include "SyntaxAnalyzer.h"
#include <stdexcept>

SyntaxAnalyzer::SyntaxAnalyzer(const std::vector<Token>& toks, ErrorManager& em)
    : tokens(toks), pos(0), errMgr(em), contadorNodos(0) {}

Token& SyntaxAnalyzer::actual() { return tokens[pos]; }

Token& SyntaxAnalyzer::siguiente(int offset) {
    int idx = pos + offset;
    if (idx < (int)tokens.size()) return tokens[idx];
    return tokens.back();
}

bool SyntaxAnalyzer::match(Token_Type tipo) {
    return actual().tipo == tipo;
}

bool SyntaxAnalyzer::esFin() {
    return match(Token_Type::FIN_ARCHIVO);
}

Token SyntaxAnalyzer::consume(Token_Type tipo, const std::string& esperado) {
    if (match(tipo)) {
        Token t = actual();
        pos++;
        return t;
    }
    errMgr.agregarError(actual().lexema, TipoError::SINTACTICO,
        "Se esperaba " + esperado + ", se encontro '" + actual().lexema + "'.",
        actual().linea, actual().columna);
    return {tipo, "", actual().linea, actual().columna};
}

void SyntaxAnalyzer::sincronizar(const std::vector<Token_Type>& conjunto) {
    while (!esFin()) {
        for (auto& t : conjunto) {
            if (match(t)) return;
        }
        pos++;
    }
}

int SyntaxAnalyzer::nuevoNodo(const std::string& etiqueta, bool esTerminal) {
    int id = contadorNodos++;
    nodosDot.push_back({id, etiqueta, esTerminal, {}});
    return id;
}

int SyntaxAnalyzer::nodoTerminal(const Token& t) {
    std::string lbl = t.lexema.empty() ? tokenTypeToString(t.tipo) : t.lexema;
    return nuevoNodo(lbl, true);
}

const std::vector<NodoDot>& SyntaxAnalyzer::getNodosDot() const {
    return nodosDot;
}

ProgramaNode SyntaxAnalyzer::parsear() {
    return parsePrograma();
}

ProgramaNode SyntaxAnalyzer::parsePrograma() {
    ProgramaNode prog;
    int nId = nuevoNodo("<programa>");

    if (!match(Token_Type::TABLERO)) {
        errMgr.agregarError(actual().lexema, TipoError::SINTACTICO,
            "Se esperaba 'TABLERO' al inicio del programa.", actual().linea, actual().columna);
        sincronizar({Token_Type::FIN_ARCHIVO});
        return prog;
    }
    Token tTablero = actual(); pos++;
    int nTablero = nodoTerminal(tTablero);
    nodosDot[nId].hijos.push_back(nTablero);

    Token tNombre = consume(Token_Type::CADENA, "nombre del tablero (cadena)");
    int nNombre = nodoTerminal(tNombre);
    nodosDot[nId].hijos.push_back(nNombre);
    prog.nombreTablero = tNombre.lexema;

    Token tLlaveAbr = consume(Token_Type::LLAVE_ABR, "'{'");
    int nLlaveAbr = nodoTerminal(tLlaveAbr);
    nodosDot[nId].hijos.push_back(nLlaveAbr);

    int nColumnas = nuevoNodo("<columnas>");
    nodosDot[nId].hijos.push_back(nColumnas);

    while (!esFin() && match(Token_Type::COLUMNA)) {
        ColumnaNode col = parseColumna(nColumnas);
        prog.columnas.push_back(col);
    }

    Token tLlaveCer = consume(Token_Type::LLAVE_CER, "'}'");
    int nLlaveCer = nodoTerminal(tLlaveCer);
    nodosDot[nId].hijos.push_back(nLlaveCer);

    Token tPuntoComa = consume(Token_Type::PUNTO_COMA, "';'");
    int nPuntoComa = nodoTerminal(tPuntoComa);
    nodosDot[nId].hijos.push_back(nPuntoComa);

    prog.valido = !errMgr.hayErrores();
    return prog;
}

ColumnaNode SyntaxAnalyzer::parseColumna(int padreId) {
    ColumnaNode col;
    int nId = nuevoNodo("<columna>");
    nodosDot[padreId].hijos.push_back(nId);

    Token tCol = actual(); pos++;
    int nCol = nodoTerminal(tCol);
    nodosDot[nId].hijos.push_back(nCol);

    Token tNombre = consume(Token_Type::CADENA, "nombre de columna (cadena)");
    int nNombre = nodoTerminal(tNombre);
    nodosDot[nId].hijos.push_back(nNombre);
    col.nombre = tNombre.lexema;

    consume(Token_Type::LLAVE_ABR, "'{'");
    int nLlaveAbr = nuevoNodo("{", true);
    nodosDot[nId].hijos.push_back(nLlaveAbr);

    int nTareas = nuevoNodo("<tareas>");
    nodosDot[nId].hijos.push_back(nTareas);

    bool primera = true;
    while (!esFin() && !match(Token_Type::LLAVE_CER)) {
        if (!primera) {
            if (match(Token_Type::COMA)) { pos++; }
            else if (!match(Token_Type::TAREA)) break;
        }
        if (match(Token_Type::TAREA)) {
            TareaNode t = parseTarea(nTareas);
            col.tareas.push_back(t);
            primera = false;
        } else {
            errMgr.agregarError(actual().lexema, TipoError::SINTACTICO,
                "Se esperaba 'tarea', se encontro '" + actual().lexema + "'.",
                actual().linea, actual().columna);
            sincronizar({Token_Type::LLAVE_CER, Token_Type::TAREA, Token_Type::COLUMNA});
            break;
        }
    }

    consume(Token_Type::LLAVE_CER, "'}'");
    int nLlaveCer = nuevoNodo("}", true);
    nodosDot[nId].hijos.push_back(nLlaveCer);

    consume(Token_Type::PUNTO_COMA, "';'");
    int nPS = nuevoNodo(";", true);
    nodosDot[nId].hijos.push_back(nPS);

    return col;
}

TareaNode SyntaxAnalyzer::parseTarea(int padreId) {
    TareaNode tarea;
    int nId = nuevoNodo("<tarea>");
    nodosDot[padreId].hijos.push_back(nId);

    Token tTarea = actual(); pos++;
    int nTarea = nodoTerminal(tTarea);
    nodosDot[nId].hijos.push_back(nTarea);

    consume(Token_Type::DOS_PUNTOS, "':'");
    int nDP = nuevoNodo(":", true);
    nodosDot[nId].hijos.push_back(nDP);

    Token tNombre = consume(Token_Type::CADENA, "nombre de tarea (cadena)");
    int nNombre = nodoTerminal(tNombre);
    nodosDot[nId].hijos.push_back(nNombre);
    tarea.nombre = tNombre.lexema;

    consume(Token_Type::CORCHETE_ABR, "'['");
    int nCAb = nuevoNodo("[", true);
    nodosDot[nId].hijos.push_back(nCAb);

    parseAtributos(tarea, nId);

    consume(Token_Type::CORCHETE_CER, "']'");
    int nCCer = nuevoNodo("]", true);
    nodosDot[nId].hijos.push_back(nCCer);

    return tarea;
}

void SyntaxAnalyzer::parseAtributos(TareaNode& tarea, int padreId) {
    int nId = nuevoNodo("<atributos>");
    nodosDot[padreId].hijos.push_back(nId);

    parseAtributo(tarea, nId);
    while (match(Token_Type::COMA)) {
        pos++;
        if (match(Token_Type::PRIORIDAD) || match(Token_Type::RESPONSABLE) || match(Token_Type::FECHA_LIMITE)) {
            parseAtributo(tarea, nId);
        } else {
            break;
        }
    }
}

void SyntaxAnalyzer::parseAtributo(TareaNode& tarea, int padreId) {
    int nId = nuevoNodo("<atributo>");
    nodosDot[padreId].hijos.push_back(nId);

    if (match(Token_Type::PRIORIDAD)) {
        Token tP = actual(); pos++;
        int nP = nodoTerminal(tP);
        nodosDot[nId].hijos.push_back(nP);

        consume(Token_Type::DOS_PUNTOS, "':'");
        int nDP = nuevoNodo(":", true);
        nodosDot[nId].hijos.push_back(nDP);

        if (match(Token_Type::ALTA) || match(Token_Type::MEDIA) || match(Token_Type::BAJA)) {
            Token tVal = actual(); pos++;
            int nVal = nodoTerminal(tVal);
            nodosDot[nId].hijos.push_back(nVal);
            tarea.prioridad = tVal.lexema;
        } else {
            errMgr.agregarError(actual().lexema, TipoError::SINTACTICO,
                "Se esperaba ALTA, MEDIA o BAJA, se encontro '" + actual().lexema + "'.",
                actual().linea, actual().columna);
        }
    } else if (match(Token_Type::RESPONSABLE)) {
        Token tR = actual(); pos++;
        int nR = nodoTerminal(tR);
        nodosDot[nId].hijos.push_back(nR);

        consume(Token_Type::DOS_PUNTOS, "':'");
        int nDP = nuevoNodo(":", true);
        nodosDot[nId].hijos.push_back(nDP);

        Token tVal = consume(Token_Type::CADENA, "nombre del responsable (cadena)");
        int nVal = nodoTerminal(tVal);
        nodosDot[nId].hijos.push_back(nVal);
        tarea.responsable = tVal.lexema;
    } else if (match(Token_Type::FECHA_LIMITE)) {
        Token tF = actual(); pos++;
        int nF = nodoTerminal(tF);
        nodosDot[nId].hijos.push_back(nF);

        consume(Token_Type::DOS_PUNTOS, "':'");
        int nDP = nuevoNodo(":", true);
        nodosDot[nId].hijos.push_back(nDP);

        Token tVal = consume(Token_Type::FECHA, "fecha en formato AAAA-MM-DD");
        int nVal = nodoTerminal(tVal);
        nodosDot[nId].hijos.push_back(nVal);
        tarea.fechaLimite = tVal.lexema;
    } else {
        errMgr.agregarError(actual().lexema, TipoError::SINTACTICO,
            "Se esperaba un atributo (prioridad, responsable, fecha_limite), se encontro '" + actual().lexema + "'.",
            actual().linea, actual().columna);
        sincronizar({Token_Type::COMA, Token_Type::CORCHETE_CER});
    }
}