# Expense Splitter - C++ Group Expense Manager

> Track shared bills, split them evenly or by percentage, keep an eye on a group budget, compare spending categories, and calculate who owes whom. Use the console interface or a native Windows GUI, with shared calculations and local save files.

## Quick start - Windows GUI

From the project folder, with MSYS2 MinGW g++ installed:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build-gui.ps1
.\build\expense-splitter-gui.exe
```

The script defaults to `C:\msys64\ucrt64\bin\g++.exe`. See [Build instructions](#build-instructions) if your compiler is elsewhere.

The GUI runs as a Windows application. GitHub hosts the source; it does not run the app in your browser.

---

## Table of contents

1. [Motivation](#motivation)
2. [What makes it different](#what-makes-it-different)
3. [Example use cases](#example-use-cases)
4. [Features and interfaces](#features-and-interfaces)
5. [Usage guide](#usage-guide)
6. [Developer guide](#developer-guide)
7. [Save files and compatibility](#save-files-and-compatibility)
8. [Limitations](#limitations)

---

## Motivation

On a group trip or shared outing, different people pay for different bills, and not everyone participates in every expense. Adding up receipts alone does not show how much each person owes.

Expense Splitter records the payer and participants for each bill, divides its cost, and calculates each person's balance. Category reports help the group understand where its money went, while editing makes it possible to correct a bill without removing and entering it again.

## Innovation

Compared with manual calculations using spreadsheets or calculators, Expense Splitter:

- Automatically calculates balances
- Supports percentage-based expense splitting
- Provides budget tracking
- Generates reports
- Stores and loads expense records
- Provides both a console version and Windows GUI version

## What makes it different

- **Exact to the cent.** All money is stored as whole cents, never floating point. When a bill cannot be divided exactly, the leftover cents go to the people whose shares were cut most by rounding (largest-remainder method), so the shares always add up to exactly the bill amount, even with uneven percentages.
- **Flexible splits.** Each bill can be split evenly among everyone, evenly among selected people, by a group's fixed percentages (e.g. a 40/30/30 rent agreement), or by custom percentages for that bill only.
- **Settle up, not just balances.** Besides showing each person's net balance, the app suggests a short list of payments that clears all debts, matching the person owed the most with the person who owes the most.
- **Budget tracking.** Set a group budget; the app shows how much is left after every bill and warns when the group goes over.
- **One core, two interfaces.** Validation, splitting, reports, and storage live in shared C++ code (`operations`, `expense`, `report`, `storage`) used by both the console app and the Windows GUI, so both always calculate the same results.
- **Safe save files.** Saves are versioned. Newer versions still load older files, and a file whose format is ambiguous or invalid is rejected instead of being loaded with a silently changed split.

## Example use cases

**1. Weekend trip with uneven participation.** Amy, Ben, and Cara set a 300.00 budget for a trip.

| Bill | Paid by | Split |
| --- | --- | --- |
| Hotel 180.00 | Amy | Evenly between all three |
| Dinner 60.00 | Ben | Custom: Amy 50%, Ben 25%, Cara 25% (Amy ordered the most) |
| Taxi 30.00 | Cara | Only Amy and Ben rode |

After each bill the app shows the budget left (120.00, 60.00, then 30.00). The balances and settle-up reports then show:

```text
  Name                Paid     Share       Net
  Amy               180.00    105.00    +75.00
  Ben                60.00     90.00    -30.00
  Cara               30.00     75.00    -45.00

  Cara pays Amy 45.00
  Ben pays Amy 30.00
