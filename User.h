// ===================== LOGIN (simple) =====================
// Stores username and hashed password in users.db
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <cctype>
using namespace std;

class UserManager {
    map<string, size_t> users;        // username -> password hash
    string filename = "users.db";

    size_t hashIt(const string& pass) const {   // password is never stored as plain text
        return hash<string>{}(pass);
    }
    void save() {
        ofstream f(filename);
        for (auto& uh : users) f << uh.first << " " << uh.second << "\n";
    }
    void load() {
        ifstream f(filename);
        string u; size_t h;
        while (f >> u >> h) users[u] = h;
    }
public:
    UserManager() { load(); }

    static bool validName(const string& u) {    // only letters/digits allowed
        if (u.empty()) return false;
        for (char c : u) if (!isalnum((unsigned char)c)) return false;
        return true;
    }
    // returns 0=ok, 1=invalid name, 2=already exists
    int registerUser(const string& u, const string& pass) {
        if (!validName(u) || pass.empty()) return 1;
        if (users.count(u)) return 2;
        users[u] = hashIt(pass);
        save();
        return 0;
    }
    bool login(const string& u, const string& pass) const {
        auto it = users.find(u);
        return it != users.end() && it->second == hashIt(pass);
    }
};
