#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>

#include <string>

enum class Token_Type{
    TABLERO, COLUMNA, TAREA, PRIORIDAD, RESPONSABLE, FECHA_LIMITE,
    ALTA, MEDIA, BAJA,
    CADENA, ENTERO, FECHA,
    LLAVE_ABR, LLAVE_CER, CORCHETE_ABR, CORCHETE_CER,
    DOS_PUNTOS, COMA, PUNTO_COMA,
    FIN_ARCHIVO,
    ERROR_LEXICO
};

struct Token {
    Token_Type tipo;
    std::string lexema;
    int linea;
    int columna;
};

std::string tokenTypeToString(Token_Type t);