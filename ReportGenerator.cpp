#include "ReportGenerator.h"
#include <map>
#include <algorithm>
#include <string>
#include <sstream>

ReportGenerator::ReportGenerator(const ProgramaNode &p, const std::vector<Token> &t,
                                 const ErrorManager &e, const std::vector<NodoDot> &n)
    : prog(p), tokens(t), em(e), nodos(n) {}

std::string ReportGenerator::htmlEscape(const std::string &s) const
{
    std::string r;
    for (char c : s)
    {
        if (c == '<')
            r += "&lt;";
        else if (c == '>')
            r += "&gt;";
        else if (c == '&')
            r += "&amp;";
        else if (c == '"')
            r += "&quot;";
        else
            r += c;
    }
    return r;
}

std::string ReportGenerator::colorPrioridad(const std::string &p) const
{
    if (p == "ALTA")
        return "#e74c3c";
    if (p == "MEDIA")
        return "#f39c12";
    if (p == "BAJA")
        return "#27ae60";
    return "#95a5a6";
}

std::string ReportGenerator::generarReporte1() const
{
    std::ostringstream ss;
    ss << R"(<!DOCTYPE html><html lang="es"><head><meta charset="UTF-8">
<title>Tablero Kanban - )"
       << htmlEscape(prog.nombreTablero) << R"(</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:'Segoe UI',sans-serif;background:#1a1a2e;min-height:100vh;padding:20px}
