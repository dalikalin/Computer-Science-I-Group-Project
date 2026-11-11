
# Student Expense Tracker

A C++ project that helps students keep track of their money. The student can record income, expenses, and savings, then see where the money went and how much is left. All records are saved in text files, so the data is still there the next time the program is opened.

The project has two versions that share the same logic:

* **Console version** (`main.cpp`): a menu-driven program in the terminal.
* **GUI version** (`guimain.cpp`): a window with tabs, buttons, text boxes, and tables, built with Dear ImGui and GLFW.

## Features

* Add income and expenses (amount, date, description)
* View all transactions
* Financial summary: total income, total expenses, and balance
* Add and view savings, with a total
* Save and load data with text files (`transactions.txt` and `savings.txt`)
* Input validation: amounts must be above 0, and menu choices must be valid. The GUI version also checks the date format (DD-MM-YYYY).

## C++ concepts used

| Concept                     | Where it is used                                                                                                              |
| --------------------------- | ----------------------------------------------------------------------------------------------------------------------------- |
| Class and objects           | The`Transaction`class holds one record (type, amount, date, description)                                                    |
| Constructor                 | Fills in a`Transaction`object when it is created                                                                            |
| `vector`                  | Two vectors of`Transaction`objects:`transactions`and`savings`                                                           |
| Functions                   | Each function has one job, such as`saveData`,`loadData`,`readAmount`,`addTransaction`,`viewAll`, and`showSummary` |
| Loops and`if/else`        | The`do-while`menu loop,`for`loops over the vectors, and input validation                                                  |
| `switch`                  | Chooses which function to run from the menu                                                                                   |
| File handling (`fstream`) | `ofstream`writes records to a file and`ifstream`reads them back                                                           |

## Project files

| File                 | Purpose                                             |
| -------------------- | --------------------------------------------------- |
| `main.cpp`         | Console version                                     |
| `guimain.cpp`      | GUI version                                         |
| `imgui/`           | Dear ImGui library files (open source, MIT license) |
| `transactions.txt` | Saved income and expenses                           |
| `savings.txt`      | Saved savings                                       |

Each line in a data file is one record, written as: `Type Amount Date Description`
(for example: `Expense 5.50 03-11-2026 Lunch`). The description must be one word.

## How to run

### Console version

```
g++ main.cpp -o main
.\main.exe
```

### GUI version (Windows, MSYS2 UCRT64)

1. Install MSYS2, then open **MSYS2 UCRT64** and install GLFW:

```
pacman -S mingw-w64-ucrt-x86_64-glfw
```

2. In the project folder, build the program (one line):

```
g++ guimain.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp -Iimgui -o guimain -lglfw3 -lopengl32 -lgdi32
```

3. Run it:

```
.\guimain.exe
```

Run both versions from the project folder, so they use the same `transactions.txt` and `savings.txt`.

## Team and contributions

| Member        | Contribution                                                                              |
| ------------- | ----------------------------------------------------------------------------------------- |
| Kun Mouyteang | [e.g. Transaction class, add/view transactions, input validation]                         |
| Lin Dalika    | Implemented the code, built the graphical user interface (GUI), created Github repository |

## Limitations and future improvements

* The description must be one word.
* Records cannot be edited or deleted yet.
* Savings are tracked separately and do not change the balance.
* Possible additions: spending by category, a monthly budget with a warning, and search by date.
