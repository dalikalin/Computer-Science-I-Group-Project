#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

// The Transaction class: one object holds ONE income or expense
class Transaction {
public:
    string type;         // "Income" or "Expense"
    double amount;       // the money amount
    string date;         // written as dd-mm-yyyy
    string description;  // one word, e.g. Lunch or Bus_fare

    // The constructor copies the given values into the variables above
    Transaction(string t, double a, string d, string desc) {
        type = t;
        amount = a;
        date = d;
        description = desc;
    }
};

// SAVE: write every transaction to transactions.txt (one per line)
// The & means "use the original list", not a copy
void saveData(vector<Transaction>& list) {
    ofstream file("transactions.txt");   // opens the file for writing

    // Go through every transaction in the list
    for (Transaction t : list) {
        // Write the 4 parts on one line, separated by spaces
        file << t.type << " " << t.amount << " " << t.date << " " << t.description << endl;
    }

    file.close();
}

// LOAD: read transactions.txt and put each line into the list
void loadData(vector<Transaction>& list) {
    ifstream file("transactions.txt");   // opens the file for reading

    // If the file does not exist yet (first run), just stop
    if (!file) {
        return;
    }

    string type;
    string date;
    string description;
    double amount;

    // Read 4 values at a time. The loop stops when the file ends.
    while (file >> type >> amount >> date >> description) {
        list.push_back(Transaction(type, amount, date, description));
    }

    file.close();
    cout << "Loaded " << list.size() << " transaction(s) from transactions.txt" << endl;
}

// Ask for an amount again and again until it is a number above 0
double readAmount() {
    double amount;
    cout << "Enter amount: $";

    // If the user types letters, or a number that is 0 or less, ask again
    while (!(cin >> amount) || amount <= 0) {
        cin.clear();               // clear the error state
        cin.ignore(1000, '\n');    // throw away the wrong input
        cout << "Invalid amount. Enter a number above 0: ";
    }
    return amount;
}

// ADD: ask for the details, then add one income or expense
void addTransaction(vector<Transaction>& list, string type) {
    string date;
    string description;

    double amount = readAmount();

    cout << "Enter date (DD-MM-YYYY): ";
    cin >> date;

    cout << "Enter description (one word, e.g. Lunch): ";
    cin >> description;
    cin.ignore(1000, '\n');   // throw away anything else typed on this line

    // Make the Transaction and add it to the end of the list
    list.push_back(Transaction(type, amount, date, description));

    // Save to the file right away, so nothing is lost
    saveData(list);

    cout << type << " added and saved." << endl;
}

// VIEW: print every transaction
void viewAll(vector<Transaction>& list) {
    // empty() is true when the list has nothing in it
    if (list.empty()) {
        cout << "No transactions yet." << endl;
        return;
    }

    cout << "\n--- All Transactions ---" << endl;
    int number = 1;   // counts the lines: 1, 2, 3...
    for (Transaction t : list) {
        cout << number << ". " << t.date << " | " << t.type
             << " | $" << fixed << setprecision(2) << t.amount
             << " | " << t.description << endl;
        number++;
    }
}

// SUMMARY: total income, total expenses, and what is left
void showSummary(vector<Transaction>& list) {
    double totalIncome = 0;
    double totalExpenses = 0;

    // Add up the amounts, depending on the type
    for (Transaction t : list) {
        if (t.type == "Income") {
            totalIncome = totalIncome + t.amount;
        } else {
            totalExpenses = totalExpenses + t.amount;
        }
    }

    double balance = totalIncome - totalExpenses;

    cout << "\n--- Financial Summary ---" << endl;
    cout << fixed << setprecision(2);
    cout << "Total income:   $" << totalIncome << endl;
    cout << "Total expenses: $" << totalExpenses << endl;
    cout << "Balance:        $" << balance << endl;
}

// Save: write all saving records to savings.txt
void saveSavings(vector<Transaction>& savings) {
    ofstream file("savings.txt");   // opens the files for writing

    for (Transaction s : savings) {
        file << s.type << " " << s.amount << " " << s.date << " " << s.description << endl;
    }
    file.close();
}

// LOAD SAVINGS: read previous saving records
void loadSavings(vector<Transaction >& savings) {
    ifstream file("savings.txt");

    if (!file) {
        return;
    }

    string type, date, description;
    double amount;

    while (file >> type >> amount >> date >> description) {
        savings.push_back(Transaction(type, amount, date, description));
    }
    file.close();
}

// ADD SAVINGS: record a new saving
void addSaving(vector<Transaction>& savings) {
    string date;
    string description;

    double amount = readAmount();

    cout << "Enter date (DD-MM-YYYY): ";
    cin >> date;

    cout << "Enter saving purpose (one word, e.g. Laptop): ";
    cin >> description;

    savings.push_back(Transaction("Saving", amount, date, description));

    saveSavings(savings);

    cout << "Saving added and saved sucessfully!" << endl;
}

// VIEW SAVINGS: display all saving records
void viewSavings(vector<Transaction>& savings) {
    if (savings.empty()) {
        cout << "No savings records yet." << endl;
        return;
    }

    double totalSavings = 0;

    cout << "\n--- ALL SAVINGS ---" << endl;

    int number = 1;
    for (Transaction s : savings) {
        cout << number << "."
             << s.date << " | "
             << "Saving | $"
             << fixed << setprecision(2) << s.amount
             << " | " << s.description << endl;

        totalSavings += s.amount;
        number++;
    }

    cout << "\nTotal Savings: $" << fixed << setprecision(2) << totalSavings << endl;
}

// MAIN: the program starts here
int main() {
    vector<Transaction> transactions;   // the list of all income and expenses
    vector<Transaction> savings;        // the list of all savings records
    int choice = 0;                     // the menu number the user types

    loadData(transactions);             // load old data from the file (if any)
    loadSavings(savings);              // load old savings records from the file (if any)
    
    // Repeat the menu until the user chooses 5
    do {
        cout << "\n====== STUDENT EXPENSE TRACKER ======" << endl;
        cout << "1. Add Income" << endl;
        cout << "2. Add Expense" << endl;
        cout << "3. View All Transactions" << endl;
        cout << "4. View Financial Summary" << endl;
        cout << "5. Add Savings" << endl;
        cout << "6. View Savings" << endl;
        cout << "7. Save and Exit" << endl;
        cout << "Choose an option: ";

        // If the user types a letter instead of a number, treat it as invalid
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        }

        // Run the part that matches the number
        switch (choice) {
            case 1:
                addTransaction(transactions, "Income");
                break;
            case 2:
                addTransaction(transactions, "Expense");
                break;
            case 3:
                viewAll(transactions);
                break;
            case 4:
                showSummary(transactions);
                break;
            case 5:
                addSaving(savings);
                break;
            case 6:
                viewSavings(savings);
                break;
            case 7:
                saveData(transactions);
                saveSavings(savings);
                cout << "Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please type a number from 1 to 7." << endl;
        }
    } while (choice != 7);

    return 0;
}