```

Two payments settle the whole trip.

**2. Flatmates with a fixed rent split.** Set fixed percentages once (option **14**, e.g. 40/30/30 for rooms of different sizes). Each month, add rent with split mode **1. fixed %**; utilities can still be split evenly. Save the group and load it again next month.

**3. Club or project spending report.** Tag each purchase with a category, then use **Category totals**, **Analytics**, and **Ranking** to show where the money went, for example for a treasurer's report.

## Features and interfaces

| Feature | Console | Windows GUI |
| --- | --- | --- |
| Create a group | Enter member names at startup | Add people using the name field |
| Add expenses | Guided prompts | Expense form |
| Choose who shares a bill | Everyone or selected member numbers | Select names in the sharing list |
| Percentage splits | Fixed group percentages or custom per-bill percentages | Not yet available |
| Budget | Set at startup or option 15; shown after every bill | Not yet available |
| List expenses | Numbered list | Selectable table |
| Edit expenses | Option 12, with confirmation | Select a row, change values, and confirm |
| Search | Description, payer, or exact day filter | Combined description, payer, and day text search |
| Remove expenses | Select a bill number | Select a row and confirm removal |
| Daily summaries, balances, settlements | Menu reports | Scrollable report windows |
| Category totals, analytics, ranking | Options 9–11 | Category report buttons |
| Set fixed percentages, adjust budget | Options 14–15 | Not yet available |
| Save and load | Enter a file path | Windows file dialogs |

Both interfaces use C++ code for expense operations, cent splitting, reports, and storage. The GUI uses Win32 controls and does not require Qt.

## Usage guide

### 1. Create or load a group

In the GUI, type a member's name into **Person name** and click **Add person**. Repeat for the other members, or click **Load...** to open a saved group.

In the console, choose whether to load a file at startup. For a new group, enter the number of people and their names. You can then optionally set a budget and a fixed percentage split for the group.

Adding a person in the GUI does not change the participants of existing expenses.

### 2. Add a bill and choose its category

In the GUI:

1. Enter the **Day** number, **Description**, and **Amount**.
2. Select the **Payer** from the dropdown.
3. Click names in **Shared by** to select or deselect participants. At least one person must share the bill.
4. Choose a **Category**: Food, Transport, Accommodation, Entertainment, or Other. The default is Other.
5. Click **Add expense**.

In the console, choose **1. Add expense** and follow the prompts. Category selection comes after the amount, then choose how to split:

1. **Fixed %** - use the group's fixed percentages (set with option **14**). If none are set, the app falls back to an even split.
2. **Even** - split equally among everyone or among the people you select.
3. **Custom %** - enter a percentage for each person for this bill only.

After adding, the console shows each person's share and, if a budget is set, how much is left.

Amounts are stored as whole cents. If a bill cannot be divided exactly, the leftover cents go to the people with the largest rounding remainders, so the shares always add up to the bill. For example, 10.01 shared evenly by two people becomes 5.01 and 5.00.

### 3. Correct an existing bill

Select a row in the GUI table, then change its **description**, **amount**, **payer**, or **category**. Click **Confirm edit...** to review the old and new values, then choose **Yes** to apply them.

Use **New / cancel edit** to discard the form changes and return to adding a bill. The bill's day and sharing list remain fixed during editing.

In the console, choose **12. Edit expense**, select the bill, and choose the field to change. Category is field **4**. The update is applied only after confirmation.

Confirming an edit updates the current group in memory. Use **Save** to write it to disk.

### 4. Search and read reports

The GUI search box matches text in descriptions, payer names, or day numbers. Matching is case-insensitive; clear the box to show all bills. The console offers separate description, payer, and day filters through option **13**.

| Report | What it shows |
| --- | --- |
| By day | Bills, totals, and individual balances for each day |
| Balances | Each person's total paid, share of costs, and net balance |
| Settle up | Suggested payments from people who owe money to people owed money |
| Category totals | Total spending in each category |
| Analytics | Category percentages and the highest-spending category |
| Ranking | Categories with spending, ordered from highest to lowest |

A positive net balance means the person should receive money; a negative balance means they owe money. For example, if Alice pays 30.00 for a bill shared equally with Bob, the settlement report suggests Bob pays Alice 15.00.

GUI reports cover the whole group, regardless of the search filter. They are snapshots: reopen a report after changing expenses. Settlement suggestions do not record or execute payments.

### 5. Set percentages and a budget (console)

- **14. Set fixed percentages** shows the current percentages and lets you enter new ones for each person. Percentages are used as relative weights, so 50/25/25 and 2/1/1 give the same split.
- **15. Adjust budget** adds to or subtracts from the budget, for example `+100` or `-50`. The budget line at the top of the menu shows the budget, spending so far, and how much is left, with a warning when the group is over budget.

### 6. Save your work

Use **Save...** and **Load...** in the GUI, or options **7** and **8** in the console. Both interfaces can read the same supported files.

The GUI asks about unsaved group changes before closing or loading another file. Unsubmitted form input is not saved. The console does not provide that warning, so save before exiting or loading a different group.

## Developer guide

### Repository layout

```text
expense-splitter/
|-- build-gui.ps1             # Windows GUI build script
|-- gui/
|   `-- main.cpp              # Win32 window, forms, table, and report windows
|-- src/
|   |-- main.cpp              # Console menu and entry point
|   |-- expense.h / .cpp      # Expense/group data and money/share helpers
|   |-- group.h / .cpp        # Console group and expense interactions
|   |-- input.h / .cpp        # Console input parsing and validation
|   |-- operations.h / .cpp   # Shared mutations, validation, and search matching
|   |-- percentage.h / .cpp   # Fixed and custom percentage splits
|   |-- budget.h / .cpp       # Budget tracking and warnings
|   |-- report.h / .cpp       # Shared report generation and console wrappers
|   `-- storage.h / .cpp      # Versioned saves and compatible loading
|-- tests/
|   `-- operations_test.cpp   # Shared-operation, report, and storage checks
`-- build/                   # Generated executables; ignored by Git
```

### Build instructions

The code targets **C++11**. The Windows GUI build requires a MinGW g++ compiler and PowerShell. Run commands from the repository root.

**GUI with the default compiler:**

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build-gui.ps1
.\build\expense-splitter-gui.exe
```