.header{background:linear-gradient(135deg,#1565c0,#0d47a1);color:white;padding:20px 28px;
border-radius:12px;margin-bottom:24px;box-shadow:0 4px 16px rgba(0,0,0,0.3)}
.header h1{font-size:1.8rem;font-weight:700}
.header p{font-size:0.9rem;opacity:0.8;margin-top:4px}
.board{display:flex;gap:18px;overflow-x:auto;padding-bottom:12px}
.column{background:#16213e;border-radius:12px;min-width:280px;max-width:320px;
flex:1;box-shadow:0 2px 12px rgba(0,0,0,0.3)}
.col-header{background:linear-gradient(135deg,#0f3460,#1565c0);color:white;
padding:14px 16px;border-radius:12px 12px 0 0;font-weight:600;font-size:1rem}
.col-count{font-size:0.8rem;opacity:0.75;font-weight:400}
.col-body{padding:12px;display:flex;flex-direction:column;gap:10px}
.card{background:#0d1b2a;border-radius:8px;padding:14px;
border-left:4px solid #ccc;box-shadow:0 2px 6px rgba(0,0,0,0.2)}
.card-title{font-weight:600;color:#ecf0f1;font-size:0.95rem;margin-bottom:8px}
.badge{display:inline-block;color:white;font-size:0.72rem;font-weight:700;
padding:2px 8px;border-radius:12px;margin-bottom:6px;letter-spacing:0.5px}
.card-info{font-size:0.8rem;color:#95a5a6;margin-top:4px}
.card-info span{color:#bdc3c7}
.resp-chip{display:inline-block;background:#1e3a5f;color:#74b9ff;
font-size:0.78rem;padding:3px 10px;border-radius:20px;margin-top:6px}
</style></head><body>
<div class="header"><h1>)"
       << htmlEscape(prog.nombreTablero) << R"(</h1>
<p>Tablero Kanban generado por TaskScript</p></div>
<div class="board">)";

    for (const auto &col : prog.columnas)
    {
        ss << "<div class='column'>"
           << "<div class='col-header'>" << htmlEscape(col.nombre)
           << " <span class='col-count'>(" << col.tareas.size() << ")</span></div>"
           << "<div class='col-body'>";
        for (const auto &t : col.tareas)
        {
            std::string color = colorPrioridad(t.prioridad);
            ss << "<div class='card' style='border-left-color:" << color << "'>"
               << "<div class='card-title'>" << htmlEscape(t.nombre) << "</div>"
               << "<span class='badge' style='background:" << color << "'>"
               << htmlEscape(t.prioridad) << "</span>"
               << "<div class='card-info'>&#128197; <span>" << htmlEscape(t.fechaLimite) << "</span></div>"
               << "<div class='resp-chip'>&#128100; " << htmlEscape(t.responsable) << "</div>"
               << "</div>";
        }
        ss << "</div></div>";
    }
    ss << "</div></body></html>";
    return ss.str();
}

std::string ReportGenerator::generarReporte2() const
{
    struct Stats
    {
        int total = 0;
        int alta = 0;
        int media = 0;
        int baja = 0;
    };
    std::map<std::string, Stats> mapa;
    int totalGlobal = 0;
    for (const auto &col : prog.columnas)
    {
        for (const auto &t : col.tareas)
        {
            auto &s = mapa[t.responsable];
            s.total++;
            totalGlobal++;
            if (t.prioridad == "ALTA")
                s.alta++;
            else if (t.prioridad == "MEDIA")
                s.media++;
            else
                s.baja++;
        }
    }

    std::ostringstream ss;
    ss << R"(<!DOCTYPE html><html lang="es"><head><meta charset="UTF-8">
<title>Carga por Responsable</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:'Segoe UI',sans-serif;background:#1a1a2e;padding:24px;color:#ecf0f1}
.container{max-width:900px;margin:auto}
h1{font-size:1.6rem;margin-bottom:20px;color:#74b9ff}
table{width:100%;border-collapse:collapse;background:#16213e;border-radius:12px;overflow:hidden;
box-shadow:0 4px 16px rgba(0,0,0,0.3)}
thead{background:linear-gradient(135deg,#1565c0,#0d47a1)}
th{padding:14px 16px;text-align:left;font-size:0.85rem;font-weight:600;color:white;letter-spacing:0.5px}
td{padding:12px 16px;border-bottom:1px solid #0d1b2a;font-size:0.9rem}
tr:last-child td{border-bottom:none}
tr:hover td{background:#1e3a5f}
.bar-wrap{background:#0d1b2a;border-radius:20px;height:22px;position:relative;min-width:120px}
.bar-fill{height:22px;border-radius:20px;background:linear-gradient(90deg,#1565c0,#42a5f5);
display:flex;align-items:center;padding-left:8px;font-size:0.75rem;font-weight:700;color:white;
white-space:nowrap;min-width:36px;transition:width 0.4s}
.alta{color:#e74c3c;font-weight:700}
.media{color:#f39c12;font-weight:700}
.baja{color:#27ae60;font-weight:700}
.total{font-weight:700;color:#74b9ff}
</style></head><body><div class="container">
<h1>&#128202; Carga por Responsable</h1>
<table><thead><tr>
<th>Responsable</th><th>Total</th><th>Alta</th><th>Media</th><th>Baja</th><th>Distribucion</th>
</tr></thead><tbody>)";

    for (const auto &[nombre, s] : mapa)
    {
        int pct = totalGlobal > 0 ? (s.total * 100 / totalGlobal) : 0;
        ss << "<tr>"
           << "<td>" << htmlEscape(nombre) << "</td>"
           << "<td class='total'>" << s.total << "</td>"
           << "<td class='alta'>" << s.alta << "</td>"
           << "<td class='media'>" << s.media << "</td>"
           << "<td class='baja'>" << s.baja << "</td>"
           << "<td><div class='bar-wrap'><div class='bar-fill' style='width:" << pct << "%'>"
           << pct << "%</div></div></td>"
           << "</tr>";
    }
    ss << "</tbody></table></div></body></html>";
    return ss.str();
}

std::string ReportGenerator::generarTablaTokens() const
{
    std::ostringstream ss;
    ss << R"(<!DOCTYPE html><html lang="es"><head><meta charset="UTF-8">
<title>Tabla de Tokens</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:'Segoe UI',sans-serif;background:#1a1a2e;padding:24px;color:#ecf0f1}
h1{font-size:1.6rem;margin-bottom:20px;color:#74b9ff}
table{width:100%;border-collapse:collapse;background:#16213e;border-radius:12px;overflow:hidden;
box-shadow:0 4px 16px rgba(0,0,0,0.3)}
thead{background:linear-gradient(135deg,#1565c0,#0d47a1)}
th{padding:12px 14px;text-align:left;font-size:0.82rem;font-weight:600;color:white}
td{padding:10px 14px;border-bottom:1px solid #0d1b2a;font-size:0.85rem;font-family:monospace}
tr:hover td{background:#1e3a5f}
.err{color:#e74c3c}
</style></head><body>
<h1>&#128196; Tabla de Tokens</h1>
<table><thead><tr><th>#</th><th>Lexema</th><th>Tipo</th><th>Linea</th><th>Columna</th></tr></thead><tbody>)";

    int n = 1;
    for (const auto &t : tokens)
    {
        bool esErr = (t.tipo == Token_Type::ERROR_LEXICO);
        ss << "<tr" << (esErr ? " class='err'" : "") << ">"
           << "<td>" << n++ << "</td>"
           << "<td>" << htmlEscape(t.lexema) << "</td>"
           << "<td>" << tokenTypeToString(t.tipo) << "</td>"
           << "<td>" << t.linea << "</td>"
           << "<td>" << t.columna << "</td>"
           << "</tr>";
    }
    ss << "</tbody></table></body></html>";
    return ss.str();
}

std::string ReportGenerator::generarTablaErrores() const
{
    std::ostringstream ss;
    ss << R"(<!DOCTYPE html><html lang="es"><head><meta charset="UTF-8">
<title>Tabla de Errores</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:'Segoe UI',sans-serif;background:#1a1a2e;padding:24px;color:#ecf0f1}
h1{font-size:1.6rem;margin-bottom:20px;color:#e74c3c}
table{width:100%;border-collapse:collapse;background:#16213e;border-radius:12px;overflow:hidden;
box-shadow:0 4px 16px rgba(0,0,0,0.3)}
thead{background:linear-gradient(135deg,#7b1fa2,#4a148c)}
th{padding:12px 14px;text-align:left;font-size:0.82rem;font-weight:600;color:white}
td{padding:10px 14px;border-bottom:1px solid #0d1b2a;font-size:0.85rem}
tr:hover td{background:#1e0033}
.LEXICO{color:#f39c12}.SINTACTICO{color:#e74c3c}
.ERROR_G{color:#e74c3c;font-weight:700}.CRITICO_G{color:#c0392b;font-weight:700}
</style></head><body>
<h1>&#9888; Tabla de Errores</h1>
<table><thead><tr><th>#</th><th>Lexema</th><th>Tipo</th><th>Descripcion</th>
<th>Linea</th><th>Col</th><th>Gravedad</th></tr></thead><tbody>)";

    for (const auto &e : em.getErrores())
    {
        std::string tipoStr = (e.tipo == TipoError::LEXICO) ? "Lexico" : "Sintactico";
        std::string gravStr = (e.gravedad == Gravedad::CRITICO) ? "CRITICO" : "ERROR";
        std::string gravClass = (e.gravedad == Gravedad::CRITICO) ? "CRITICO_G" : "ERROR_G";
        ss << "<tr>"
           << "<td>" << e.numero << "</td>"
           << "<td><code>" << htmlEscape(e.lexema) << "</code></td>"
           << "<td class='" << tipoStr << "'>" << tipoStr << "</td>"
           << "<td>" << htmlEscape(e.descripcion) << "</td>"
           << "<td>" << e.linea << "</td>"
           << "<td>" << e.columna << "</td>"
           << "<td class='" << gravClass << "'>" << gravStr << "</td>"
           << "</tr>";
    }
    ss << "</tbody></table></body></html>";
    return ss.str();
}

std::string ReportGenerator::generarDot() const
{
    std::ostringstream ss;
    ss << "digraph ArbolDerivacion {\n"
       << "    rankdir=TB;\n"
       << "    node [shape=box, style=filled, fontname=\"Arial\"];\n";

    for (const auto &n : nodos)
    {
        std::string lbl = n.etiqueta;
        std::string fill = n.esTerminal ? "#D6EAF8" : "#2E75B6";
        std::string font = n.esTerminal ? "black" : "white";
        std::string lbl2;
        for (char c : lbl)
        {
            if (c == '"')
                lbl2 += "\\\"";
            else
                lbl2 += c;
        }
        ss << "    n" << n.id << " [label=\"" << lbl2
           << "\", fillcolor=\"" << fill << "\", fontcolor=" << font << "];\n";
    }

    for (const auto &n : nodos)
    {
        for (int hijo : n.hijos)
        {
            ss << "    n" << n.id << " -> n" << hijo << ";\n";
        }
    }
    ss << "}\n";
    return ss.str();
}