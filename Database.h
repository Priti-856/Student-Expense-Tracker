// ===================== DBMS CONCEPTS =====================
// Tiny file-backed database: table, primary key (auto-increment), CRUD, aggregates.

#include "Record.h"
#include <map>
#include <fstream>
#include <vector>

class Database {
    map<int, unique_ptr<Transaction>> table;   // PRIMARY KEY (id) -> row
    string filename;
    int nextId = 1;                            // AUTO_INCREMENT

    void save() {                              // PERSISTENCE
        ofstream f(filename);
        for (auto& kv : table) f << kv.second->toCSV() << "\n";
    }
    void load() {
        ifstream f(filename);
        string line;
        while (getline(f, line))
            if (!line.empty()) {
                auto t = Transaction::fromCSV(line);
                nextId = max(nextId, t->key() + 1);
                table[t->key()] = move(t);
            }
    }
public:
    Database(const string& file) : filename(file) { load(); }

    // INSERT
    int insert(bool isIncome, const string& cat, double amt, const string& date) {
        int id = nextId++;
        if (isIncome) table[id] = make_unique<Income>(id, cat, amt, date);
        else          table[id] = make_unique<Expense>(id, cat, amt, date);
        save();
        return id;
    }
    // SELECT *
    vector<const Transaction*> selectAll() const {
        vector<const Transaction*> v;
        for (auto& kv : table) v.push_back(kv.second.get());
        return v;
    }
    // SELECT ... WHERE category = ?
    vector<const Transaction*> byCategory(const string& cat) const {
        vector<const Transaction*> v;
        for (auto& kv : table) if (kv.second->getCategory() == cat) v.push_back(kv.second.get());
        return v;
    }
    // UPDATE
    bool update(int id, double amt) {
        auto it = table.find(id);
        if (it == table.end()) return false;
        it->second->setAmount(amt); save(); return true;
    }
    // DELETE
    bool remove(int id) {
        bool ok = table.erase(id) > 0;
        if (ok) save();
        return ok;
    }
    // AGGREGATE: SUM(), GROUP BY category
    string summary() const {
        double inc = 0, exp = 0;
        map<string, double> perCat;
        for (auto& [k, t] : table) {
            double s = t->signedAmount();
            if (s >= 0) inc += s;
            else { exp += -s; perCat[t->getCategory()] += -s; }
        }
        string out = "Total Income  : Rs." + money(inc) + "\n" +
                     "Total Expense : Rs." + money(exp) + "\n" +
                     "Balance       : Rs." + money(inc - exp) + "\n";
        if (!perCat.empty()) {
            out += "Expense by category:\n";
            for (auto& cv : perCat) out += "  " + cv.first + " : Rs." + money(cv.second) + "\n";
        }
        return out;
    }
};
