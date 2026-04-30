#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <fstream>
#include <sstream>

// Tus headers
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "ReportGenerator.h"
#include "ErrorManager.h"
#include "Token.h"

std::string wstringToString(const std::wstring &w)
{
    return std::string(w.begin(), w.end());
}

std::string leerArchivo(const std::wstring &path)
{
    std::ifstream file(wstringToString(path));

    if (!file.is_open())
    {
        MessageBoxW(NULL, L"No se pudo abrir el archivo.", L"Error", MB_OK);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void guardarArchivo(const std::wstring &path, const std::string &contenido)
{
    std::ofstream file(wstringToString(path));
    file << contenido;
}

std::wstring abrirDialogo()
{
    OPENFILENAMEW ofn;
    wchar_t fileName[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"TaskScript (*.tsk)\0*.tsk\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn))
        return fileName;

    return L"";
}

void abrirArchivo(const std::wstring &path)
{
    ShellExecuteW(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void generarImagenDOT(const std::wstring &dotPath, const std::wstring &imgPath)
{
    std::string cmd = "dot -Tpng " + wstringToString(dotPath) +
                      " -o " + wstringToString(imgPath);

    system(cmd.c_str());
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    std::wstring path = abrirDialogo();

    if (path.empty())
    {
        MessageBoxW(NULL, L"No se selecciono archivo.", L"Info", MB_OK);
        return 0;
    }

    std::string contenido = leerArchivo(path);
    if (contenido.empty()) return 0;

    ErrorManager errMgr;

    LexicalAnalyzer lexer(contenido, errMgr);
    std::vector<Token> tokens = lexer.tokenizarTodo();

    SyntaxAnalyzer parser(tokens, errMgr);
    ProgramaNode prog = parser.parsear();

    ReportGenerator rep(prog, tokens, errMgr, parser.getNodosDot());

    guardarArchivo(L"reporte1.html", rep.generarReporte1());
    guardarArchivo(L"reporte2.html", rep.generarReporte2());
    guardarArchivo(L"tokens.html", rep.generarTablaTokens());
    guardarArchivo(L"errores.html", rep.generarTablaErrores());
    guardarArchivo(L"arbol.dot", rep.generarDot());

    generarImagenDOT(L"arbol.dot", L"arbol.png");

    abrirArchivo(L"reporte1.html");
    abrirArchivo(L"reporte2.html");
    abrirArchivo(L"tokens.html");
    abrirArchivo(L"errores.html");
    abrirArchivo(L"arbol.png");

    MessageBoxW(NULL, L"Reportes generados correctamente.", L"OK", MB_OK);

    return 0;
}