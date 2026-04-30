#pragma once
#include "SyntaxAnalyzer.h"
#include "ErrorManager.h"
#include "Token.h"
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>

class ReportGenerator {
public:
    ReportGenerator(const ProgramaNode& prog,
                    const std::vector<Token>& tokens,
                    const ErrorManager& em,
                    const std::vector<NodoDot>& nodos);

    std::string generarReporte1() const;
    std::string generarReporte2() const;
    std::string generarTablaTokens() const;
    std::string generarTablaErrores() const;
    std::string generarDot() const;

private:
    const ProgramaNode& prog;
    const std::vector<Token>& tokens;
    const ErrorManager& em;
    const std::vector<NodoDot>& nodos;

    std::string htmlEscape(const std::string& s) const;
    std::string colorPrioridad(const std::string& p) const;
};