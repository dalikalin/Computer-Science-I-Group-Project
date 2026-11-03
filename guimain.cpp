// ============ STUDENT EXPENSE TRACKER (ImGui window version) ============
// ImGui draws the buttons and boxes. GLFW makes the window.

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <string>    // string (text)
#include <vector>    // vector (a list that can grow)
#include <fstream>   // ifstream and ofstream (reading and writing files)
#include <iomanip>   // fixed and setprecision (2 decimal places in the file)
#include <cctype>    // isdigit (to check the date)
using namespace std;

// ---------------------------------------------------------------
// The Transaction class: one object holds ONE record.
// It is used for income, expenses AND savings.
// ---------------------------------------------------------------
class Transaction {
public:
    string type;         // "Income", "Expense" or "Saving"
    double amount;       // the money amount
    string date;         // written as dd-mm-yyyy
    string description;  // one word, e.g. Lunch or Laptop

    // The constructor copies the given values into the variables above
    Transaction(string t, double a, string d, string desc) {
        type = t;
        amount = a;
        date = d;
        description = desc;
    }
};

// ---------------------------------------------------------------
// SAVE: write every record in a list to a file (one per line).
// "filename" tells it WHICH file, so we can use it for both
// transactions.txt and savings.txt.
// ---------------------------------------------------------------
void saveData(vector<Transaction>& list, string filename) {
    ofstream file(filename);                  // opens the file for writing
    file << fixed << setprecision(2);         // always write 2 decimal places

    for (Transaction t : list) {
        file << t.type << " " << t.amount << " " << t.date << " " << t.description << endl;
    }

    file.close();
}

// ---------------------------------------------------------------
// LOAD: read a file and put each line into the list
// ---------------------------------------------------------------
void loadData(vector<Transaction>& list, string filename) {
    ifstream file(filename);                  // opens the file for reading

    // If the file does not exist yet (first run), just stop
    if (!file) {
        return;
    }

    string type, date, description;
    double amount;

    // Read 4 values at a time. The loop stops when the file ends.
    while (file >> type >> amount >> date >> description) {
        list.push_back(Transaction(type, amount, date, description));
    }

    file.close();
}

// ---------------------------------------------------------------
// The file stores one word per field, so spaces become underscores
// ---------------------------------------------------------------
string makeOneWord(string text) {
    for (int i = 0; i < (int)text.size(); i++) {
        if (text[i] == ' ') {
            text[i] = '_';
        }
    }
    // An empty description would break the file, so use a dash
    if (text.empty()) {
        text = "-";
    }
    return text;
}

// ---------------------------------------------------------------
// Check that the date looks like DD-MM-YYYY (for example 03-11-2026)
// ---------------------------------------------------------------
bool isValidDate(string d) {
    if (d.size() != 10) {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            if (d[i] != '-') return false;                         // dashes in the right places
        } else {
            if (!isdigit((unsigned char)d[i])) return false;       // digits everywhere else
        }
    }
    int day = stoi(d.substr(0, 2));
    int month = stoi(d.substr(3, 2));
    return day >= 1 && day <= 31 && month >= 1 && month <= 12;
}

