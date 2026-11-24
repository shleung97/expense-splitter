# Expense Splitter - C++ Group Expense Manager

> Track shared bills, correct expenses, compare spending categories, and calculate who owes whom. Use the console interface or a native Windows GUI, with shared calculations and local save files.

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
2. [Features and interfaces](#features-and-interfaces)
3. [Usage guide](#usage-guide)
4. [Developer guide](#developer-guide)
5. [Save files and compatibility](#save-files-and-compatibility)
6. [Limitations](#limitations)

---

## Motivation

On a group trip or shared outing, different people pay for different bills, and not everyone participates in every expense. Adding up receipts alone does not show how much each person owes.

Expense Splitter records the payer and participants for each bill, divides its cost, and calculates each person's balance. Category reports help the group understand where its money went, while editing makes it possible to correct a bill without removing and entering it again.

## Features and interfaces

| Feature | Console | Windows GUI |
| --- | --- | --- |
| Create a group | Enter member names at startup | Add people using the name field |
| Add expenses | Guided prompts | Expense form |
| Choose who shares a bill | Everyone or selected member numbers | Select names in the sharing list |
| List expenses | Numbered list | Selectable table |
| Edit expenses | Option 12, with confirmation | Select a row, change values, and confirm |
| Search | Description, payer, or exact day filter | Combined description, payer, and day text search |
| Remove expenses | Select a bill number | Select a row and confirm removal |
| Daily summaries, balances, settlements | Menu reports | Scrollable report windows |
| Category totals, analytics, ranking | Options 9–11 | Category report buttons |
| Save and load | Enter a file path | Windows file dialogs |

Both interfaces use C++ code for expense operations, cent splitting, reports, and storage. The GUI uses Win32 controls and does not require Qt.

## Usage guide

### 1. Create or load a group

In the GUI, type a member's name into **Person name** and click **Add person**. Repeat for the other members, or click **Load...** to open a saved group.

In the console, choose whether to load a file at startup. For a new group, enter the number of people and their names.

Adding a person in the GUI does not change the participants of existing expenses.

### 2. Add a bill and choose its category

In the GUI:

1. Enter the **Day** number, **Description**, and **Amount**.
2. Select the **Payer** from the dropdown.
3. Click names in **Shared by** to select or deselect participants. At least one person must share the bill.
4. Choose a **Category**: Food, Transport, Accommodation, Entertainment, or Other. The default is Other.
5. Click **Add expense**.

In the console, choose **1. Add expense** and follow the prompts. Category selection comes after the amount; sharing selection follows it.

Amounts are stored as whole cents. If a bill cannot be divided evenly, extra cents are assigned in the order of its stored participant list. For example, 10.01 shared by two people becomes 5.01 and 5.00.

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

### 5. Save your work

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

### Run the checks

With g++ on PATH, compile and run the existing checks:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ -std=c++11 -Wall -Wextra -Isrc tests/operations_test.cpp src/operations.cpp src/expense.cpp src/report.cpp src/storage.cpp src/input.cpp -o build/operations-test.exe
.\build\operations-test.exe build/test-roundtrip.txt
```

Use a disposable path for the test argument: the test writes and removes that file. Keep assertions enabled; do not compile these checks with `-DNDEBUG`.

The checks cover expense validation, cent splitting, search matching, reports, and save/load compatibility. They do not automate GUI interactions. Also check adding, selecting, editing, cancelling, filtering, and saving bills in the desktop app when changing its controls.

## Save files and compatibility

New files start with `EXPENSE_SPLITTER_V2`. They store member names and each bill's day, description, payer, amount in cents, category, and participants.

- Both updated interfaces read and write this format.
- Older unversioned files are accepted when their format can be identified unambiguously.
- Bills loaded from files without categories receive **Other**.
- Ambiguous or invalid files are rejected without replacing the current group.
- Older app versions cannot read the new versioned saves.

Files are plain text and are stored locally. There is no cloud synchronization or account system.

## Limitations

- The GUI is Windows-only and has a fixed main-window layout.
- Interface text is English. The GUI uses Windows system text encoding rather than full Unicode support, so text portability across different language settings is limited.
- Expenses split equally among selected people; custom percentages and unequal shares are not supported.
- Days are numbers from 1 to 365, not calendar dates.
- Editing supports amount, description, payer, and category; it does not change the day or participants.
- There is no currency conversion, payment processing, or edit history.
- Several totals use 32-bit integers; very large accumulated amounts can overflow. The app is intended for small group expenses.