`-ExecutionPolicy Bypass` applies to this PowerShell process; it does not permanently change the machine's execution policy.

**GUI with a different compiler path:**

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build-gui.ps1 -Compiler 'C:\path\to\g++.exe'
```

The build script links the GUI statically. If an older app instance is running, close it after saving, or build a separate executable:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build-gui.ps1 -OutputName expense-splitter-gui-updated.exe
.\build\expense-splitter-gui-updated.exe
```

**Console with g++ on PATH:**

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ -std=c++11 -Wall -Wextra src/*.cpp -o build/expense-splitter-console.exe
.\build\expense-splitter-console.exe
```

If PowerShell cannot find `g++`, add your compiler's `bin` directory to PATH for the current terminal. For the default MSYS2 installation:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
```

### Shared logic

`appendExpense`, `replaceExpense`, and `eraseExpense` in `operations.cpp` are used by both interfaces. They validate mutations before changing the group. `matchesExpense` provides common search matching.

Report functions return strings that the console prints or the GUI displays. When adding a new feature, keep reusable data operations outside console prompts and Win32 event handlers so both interfaces can use them.

## Example Use Cases

### Travel Group

**Members:**
- Alice
- Bob
- Charlie

**Expenses:**
- Alice paid $300 for hotel accommodation
- Bob paid $150 for food
- Charlie paid $150 for transportation

**Calculation:**

Total expenses = $600

Each member's share = $600 ÷ 3 = $200

**Result:**
- Bob pays Alice $50
- Charlie pays Alice $50

### Student Project Team

A project team can track shared expenses throughout a semester, such as:

- Food and refreshments
- Printing and stationery
- Transportation costs
- Event-related purchases

The system automatically records expenses, calculates each member's share, and determines reimbursements required to settle all balances fairly.

### Run the checks

With g++ on PATH, compile and run the existing checks:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ -std=c++11 -Wall -Wextra -Isrc tests/operations_test.cpp src/operations.cpp src/expense.cpp src/report.cpp src/storage.cpp src/input.cpp src/percentage.cpp -o build/operations-test.exe
.\build\operations-test.exe build/test-roundtrip.txt
```

Use a disposable path for the test argument: the test writes and removes that file. Keep assertions enabled; do not compile these checks with `-DNDEBUG`.

The checks cover expense validation, cent splitting, search matching, reports, and save/load compatibility. They do not automate GUI interactions. Also check adding, selecting, editing, cancelling, filtering, and saving bills in the desktop app when changing its controls.

## Save files and compatibility

New files start with `EXPENSE_SPLITTER_V3`. They store member names, the group's fixed percentages and budget, and each bill's day, description, payer, amount in cents, category, participants, and percentage weights.

- Both interfaces read and write this format.
- Files from earlier versions (`EXPENSE_SPLITTER_V2`, `v2`, and unversioned files) still load. Missing budgets are treated as not set, and missing percentages as an even split.
- Older unversioned files are accepted when their format can be identified unambiguously.
- Bills loaded from files without categories receive **Other**.
- Ambiguous or invalid files are rejected without replacing the current group.
- Older app versions cannot read the new versioned saves.

Files are plain text and are stored locally. There is no cloud synchronization or account system.

## Limitations

- The GUI is Windows-only and has a fixed main-window layout.
- Interface text is English. The GUI uses Windows system text encoding rather than full Unicode support, so text portability across different language settings is limited.
- Percentage splits and the budget can only be set in the console. The GUI keeps them when loading and saving a file, and its reports use the percentage splits, but it cannot set or change them yet.
- Percentage splits always include every member of the group; to leave someone out of a percentage-split bill, give them 0%.
- Settle-up suggests a short list of payments but does not guarantee the smallest possible number of payments in every case.
- Days are numbers from 1 to 365, not calendar dates.
- Editing supports amount, description, payer, and category; it does not change the day or participants.
- There is no currency conversion, payment processing, or edit history.
- Several totals use 32-bit integers; very large accumulated amounts can overflow. The app is intended for small group expenses.