// ---------------------------------------------------------------
// MAIN: the program starts here
// ---------------------------------------------------------------
int main() {
    // Start GLFW. If it fails, stop the program.
    if (!glfwInit()) {
        return 1;
    }

    // Create the window: 800 wide, 600 tall
    GLFWwindow* window = glfwCreateWindow(800, 600, "Student Expense Tracker", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);   // draw into this window
    glfwSwapInterval(1);              // match the screen speed

    // Start ImGui and connect it to the window
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Make all text bigger. 1.0 is normal size, 1.5 is 50% bigger.
    ImGui::GetStyle().FontScaleMain = 1.5f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // ----- Our data: two lists, two files -----
    vector<Transaction> transactions;   // income and expenses
    vector<Transaction> savings;        // savings records
    loadData(transactions, "transactions.txt");
    loadData(savings, "savings.txt");

    // ----- Variables for the TRANSACTION form -----
    double amount = 0;            // the amount box
    char date[32] = "";           // the date box (text)
    char description[64] = "";    // the description box (text)
    int typeChoice = 0;           // 0 = Income, 1 = Expense
    string message = "";          // the message shown under the Add button
    bool messageIsError = false;  // true = show the message in red

    // ----- Variables for the SAVINGS form -----
    double savAmount = 0;
    char savDate[32] = "";
    char savPurpose[64] = "";
    string savMessage = "";
    bool savMessageIsError = false;

    // This loop runs once per frame until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();   // check for mouse and keyboard events

        // Tell ImGui a new frame is starting
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Make our ImGui window fill the whole GLFW window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Student Expense Tracker", NULL,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        // The tab bar: click a tab name to switch pages
        if (ImGui::BeginTabBar("mainTabs")) {

            // =====================================================
            // TAB 1: TRANSACTIONS (income and expenses)
            // =====================================================
            if (ImGui::BeginTabItem("Transactions")) {

                // ----- Summary -----
                double totalIncome = 0;
                double totalExpenses = 0;
                for (Transaction t : transactions) {
                    if (t.type == "Income") {
                        totalIncome = totalIncome + t.amount;
                    } else {
                        totalExpenses = totalExpenses + t.amount;
                    }
                }
                double balance = totalIncome - totalExpenses;

                ImGui::Text("FINANCIAL SUMMARY");
                ImGui::Text("Total income:   $%.2f", totalIncome);
                ImGui::Text("Total expenses: $%.2f", totalExpenses);
                ImGui::Text("Balance:        $%.2f", balance);

                ImGui::Separator();

                // ----- Add form -----
                ImGui::Text("ADD TRANSACTION");

                // Two radio buttons: only one can be chosen
                ImGui::RadioButton("Income", &typeChoice, 0);
                ImGui::SameLine();   // put the next item on the same line
                ImGui::RadioButton("Expense", &typeChoice, 1);

                ImGui::InputDouble("Amount", &amount, 0.0, 0.0, "%.2f");
                ImGui::InputText("Date (DD-MM-YYYY)", date, sizeof(date));
                ImGui::InputText("Description", description, sizeof(description));

                // This code runs when the Add button is clicked
                if (ImGui::Button("Add")) {
                    string dateText = date;

                    if (amount <= 0) {
                        message = "Amount must be above 0.";
                        messageIsError = true;
                    } else if (!isValidDate(dateText)) {
                        message = "Date must look like 03-11-2026.";
                        messageIsError = true;
                    } else {
                        string type = (typeChoice == 0) ? "Income" : "Expense";

                        // Add the record to the list, then save the file right away
                        transactions.push_back(Transaction(type, amount, dateText, makeOneWord(description)));
                        saveData(transactions, "transactions.txt");

                        message = type + " added and saved.";
                        messageIsError = false;

                        // Clear the boxes for the next record
                        amount = 0;
                        date[0] = '\0';
                        description[0] = '\0';
                    }
                }

                // Show the message (red for errors, green for success)
                if (!message.empty()) {
                    if (messageIsError) {
                        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", message.c_str());
                    } else {
                        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s", message.c_str());
                    }
                }

                ImGui::Separator();

                // ----- Table of all transactions -----
                ImGui::Text("ALL TRANSACTIONS (%d)", (int)transactions.size());

                if (transactions.empty()) {
                    ImGui::Text("No transactions yet.");
                } else if (ImGui::BeginTable("transactionTable", 5,
                           ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {

                    ImGui::TableSetupScrollFreeze(0, 1);   // keep the heading visible when scrolling
                    ImGui::TableSetupColumn("#");
                    ImGui::TableSetupColumn("Date");
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupColumn("Amount");
                    ImGui::TableSetupColumn("Description");
                    ImGui::TableHeadersRow();

                    // One row for each transaction
                    for (int i = 0; i < (int)transactions.size(); i++) {
                        Transaction& t = transactions[i];

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%d", i + 1);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", t.date.c_str());

                        ImGui::TableSetColumnIndex(2);
                        if (t.type == "Income") {
                            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Income");    // green
                        } else {
                            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Expense");   // red
                        }

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("$%.2f", t.amount);

                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("%s", t.description.c_str());
                    }

                    ImGui::EndTable();
                }

                ImGui::EndTabItem();   // end of tab 1
            }

            // =====================================================
            // TAB 2: SAVINGS
            // =====================================================
            if (ImGui::BeginTabItem("Savings")) {

                // ----- Total savings -----
                double totalSavings = 0;
                for (Transaction s : savings) {
                    totalSavings = totalSavings + s.amount;
                }

                ImGui::Text("SAVINGS SUMMARY");
                ImGui::Text("Total savings: $%.2f", totalSavings);

                ImGui::Separator();

                // ----- Add saving form -----
                ImGui::Text("ADD SAVING");

                ImGui::InputDouble("Saving amount", &savAmount, 0.0, 0.0, "%.2f");
                ImGui::InputText("Saving date (DD-MM-YYYY)", savDate, sizeof(savDate));
                ImGui::InputText("Purpose", savPurpose, sizeof(savPurpose));

                // This code runs when the Add Saving button is clicked
                if (ImGui::Button("Add Saving")) {
                    string dateText = savDate;

                    if (savAmount <= 0) {
                        savMessage = "Amount must be above 0.";
                        savMessageIsError = true;
                    } else if (!isValidDate(dateText)) {
                        savMessage = "Date must look like 03-11-2026.";
                        savMessageIsError = true;
                    } else {
                        // Add the record to the savings list, then save savings.txt
                        savings.push_back(Transaction("Saving", savAmount, dateText, makeOneWord(savPurpose)));
                        saveData(savings, "savings.txt");

                        savMessage = "Saving added and saved.";
                        savMessageIsError = false;

                        // Clear the boxes for the next record
                        savAmount = 0;
                        savDate[0] = '\0';
                        savPurpose[0] = '\0';
                    }
                }

                // Show the message (red for errors, green for success)
                if (!savMessage.empty()) {
                    if (savMessageIsError) {
                        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", savMessage.c_str());
                    } else {
                        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s", savMessage.c_str());
                    }
                }

                ImGui::Separator();

                // ----- Table of all savings -----
                ImGui::Text("ALL SAVINGS (%d)", (int)savings.size());

                if (savings.empty()) {
                    ImGui::Text("No savings records yet.");
                } else if (ImGui::BeginTable("savingsTable", 4,
                           ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {

                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("#");
                    ImGui::TableSetupColumn("Date");
                    ImGui::TableSetupColumn("Amount");
                    ImGui::TableSetupColumn("Purpose");
                    ImGui::TableHeadersRow();

                    for (int i = 0; i < (int)savings.size(); i++) {
                        Transaction& s = savings[i];

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%d", i + 1);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", s.date.c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("$%.2f", s.amount);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%s", s.description.c_str());
                    }

                    ImGui::EndTable();
                }

                ImGui::EndTabItem();   // end of tab 2
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        // Draw everything on the screen
        ImGui::Render();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);   // dark gray background
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Clean up when the window closes
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}