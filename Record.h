// ===================== OOP CONCEPTS =====================
#include<iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <memory>
using namespace std;

string money(double x) {
    ostringstream o; o << fixed << setprecision(2) << x; return o.str();
}

// ABSTRACTION: interface that every database row must follow
class Record {
public:
    virtual string toCSV() const = 0;      // pure virtual
    virtual string describe() const = 0;   // pure virtual
    virtual int key() const = 0;           // primary key
    virtual ~Record() {}
};

// INHERITANCE: Record -> Transaction -> Income / Expense
class Transaction : public Record {
protected:                                 // ENCAPSULATION: data hidden from outside
    int id;
    string category;
    double amount;
    string date;
public:
    Transaction(int i, const string& c, double a, const string& d)
        : id(i), category(c), amount(a), date(d) {}

    int key() const override { return id; }
    string getCategory() const { return category; }
    void setAmount(double a) { amount = a; }

    virtual double signedAmount() const = 0;   // + for income, - for expense
    virtual string type() const = 0;

    string toCSV() const override {
        return type() + "," + to_string(id) + "," + category + "," + money(amount) + "," + date;
    }
    string describe() const override {
        return "#" + to_string(id) + " | " + date + " | " + type() + " | " +
               category + " | " + (signedAmount() < 0 ? "-" : "+") + "Rs." + money(amount);
    }
    // Build the correct object from a CSV line (factory)
    static unique_ptr<Transaction> fromCSV(const string& line);
};

class Income : public Transaction {
public:
    using Transaction::Transaction;
    // POLYMORPHISM: same function, different behaviour
    double signedAmount() const override { return amount; }
    string type() const override { return "INCOME"; }
};

class Expense : public Transaction {
public:
    using Transaction::Transaction;
    double signedAmount() const override { return -amount; }
    string type() const override { return "EXPENSE"; }
};

unique_ptr<Transaction> Transaction::fromCSV(const string& line) {
    stringstream ss(line);
    string t, id, cat, amt, date;
    getline(ss, t, ','); getline(ss, id, ','); getline(ss, cat, ',');
    getline(ss, amt, ','); getline(ss, date, ',');
    if (t == "INCOME") return make_unique<Income>(stoi(id), cat, stod(amt), date);
    return make_unique<Expense>(stoi(id), cat, stod(amt), date);
}
