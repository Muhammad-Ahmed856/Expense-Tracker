# 💰 Expense Tracker - Console-based C++ Application

A full-featured console-based Expense Tracker built in C++ that helps users manage personal finances with budgeting, smart suggestions, sorting, category-based filtering, and more.

---

## 🔧 Features

- 🔐 **User Authentication**
  - Register/Login using a local file-based system.
  
- 💸 **Expense Management**
  - Add, edit, and delete expenses.
  - Assign categories to expenses (with custom options).
  - Validate amount and date formats.
  
- 📊 **Budgeting**
  - Set and track monthly budgets.
  - Visualize remaining vs spent amounts.
  - Automatic alerts when spending exceeds 80% or 100% of budget.
  
- 📅 **Time-Based Insights**
  - Filter by current month or all-time.
  - Monthly summaries with minimum and maximum spend detection.
  
- 🔍 **Advanced Viewing & Analysis**
  - View all expenses.
  - Filter by category or month.
  - Sort by date or amount (ascending/descending).
  - Smart budget suggestions based on spending trends.
  - Compare current and previous month’s expenses.
  
- 📑 **Additional Tools**
  - Loan payment calculator with warning if budget drops below threshold.
  - Operation history tracking (last 5 actions).
  - Help guide with instructions.
  - Clear all or monthly expenses with confirmation.
  
---

## 🧑‍💻 Technologies Used

- Language: **C++**
- IDE: Dev-C++ or any C++-compatible compiler (Windows recommended)
- Data Persistence: **Text file-based storage** (no database needed)
- OS-Specific Command: Uses `system("cls")` for screen clearing (works on **Windows** only)

---

## 📁 File Structure

- `Expense Tracker.cpp` - Main application source code.
- `users.txt` - Stores user login data.
- `USERNAME_expenses.txt` - Each user's expenses saved in a separate file.

---

## 🛠 How to Run

1. Open `Expense Tracker.cpp` in your IDE or C++ compiler (Dev-C++, Code::Blocks, Visual Studio).
2. Compile and run the program.
3. Choose to Register or Log In.
4. Use the main menu to manage expenses and budgets.

> ✅ Make sure to allow file creation in your project directory. The app will save user and expense data locally.

---

## 🚀 Future Improvements

- Port to GUI or web interface.
- Integrate with database (SQLite or MySQL).
- Export reports to CSV or PDF.
- Add graphical expense charts using external libraries.

---

## 👤 Author

**Muhammad Ahmed**
---

## 📃 License

This project is licensed under the [MIT License](LICENSE).

---

