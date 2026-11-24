#include "storage.h"
#include "input.h"
#include "operations.h"
#include <fstream>
#include <sstream>
#include <limits>
using namespace std;
#include <string>
#include <vector>
#include "percentage.h"

bool saveToFile(const Group& g, const string& filename) {
    ofstream out(filename);
    if (!out) return false;
    
    out << "EXPENSE_SPLITTER_V3\n" << g.people.size() << "\n";
    for (const string& name : g.people) out << name << "\n";
    bool fixed = hasFixedPercent(g);
    out << (fixed ? "1" : "0") << "\n";
    if (fixed) {
        for (size_t i = 0; i < g.fixedWeight.size(); i++) {
            out << (i ? " " : "") << g.fixedWeight[i];
        }
        out << "\n";
    }
    out << g.budget << "\n" << g.expenses.size() << "\n";

    for (const Expense& e : g.expenses) {
        out << e.day << "\n" << e.description << "\n";
        out << e.payer << " " << e.amount << " " << static_cast<int>(e.category)
            << " " << e.sharedBy.size();
        for (int p : e.sharedBy) out << " " << p;
        out << " " << e.weights.size();
        for (int w : e.weights) out << " " << w;
        out << "\n";
    }
    out.flush();
    return static_cast<bool>(out);
}

static bool parseGroup(const string& data, bool categories, Group& result) {
    istringstream in(data);
    Group temp;

    int count;
    if (!(in >> count) || count < 1 || count > 50) return false;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < count; ++i) {
        string name;
        if (!getline(in, name)) return false;
        temp.people.push_back(trimLine(name));
    }
    int expenses;
    if (!(in >> expenses) || expenses < 0) return false;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < expenses; ++i) {
        Expense e = {};
        if (!(in >> e.day)) return false;
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        if (!getline(in, e.description)) return false;
        e.description = trimLine(e.description);
        string line;
        if (!getline(in, line)) return false;
        istringstream fields(line);
        int category = OTHER, shared;
        if (!(fields >> e.payer >> e.amount)) return false;
        if (categories && !(fields >> category)) return false;
        if (category < FOOD || category > OTHER || !(fields >> shared) || shared < 1 || shared > count) return false;
        e.category = static_cast<Category>(category);
        for (int k = 0; k < shared; ++k) {
            int person;
            if (!(fields >> person)) return false;
            e.sharedBy.push_back(person);
        }
        string extra;
        if (fields >> extra) return false;
        if (!appendExpense(temp, e)) return false;
    }
    in >> ws;
    if (!in.eof()) return false;
    result = temp;
    return true;
}

static bool parseOwn(const string& data, bool withCategory, Group& result) {
    istringstream in(data);
    Group temp;
    int count = 0;
    if (!(in >> count) || count < 1 || count > 50) return false;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < count; i++) {
        string name;
        if (!getline(in, name)) return false;
        temp.people.push_back(trimLine(name));
    }
    int hasFixed = 0;
    if (!(in >> hasFixed)) return false;
    if (hasFixed) {
        temp.fixedWeight.assign(count, 0);
        for (int& w : temp.fixedWeight) {
            if (!(in >> w)) return false;
        }
    }
    if (!(in >> temp.budget)) return false;
    int expenses;
    if (!(in >> expenses) || expenses < 0) return false;
    for (int i = 0; i < expenses; ++i) {
        Expense e = {};
        int shared = 0, weightCount = 0, category = OTHER;
        if (!(in >> e.day)) return false;
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        if (!getline(in, e.description)) return false;
        e.description = trimLine(e.description);
        if (!(in >> e.payer >> e.amount)) return false;
        if (withCategory && !(in >> category)) return false;
        if (category < FOOD || category > OTHER) return false;
        e.category = static_cast<Category>(category);
        if (!(in >> shared) || shared < 1 || shared > count) return false;
        for (int k = 0; k < shared; k++) {
            int p;
            if (!(in >> p)) return false;
            e.sharedBy.push_back(p);
        }
        if (!(in >> weightCount) || weightCount < 0 || weightCount > shared) return false;
        for (int k = 0; k < weightCount; k++) {
            int w;
            if (!(in >> w)) return false;
            e.weights.push_back(w);
        }
        if (weightCount != shared) e.weights.clear();
        if (!appendExpense(temp, e)) return false;
    }
    in >> ws;
    if (!in.eof()) return false;
    result = temp;
    return true;
}

bool loadFromFile(Group& g, const string& filename) {
    ifstream in(filename);
    if (!in) return false;
    string first;
    if (!getline(in, first)) return false;
    ostringstream rest;
    rest << in.rdbuf();
    string tail = rest.str();
    string head = trimLine(first);
    Group parsed;
    if (head == "EXPENSE_SPLITTER_V3") {
        if (!parseOwn(tail, true, parsed)) return false;
    } else if (head == "v2") {
        if (!parseOwn(tail, false, parsed)) return false;
    } else if (head == "EXPENSE_SPLITTER_V2") {
        if (!parseGroup(tail, true, parsed)) return false;
    } else {
        string data = first + "\n" + tail;
        Group legacy;
        bool categoryOk = parseGroup(data, true, parsed);
        bool legacyOk = parseGroup(data, false, legacy);
        // Unversioned formats can be ambiguous. Never silently change a bill's split.
        if (categoryOk && legacyOk && !parsed.expenses.empty()) return false;
        if (!categoryOk && !legacyOk) return false;
        if (!categoryOk) parsed = legacy;
    }
    g = parsed;
    return true;
}
