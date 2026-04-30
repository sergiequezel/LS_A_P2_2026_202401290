#include "Token.h"

std::string tokenTypeToString(Token_Type t) {
    switch(t) {
        case Token_Type::TABLERO: return "TABLERO";
        case Token_Type::COLUMNA: return "COLUMNA";
        case Token_Type::TAREA: return "tarea";
        case Token_Type::PRIORIDAD: return "prioridad";
        case Token_Type::RESPONSABLE: return "responsable";
        case Token_Type::FECHA_LIMITE: return "fecha_limite";
        case Token_Type::ALTA: return "ALTA";
        case Token_Type::MEDIA: return "MEDIA";
        case Token_Type::BAJA: return "BAJA";
        case Token_Type::CADENA: return "CADENA";
        case Token_Type::ENTERO: return "ENTERO";
        case Token_Type::FECHA: return "FECHA";
        case Token_Type::LLAVE_ABR: return "LLAVE_ABR";
        case Token_Type::LLAVE_CER: return "LLAVE_CER";
        case Token_Type::CORCHETE_ABR: return "CORCHETE_ABR";
        case Token_Type::CORCHETE_CER: return "CORCHETE_CER";
        case Token_Type::DOS_PUNTOS: return "DOS_PUNTOS";
        case Token_Type::COMA: return "COMA";
        case Token_Type::PUNTO_COMA: return "PUNTO_COMA";
        case Token_Type::FIN_ARCHIVO: return "FIN_ARCHIVO";
        case Token_Type::ERROR_LEXICO: return "ERROR_LEXICO";
        default: return "DESCONOCIDO";
    }
}