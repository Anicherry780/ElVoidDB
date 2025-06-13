#include "Parser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>        //  isspace

namespace astrodb {

/* ── helper: remove optional trailing semicolon ─────────────── */
static void stripSemicolon(std::string& s)
{
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
        s.pop_back();
    if (!s.empty() && s.back() == ';')
        s.pop_back();
}

/* ── Parser core ────────────────────────────────────────────── */
void Parser::upper(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

std::unique_ptr<SQLCommand> Parser::parse(const std::string& line)
{
    std::string buf = line;
    stripSemicolon(buf);                          // <─ NEW
    std::istringstream ss(buf);

    std::string tok; ss >> tok; upper(tok);

    /* CREATE TABLE name (col1,col2,…) */
    if (tok == "CREATE") {
        ss >> tok; upper(tok);
        if (tok != "TABLE") throw ParseError("expected TABLE after CREATE");

        std::string name; ss >> name; char ch; ss >> ch;    // '('
        std::vector<std::string> cols; std::string col;

        while (std::getline(ss, col, ',')) {
            col.erase(std::remove_if(col.begin(), col.end(),
                     [](char c){ return c==' '||c=='('||c==')'; }), col.end());
            if (!col.empty()) cols.push_back(col);
            if (col.find(')') != std::string::npos) break;
        }
        return std::make_unique<CreateTableCmd>(name, cols);
    }

    /* INSERT INTO name VALUES (…) */
    if (tok == "INSERT") {
        ss >> tok;                                 // INTO
        std::string name; ss >> name;
        ss >> tok;                                 // VALUES
        if (tok.rfind("VALUES",0) != 0) throw ParseError("expected VALUES");
        char ch;
        if (tok.size() == 6) {          // "VALUES" exactly → read next char
            ss >> ch;                   // '('
        } else {                        // token was "VALUES(..."
            ch = tok[6];                // first char after "VALUES"
            tok.erase(0,7);             // keep remainder inside stream buffer
            ss.putback(' ');            // fake a space so getline sees it
            for (auto it = tok.rbegin(); it!=tok.rend(); ++it) ss.putback(*it);
        }
        std::vector<std::string> vals; std::string val;

        while (std::getline(ss, val, ',')) {
            val.erase(std::remove_if(val.begin(), val.end(),
                     [](char c){ return c==' '||c=='('||c==')'; }), val.end());
            if (!val.empty()) vals.push_back(val);
            if (val.find(')') != std::string::npos) break;
        }
        return std::make_unique<InsertCmd>(name, vals);
    }

    /* SELECT * FROM name */
    if (tok == "SELECT") {
        ss >> tok;                                 // *
        ss >> tok;                                 // FROM
        std::string name; ss >> name;
        return std::make_unique<SelectCmd>(name);
    }

    /* EXIT / QUIT */
    if (tok == "EXIT" || tok == "QUIT") return nullptr;

    throw ParseError("unknown command");
}

} // namespace astrodb
