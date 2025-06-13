// MiniSQL — very small in‑memory SQL CLI prototype
// build: g++ -std=c++17 minisql.cpp -o minisql

#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

// simple table structure
struct Table {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
};

// trivial global DB container
static std::unordered_map<std::string, Table> gDatabase;

// base command
class SQLCommand {
public:
    virtual ~SQLCommand() = default;
    virtual void execute() = 0;
};

// CREATE TABLE name (col1,col2,..)
class CreateTableCmd : public SQLCommand {
    std::string name;
    std::vector<std::string> cols;
public:
    CreateTableCmd(std::string n, std::vector<std::string> c)
        : name(std::move(n)), cols(std::move(c)) {}
    void execute() override {
        if (gDatabase.count(name)) { std::cout << "Table already exists\n"; return; }
        gDatabase[name] = Table{cols, {}};
        std::cout << "OK\n";
    }
};

// INSERT INTO name VALUES (v1,v2,..)
class InsertCmd : public SQLCommand {
    std::string name;
    std::vector<std::string> values;
public:
    InsertCmd(std::string n, std::vector<std::string> v)
        : name(std::move(n)), values(std::move(v)) {}
    void execute() override {
        auto it = gDatabase.find(name);
        if (it == gDatabase.end()) { std::cout << "Table not found\n"; return; }
        if (values.size() != it->second.columns.size()) { std::cout << "Column mismatch\n"; return; }
        it->second.rows.push_back(values);
        std::cout << "OK\n";
    }
};

// SELECT * FROM name
class SelectCmd : public SQLCommand {
    std::string name;
public:
    explicit SelectCmd(std::string n) : name(std::move(n)) {}
    void execute() override {
        auto it = gDatabase.find(name);
        if (it == gDatabase.end()) { std::cout << "Table not found\n"; return; }
        // print header
        for (size_t i = 0; i < it->second.columns.size(); ++i) {
            std::cout << it->second.columns[i] << (i + 1 == it->second.columns.size() ? '\n' : '\t');
        }
        // print rows
        for (auto &row : it->second.rows) {
            for (size_t i = 0; i < row.size(); ++i) {
                std::cout << row[i] << (i + 1 == row.size() ? '\n' : '\t');
            }
        }
    }
};

// very naive parser (case‑insensitive keywords, no quotes)
class Parser {
public:
    static std::unique_ptr<SQLCommand> parse(const std::string &line) {
        std::istringstream ss(line);
        std::string tok;
        ss >> tok;
        std::transform(tok.begin(), tok.end(), tok.begin(), ::toupper);
        if (tok == "CREATE") {
            ss >> tok; // TABLE
            ss >> tok; // name
            std::string tableName = tok;
            char ch; ss >> ch; // '('
            std::vector<std::string> cols;
            while (ss >> tok) {
                bool end = false;
                if (tok.back() == ',') { tok.pop_back(); }
                if (tok.back() == ')') { tok.pop_back(); end = true; }
                cols.push_back(tok);
                if (end) break;
            }
            return std::make_unique<CreateTableCmd>(tableName, cols);
        }
        if (tok == "INSERT") {
            ss >> tok; // INTO
            ss >> tok; // table name
            std::string tableName = tok;
            ss >> tok; // VALUES
            char ch; ss >> ch; // '('
            std::vector<std::string> vals;
            std::string val;
            while (std::getline(ss, val, ',')) {
                val.erase(std::remove_if(val.begin(), val.end(), [](char c){ return c==' '||c=='('||c==')'; }), val.end());
                if (!val.empty()) vals.push_back(val);
                if (val.find(')') != std::string::npos) break;
            }
            return std::make_unique<InsertCmd>(tableName, vals);
        }
        if (tok == "SELECT") {
            ss >> tok; // *
            ss >> tok; // FROM
            ss >> tok; // table name
            return std::make_unique<SelectCmd>(tok);
        }
        if (tok == "EXIT" || tok == "QUIT") {
            return nullptr; // signals loop end
        }
        std::cout << "Unknown command\n";
        return std::make_unique<SelectCmd>(""); // no‑op
    }
};

int main() {
    std::cout << "MiniSQL> ";
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) { std::cout << "MiniSQL> "; continue; }
        auto cmd = Parser::parse(line);
        if (!cmd) break;
        cmd->execute();
        std::cout << "MiniSQL> ";
    }
    std::cout << "Bye!\n";
    return 0;
}
