#include "group.h"
#include "operations.h"
#include <iostream>
#include <sstream>
#include "input.h"
#include <string>
#include <vector>
#include "budget.h"
#include "percentage.h"
using namespace std;

static string categoryName(Category c) {
    switch (c) {
        case FOOD: return "Food";
        case TRANSPORT: return "Transport";
        case ACCOMMODATION: return "Accommodation";
        case ENTERTAINMENT: return "Entertainment";
        default: return "Other";
    }
}


void setupPeople(Group& g) {
    g.people.clear();
    g.expenses.clear();

    g.fixedWeight.clear();
    g.budget = -1;

    int count = readInt("How many people are in the group? ", 1, 50);
    for (int i = 0; i < count; i++) {
        string name = readText("Name of person " + to_string(i + 1) + ": ");
        if (name == "") name = "Person " + to_string(i + 1);
        g.people.push_back(name);
    }

    if (readYesNo("Set a budget now?")) {
        g.budget = readAmount("Budget total: ");
        showBudgetLine(g);
    }
    if (readYesNo("Set initial percentage split now?")) {
        g.fixedWeight = askPercentages(g);
        cout << "Fixed percentages saved:\n";
        printPercentages(g);
    }
}


void printPeople(const Group& g) {
    for (int i = 0; i < (int)g.people.size(); i++) {
        cout << "  " << i + 1 << ". " << g.people[i] << "\n";
    }
}

static vector<int> readPeopleList(const Group& g) {
    int n = g.people.size();
    while (true) {
        string line = readText("Enter the numbers of the people sharing it (e.g. 1 3): ");
        stringstream ss(line);
        vector<bool> picked(n, false);
        int number;
        bool ok = true;

        while (ss >> number) {
            if (number < 1 || number > n) {
                ok = false;
                break;
            }
            picked[number - 1] = true;
        }
        if (!ss.eof()) ok = false;

        vector<int> result;
        for (int i = 0; i < n; i++) {
            if (picked[i]) result.push_back(i);
        }
        if (ok && !result.empty()) return result;
        cout << "Please enter numbers from 1 to " << n << ", separated by spaces.\n";
    }
}

void addExpense(Group& g) {
    Expense e;
    e.day = readInt("Day number (e.g. 1): ", 1, 365);
    e.description = readText("What was it for? ");
    if (e.description == "") e.description = "(no description)";

    printPeople(g);
    e.payer = readInt("Who paid? (enter their number): ", 1, g.people.size()) - 1;
    e.amount = readAmount("Amount: ");

    cout << "\nSelect category:\n";
    cout << "1. Food\n";
    cout << "2. Transport\n";
    cout << "3. Accommodation\n";
    cout << "4. Entertainment\n";
    cout << "5. Other\n";
    
    int categoryChoice = readInt("Category: ", 1, 5);
    
    e.category = static_cast<Category>(categoryChoice - 1);

    bool useFixed = hasFixedPercent(g);
    cout << "How to split?  1. fixed %  (" << (useFixed ? "available" : "none set")
         << ") 2. even  3. custom %\n";
    int mode = readInt("Choose: ", 1, 3);
    if (mode == 1 && !useFixed) {
        cout << "No fixed percentages set - Using even split.\n";
        mode = 2;
    }
    if (mode == 1) {
      applyFixedWeights(g, e);
    } else if (mode == 2) {
        if (readYesNo("Split between everyone?")) {
            e.sharedBy.clear();
            for (int i = 0; i < (int)g.people.size(); i++) e.sharedBy.push_back(i);
        } else {
            e.sharedBy = readPeopleList(g);
        }
    } else {
        applyCustomWeights(g, e);
    }
    cout << "\nExpense added.\n";
    reportAfterExpense(g);
    if (!appendExpense(g, e)) { cout << "Invalid expense.\n"; return; }
    cout << "Split: ";
    for (size_t k = 0; k < e.sharedBy.size(); k++) {
        cout << (k ? ", " : "") << g.people[e.sharedBy[k]] << " " << money(shareOf(e, e.sharedBy[k]));
    }
    cout << "\nExpense added.\n";
    reportAfterExpense(g);
}

