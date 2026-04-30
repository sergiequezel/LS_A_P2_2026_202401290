#define UNICODE
#define _UNICODE

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <sstream>
// Tus headers
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "ReportGenerator.h"
#include "ErrorManager.h"
#include "Token.h"

// ===== Conversión segura =====
std::wstring toW(const std::string& s) {
    return std::wstring(s.begin(), s.end());
}

std::string wstringToString(const std::wstring& w) {
    return std::string(w.begin(), w.end());
}
// ===== Leer archivo =====
std::string leerArchivo(const std::wstring& path) {
    std::ifstream file(wstringToString(path));
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ===== Guardar archivo =====
void guardarArchivo(const std::wstring& path, const std::string& contenido) {
    std::ofstream file(wstringToString(path));
    file << contenido;
}

// ===== Abrir archivo (WinAPI) =====
std::wstring abrirDialogo() {
    OPENFILENAME ofn;
    wchar_t fileName[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"TaskScript (*.tsk)\0*.tsk\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        return fileName;
    }
    return L"";
}

// ===== Abrir HTML automáticamente =====
void abrirHTML(const std::wstring& path) {
    ShellExecute(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

// ===== MAIN =====
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // 1. Seleccionar archivo
    std::wstring path = abrirDialogo();

    if (path.empty()) {
        MessageBox(NULL, L"No se selecciono archivo.", L"Info", MB_OK);
        return 0;
    }

    // 2. Leer archivo
    std::string contenido = leerArchivo(path);

    // 3. Analisis lexico
    ErrorManager errMgr;
    LexicalAnalyzer lexer(contenido, errMgr);
    std::vector<Token> tokens = lexer.tokenizarTodo();

    // 4. Analisis sintactico
    SyntaxAnalyzer parser(tokens, errMgr);
    ProgramaNode prog = parser.parsear();

    // 5. Generar reportes
    ReportGenerator rep(prog, tokens, errMgr, parser.getNodosDot());

    std::string html1 = rep.generarReporte1();
    std::string html2 = rep.generarReporte2();
    std::string htmlTokens = rep.generarTablaTokens();
    std::string htmlErrores = rep.generarTablaErrores();
    std::string dot = rep.generarDot();

    // 6. Guardar archivos
    guardarArchivo(L"reporte1.html", html1);
    guardarArchivo(L"reporte2.html", html2);
    guardarArchivo(L"tokens.html", htmlTokens);
    guardarArchivo(L"errores.html", htmlErrores);
    guardarArchivo(L"arbol.dot", dot);

    // 7. Abrir resultados
    abrirHTML(L"reporte1.html");

    MessageBox(NULL, L"Reportes generados correctamente.", L"OK", MB_OK);

    return 0;
}