void listExpenses(const Group& g) {
    if (g.expenses.empty()) {
        cout << "No expenses yet.\n";
        return;
    }
    int total = 0;
    for (int i = 0; i < (int)g.expenses.size(); i++) {
        const Expense& e = g.expenses[i];
        cout << "#" << i + 1 << "  Day " << e.day <<"  [" << categoryName(e.category) << "] "<< "  " << e.description << "  - "
             << g.people[e.payer] << " paid " << money(e.amount) << "  (shared by: ";
        if (e.sharedBy.size() == g.people.size()) {
            cout << "everyone";
        } else {
            for (int k = 0; k < (int)e.sharedBy.size(); k++) {
                if (k > 0) cout << ", ";
                cout << g.people[e.sharedBy[k]];
            }
        }
        if ((int)e.weights.size() == (int)e.sharedBy.size() && !e.weights.empty()) cout << " | %";
        cout << ")\n";
        total += e.amount;
    }
    cout << "Total spent: " << money(total) << "\n";
    showBudgetLine(g);
}

void removeExpense(Group& g) {
    if (g.expenses.empty()) {
        cout << "No expenses to remove.\n";
        return;
    }
    listExpenses(g);
    int number = readInt("Which expense to remove? (0 = cancel): ", 0, g.expenses.size());
    if (number == 0) return;
    eraseExpense(g, number - 1);
    cout << "Expense #" << number << " removed.\n";

    showBudgetLine(g);
}


void editExpense(Group& g) {
    if (g.expenses.empty()) {
        cout << "No expenses to edit.\n";
        return;
    }
    listExpenses(g);
    int number = readInt("Which expense to edit? (0 = cancel): ", 0, g.expenses.size());
    if (number == 0) return;

    cout << "1. Amount\n"
         << "2. Description\n"
         << "3. Payer\n"
         << "4. Category\n"
         << "0. Cancel\n";
    int field = readInt("What would you like to change? ", 0, 4);
    if (field == 0) return;

    Expense& e = g.expenses[number - 1];
    Expense proposed = e;
    switch (field) {
        case 1:
            proposed.amount = readAmount("New amount: ");
            cout << "Amount: " << money(e.amount) << " -> " << money(proposed.amount) << "\n";
            break;
        case 2:
            proposed.description = readText("New description: ");
            if (proposed.description == "") proposed.description = "(no description)";
            cout << "Description: " << e.description << " -> " << proposed.description << "\n";
            break;
        case 3:
            printPeople(g);
            proposed.payer = readInt("Who paid? (enter their number): ", 1, g.people.size()) - 1;
            cout << "Payer: " << g.people[e.payer] << " -> " << g.people[proposed.payer] << "\n";
            break;
        case 4:
            for (int category = FOOD; category <= OTHER; ++category)
                cout << category + 1 << ". " << categoryName(static_cast<Category>(category)) << "\n";
            proposed.category = static_cast<Category>(readInt("New category: ", 1, 5) - 1);
            cout << "Category: " << categoryName(e.category) << " -> " << categoryName(proposed.category) << "\n";
            break;
    }
    if (!readYesNo("Save this change?")) {
        cout << "Edit cancelled.\n";
        return;
    }
    if (!replaceExpense(g, number - 1, proposed)) { cout << "Invalid expense.\n"; return; }
    cout << "Expense #" << number << " updated.\n";
}

void searchExpenses(const Group& g) {
    if (g.expenses.empty()) {
        cout << "No expenses to search.\n";
        return;
    }
    cout << "Search by:\n"
         << "1. Description\n"
         << "2. Payer\n"
         << "3. Day\n"
         << "0. Cancel\n";
    int field = readInt("Choose: ", 0, 3);
    if (field == 0) return;

    string query;
    int payer = -1;
    int day = 0;
    if (field == 1) {
        query = readText("Description contains (blank = cancel): ");
        if (query.empty()) return;
    } else if (field == 2) {
        printPeople(g);
        int number = readInt("Payer number (0 = cancel): ", 0, g.people.size());
        if (number == 0) return;
        payer = number - 1;
    } else {
        day = readInt("Day number (0 = cancel): ", 0, 365);
        if (day == 0) return;
    }

    int matches = 0;
    for (int i = 0; i < (int)g.expenses.size(); i++) {
        const Expense& e = g.expenses[i];
        ExpenseFilter filter = field == 1 ? ExpenseFilter::Description
                             : field == 2 ? ExpenseFilter::Payer : ExpenseFilter::Day;
        bool match = matchesExpense(g, e, filter, query, field == 2 ? payer : day);
        if (match) {
            cout << "#" << i + 1 << "  Day " << e.day << "  " << e.description
                 << "  - " << g.people[e.payer] << " paid " << money(e.amount) << "\n";
            matches++;
        }
    }
    if (matches == 0) cout << "No matching expenses.\n";
    else cout << "Matching expenses: " << matches << "\n";
}
