#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <iomanip>
#include <ctime>
#include <limits>
#include <cmath>
using namespace std;

// ==================== Utility Functions ====================
void clearScreen() {
    system("cls"); // Windows-specific command to clear the screen
}

int getValidatedChoice() {
    int choice;
    while (true) 
	{
        cout << "Enter your choice: ";
        cin >> choice;
        if (cin.fail()||cin.peek()!='\n') 
		{
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter a valid integer." << endl;
        } 
		else 
		{
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
    }
}

double validatedAmount() {
    double amount;
    while (true) {
        cout << "Enter amount: ";
        cin >> amount;
        if (cin.fail() || cin.peek() != '\n') {
            cin.clear();  // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
            cout << "Invalid input. Please enter a numeric value.\n";
        } else if (amount <= 0) {
            cout << "Error: Amount must be greater than 0.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clean up input buffer
            return amount;  // Valid amount entered
        }
    }
}

string getCurrentDate() {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", now);
    return string(buffer);
}

string getCurrentMonth() {
	time_t t = time(0);
    struct tm* now = localtime(&t);
    char buffer[8];
    strftime(buffer, sizeof(buffer), "%Y-%m", now);
    return string(buffer);
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool isValidDate(const string& date) {
    if (date.length() != 10 || date[4] != '-' || date[7] != '-') return false;

    try {
        int year = stoi(date.substr(0, 4));
        int month = stoi(date.substr(5, 2));
        int day = stoi(date.substr(8, 2));

        if (month < 1 || month > 12) return false;

        if (month == 2) {
            if (isLeapYear(year)) return day >= 1 && day <= 29;
            else return day >= 1 && day <= 28;
        } else if (month == 4 || month == 6 || month == 9 || month == 11) {
            return day >= 1 && day <= 30;
        } else {
            return day >= 1 && day <= 31;
        }
        // Restrict future dates
        string currentDate = getCurrentDate();
        if (date > currentDate) 
        return false;
        if (year < 1900 || year > 2100) {
             cout << "Error: Year must be between 1900 and 2100.\n";
             return false;
}

    } catch (const invalid_argument&) {
        return false;
    }
}

void displayHelp() {
    clearScreen();
    cout << "==================== Expense Tracker Help ====================\n";
    cout << "1. Add Expense: Enter amount, description, and category.\n";
    cout << "2. Remove Expense: Enter the description of the expense to remove.\n";
    cout << "3. View Expenses: View all expenses or filter by category.\n";
    cout << "4. Set Budget: Enter your monthly budget.\n";
    cout << "5. Exit: Save and exit the program.\n";
    cout << "==============================================================\n";
}

string getCategoryFromUser() {
    const int numCategories = 8;
    string categoryList[numCategories] = {"Food", "Transport", "Entertainment", "Rent", "Utilities", "Healthcare", "Shopping", "Other"};

    clearScreen();
    cout << "==================== Select Category ====================\n";
    for (int i = 0; i < numCategories; i++) {
        cout << i + 1 << ". " << categoryList[i] << endl;
    }
    cout << "0. Enter custom category\n";
    cout << "=========================================================\n";

    int choice;
    string category;
    choice = getValidatedChoice();

    if (choice > 0 && choice <= numCategories) {
        return categoryList[choice - 1];
    } else {
        cout << "Enter custom category: ";
        getline(cin, category);
        return category;
    }
}

// ==================== User Class ====================
class User {
public:
    string username;
    string password;

    User(string user, string pass) : username(user), password(pass) {}
};

// ==================== Expense Class ====================
class Expense {
public:
    double amount;
    string description;
    string category;
    string date;

    Expense(double amt, string desc, string cat, string dt)
        : amount(amt), description(desc), category(cat), date(dt) {}

    void display() const {
        cout << description << " - " << amount << " in category " << category << " on " << date << endl;
    }
};

// ==================== Login System ====================
class LoginSystem {
private:
    string userFile = "users.txt";

public:
    void registerUser() {
        clearScreen();
        string username, password;
        cout << "==================== User Registration ====================\n";
        cout << "Enter new username: ";
        cin >> username;
        
        while (checkUserExists(username)) {
            cout << "Username already exists. Please choose another one: ";
            cin >> username;
        }
        
        cout << "Enter new password: ";
        cin >> password;
        
        ofstream outFile(userFile, ios::app);
        if (outFile) {
            outFile << username << endl << password << endl;
            cout << "User registered successfully!\n";
        } else {
            cout << "Error opening file to save user!\n";
        }
    }

    bool authenticate(string& username) {
        clearScreen();
        string password;
        cout << "==================== User Login ====================\n";
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        
        ifstream inFile(userFile);
        string user, pass;
        while (inFile >> user >> pass) {
            if (user == username && pass == password) {
                return true;
            }
        }
        cout << "Invalid username or password. Please try again.\n";
        return false;
    }

    bool checkUserExists(const string& username) {
        ifstream inFile(userFile);
        string user, pass;
        while (inFile >> user >> pass) {
            if (user == username) return true;
        }
        return false;
    }
};

// ==================== Expense Tracker ====================
struct Node {
    Expense data;
    Node* next;
    Node(Expense expense) : data(expense), next(nullptr) {}
};

class ExpenseTracker {
private:
    Node* head;
    queue<string> operationHistory;
    double budget;
    string currentBudgetMonth;
    string expenseFile;
    
    void saveExpensesToFile() {
        ofstream outFile(expenseFile);
        if (!outFile) {
            cout << "Failed to open file for saving!\n";
            return;
        }
        outFile << "Budget Month: "<< currentBudgetMonth << endl;
        outFile << "Budget: " << budget << endl;
        Node* temp = head;
        while (temp != nullptr) {
            outFile << "Description: " << temp->data.description << endl
                   << "Amount: " << temp->data.amount << endl
                   << "Category: " << temp->data.category << endl
                   << "Date: " << temp->data.date << endl
                   << "-----\n";
            temp = temp->next;
        }
    }

    void loadExpensesFromFile() {
        ifstream inFile(expenseFile);
        if (!inFile) {
        	currentBudgetMonth = getCurrentMonth();
            cout << "No existing data found. Starting fresh.\n";
            return;
        }

        string line;
        //Read Budget Month
        getline(inFile, line);
        if (line.find("BudgetMonth: ") != string::npos) {
            currentBudgetMonth = line.substr(line.find(":") + 2);
        } else {
        	currentBudgetMonth = getCurrentMonth();
		}
		//Read Budget Amount
        getline(inFile, line);
        if (line.find("Budget: ") != string::npos) {
            budget = stod(line.substr(line.find(":") + 2));
        }

        string description, category, date;
        double amount;
        
        while (getline(inFile, line)) {
            if (line.find("Description:") != string::npos) {
                description = line.substr(line.find(":") + 2);
                getline(inFile, line);
                amount = stod(line.substr(line.find(":") + 2));
                getline(inFile, line);
                category = line.substr(line.find(":") + 2);
                getline(inFile, line);
                date = line.substr(line.find(":") + 2);
                getline(inFile, line); // Skip separator

                Expense newExpense(amount, description, category, date);
                Node* newNode = new Node(newExpense);
                
                if (!head) head = newNode;
                else {
                    Node* temp = head;
                    while (temp->next) temp = temp->next;
                    temp->next = newNode;
                }
            }
        }
    }
    // ==================== Simplified Structures ====================
    struct BudgetCategory {
    string name;
    double idealPercent;
    bool isEssential;
};
	// Default budget categories
	const BudgetCategory defaultCategories[8] = {
    {"Food", 15.0, true},
    {"Transport", 10.0, true},
    {"Entertainment", 5.0, false},
    {"Rent", 30.0, true},
    {"Utilities", 10.0, true},
    {"Healthcare", 10.0, true},
    {"Shopping", 5.0, false},
    {"Other", 15.0, false}
};

const int NUM_DEFAULT_CATEGORIES = 8;


public:
    ExpenseTracker(const string& username) : head(nullptr), budget(0.0) {
        expenseFile = username + "_expenses.txt";
        loadExpensesFromFile();
    }
	//Destructor
    ~ExpenseTracker() {
        saveExpensesToFile();
        Node* current = head;
        while (current) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    
    // ==================== Budget Functions ====================
    void setBudget(double newBudget) {
    	currentBudgetMonth = getCurrentMonth();
        budget = newBudget;
        operationHistory.push("Set Budget for " + currentBudgetMonth + ": $" + to_string(budget));
        if (operationHistory.size() > 5) operationHistory.pop();
        saveExpensesToFile();
        cout << "Budget set to: " << budget << "for " << currentBudgetMonth << endl;
    }
    
    void checkBudget() {
        double totalExpenses = 0.0;
        string currentMonth = getCurrentMonth();
        //Only sum expenses for current month
        Node* temp = head;
        while (temp) {
        	if (temp->data.date.substr(0, 7) == currentMonth){
        		totalExpenses += temp->data.amount;
			}
            temp = temp->next;
        }

        clearScreen();
        cout << "==================== Budget Status (" <<currentMonth << ") ====================\n";
        cout << "Total Expenses: $" << fixed << setprecision(2) << totalExpenses << endl;
        cout << "Budget: $" << budget << endl;
        
        if (totalExpenses > budget) {
            cout << "You have exceeded your budget by $" << (totalExpenses - budget) << "!\n";
        } else if (totalExpenses == budget) {
            cout << "You have reached your budget limit.\n";
        } else {
            cout << "You have $" << (budget - totalExpenses) << " remaining in your budget.\n";
        }
        cout << "======================================================\n";
        
        operationHistory.push("Checked Budget for " + currentMonth);
        if (operationHistory.size() > 5) operationHistory.pop();
    }

    // ==================== Expense Management ====================
	void addExpense(double amount, const string& description) {
    	string category = getCategoryFromUser();
    	string date = getCurrentDate(); // Default to current date
    
    	cout << "Do you want to enter a different date? (y/n): ";
    	char changeDate;
    	cin >> changeDate;
    	cin.ignore();
    
    	if (changeDate == 'y' || changeDate == 'Y') {
        	while (true) {
            	cout << "Enter date (YYYY-MM-DD): ";
            	getline(cin, date);
            	if (isValidDate(date)) break;
            	cout << "Invalid date format. Please enter the date in YYYY-MM-DD format.\n";
        	}
    	}
    
    	Expense newExpense(amount, description, category, date);
    	Node* newNode = new Node(newExpense);
    
    	if (!head) head = newNode;
    	else {
    	    Node* temp = head;
    	    while (temp->next) temp = temp->next;
    	    temp->next = newNode;
    	}
    
    	// Check if expense is in current budget month
    	if (date.substr(0, 7) == getCurrentMonth() && budget > 0) {
        	double total = 0.0;
        	Node* temp = head;
        	while (temp) {
        	    if (temp->data.date.substr(0, 7) == getCurrentMonth()) {
        	        total += temp->data.amount;
        	    }
        	    temp = temp->next;
        	}
        
        	if (total > budget) {
        	    cout << "WARNING: This expense exceeds your monthly budget!\n";
        	    cout << "Budget: $" << budget << " | Current Spending: $" << total << "\n";
        	} else if (total > budget * 0.8) {
        	    cout << "NOTE: You've used " << (total/budget)*100 << "% of your budget\n";
        	}
    	}
    
    	operationHistory.push("Added Expense: " + description + " - $" + to_string(amount) + " in " + category);
    	saveExpensesToFile();
    	cout << "Expense added successfully!\n";
	}

    void removeExpense(const string& description) {
        if (!head) {
            cout << "No expenses found!\n";
            return;
        }

        Node* current = head;
        int matchIndex = 1;
        bool found = false;

        clearScreen();
        cout << "==================== Matching Expenses ====================\n";
        while (current) {
            if (current->data.description == description) {
                cout << "[" << matchIndex++ << "] ";
                current->data.display();
                found = true;
            }
            current = current->next;
        }

        if (!found) {
            cout << "No expense found with description: " << description << endl;
            return;
        }

        int choice;
		choice = getValidatedChoice();
        if (choice < 1 || choice >= matchIndex) {
            cout << "Invalid choice. Deletion canceled.\n";
            return;
        }

        current = head;
        matchIndex = 1;
        Node* selected = nullptr;
        Node* prev = nullptr;

        while (current) {
            if (current->data.description == description && matchIndex++ == choice) {
                selected = current;
                break;
            }
            prev = current;
            current = current->next;
        }

        char confirm;
        cout << "Are you sure you want to delete this expense? (y/n): ";
        cin >> confirm;
        cin.ignore();

    	if (confirm == 'y' || confirm == 'Y') {
        	// Check if deleted expense was in current month
        	if (selected->data.date.substr(0, 7) == getCurrentMonth()) {
        	    double newTotal = 0.0;
        	    Node* temp = head;
            	while (temp) {
                	if (temp != selected && temp->data.date.substr(0, 7) == getCurrentMonth()) {
                    	newTotal += temp->data.amount;
                	}
                	temp = temp->next;
            	}
            	
            	cout << "Budget update: Remaining for " << getCurrentMonth() 
            	     << ": $" << (budget - newTotal) << "\n";
        	}
    	}

        if (confirm != 'y' && confirm != 'Y') {
            cout << "Deletion canceled.\n";
            return;
        }

        if (selected == head) {
            head = head->next;
        } else {
            prev->next = selected->next;
        }

        delete selected;
        cout << "Expense deleted successfully!\n";

        operationHistory.push("Removed Expense: " + description);
        if (operationHistory.size() > 5) operationHistory.pop();
        saveExpensesToFile();
    }
    void editExpense(const string& description) {
        if (!head) {
            cout << "No expenses found!\n";
            return;
        }

        clearScreen();
        cout << "==================== Edit Expense ====================\n";
        Node* current = head;
        int matchIndex = 1;

        cout << "Expenses with description: " << description << endl;
        while (current) {
            if (current->data.description == description) {
                cout << "[" << matchIndex++ << "] ";
                current->data.display();
            }
            current = current->next;
        }

        if (matchIndex == 1) {
            cout << "No expense found with description: " << description << endl;
            return;
        }

        int choice;
		choice = getValidatedChoice();
        current = head;
        matchIndex = 1;
        Node* selected = nullptr;
        while (current) {
            if (current->data.description == description && matchIndex++ == choice) {
                selected = current;
                break;
            }
            current = current->next;
        }

        if (!selected) {
            cout << "Invalid choice. Edit canceled.\n";
            return;
        }

        double newAmount;
        string newDescription, newCategory, newDate;
        
        newAmount = validatedAmount();
        cin.ignore();

        do {
                    
           cout << "Enter new description: ";
           getline(cin, newDescription);
           if (newDescription.empty()) {
              cout << "Error: Description cannot be empty.\n";
              }
        } while (newDescription.empty());

        newCategory = getCategoryFromUser();

        cout << "Do you want to change the date? (y/n): ";
        char changeDate;
        cin >> changeDate;
        cin.ignore();

        if (changeDate == 'y' || changeDate == 'Y') {
            while (true) {
                cout << "Enter new date (YYYY-MM-DD): ";
                getline(cin, newDate);
                if (isValidDate(newDate)) break;
                cout << "Invalid date format. Please enter the date in YYYY-MM-DD format.\n";
            }
        } else {
            newDate = selected->data.date;
        }

        selected->data.amount = newAmount;
        selected->data.description = newDescription;
        selected->data.category = newCategory;
        selected->data.date = newDate;

    	// Check if date changed to/from current month
    	string oldMonth = selected->data.date.substr(0, 7);
    	string newMonth = newDate.substr(0, 7);
    	string currentMonth = getCurrentMonth();

    	if (oldMonth != newMonth && (oldMonth == currentMonth || newMonth == currentMonth)) {
        	checkBudget(); // Refresh budget display
    	}

        cout << "Expense updated successfully!\n";

        operationHistory.push("Edited Expense: " + description);
        if (operationHistory.size() > 5) operationHistory.pop();
        saveExpensesToFile();
    }

    // ==================== View Functions ====================
    void viewAllExpenses(bool currentMonthOnly = false) {
        clearScreen();
        Node* temp = head;
        double totalAmount = 0.0;
        string currentMonth = getCurrentMonth();

        cout << "\n==================== " << (currentMonthOnly ? "Current Month Expenses" : "All Expenses") << " ====================\n";
        cout << "| Description     | Amount  | Category  | Date       |\n";
        cout << "-------------------------------------------------------\n";

        while (temp) {
        	if (!currentMonthOnly || temp->data.date.substr(0, 7) == currentMonth) {
            cout << "| " << left << setw(16) << temp->data.description
                 << "| $" << right << setw(7) << fixed << setprecision(2) << temp->data.amount
                 << "| " << left << setw(10) << temp->data.category
                 << "| " << temp->data.date << " |\n";
            totalAmount += temp->data.amount;
        }
            temp = temp->next;
        }

        cout << "-------------------------------------------------------\n";
        cout << "Total Expenses: $" << fixed << setprecision(2) << totalAmount << endl;
        if (currentMonthOnly) {
        	cout << "Budget: $" << budget << endl;
        	cout << "Remaining: $" << (budget - totalAmount) << endl;
		}
    }

    void viewExpensesByCategory(const string& category, bool currentMonthOnly = false) {
    	clearScreen();
    	Node* temp = head;
    	double totalCategoryAmount = 0.0;
    	bool found = false;
    	string currentMonth = getCurrentMonth();

    	cout << "\n==================== Expenses in Category: " << category;
    	if (currentMonthOnly) cout << " (" << currentMonth << ")";
    	cout << " ====================\n";
    	cout << "| Description     | Amount  | Date       |\n";
    	cout << "-----------------------------------------\n";

    	while (temp) {
        	if (temp->data.category == category && (!currentMonthOnly || temp->data.date.substr(0, 7) == currentMonth)) {
            	cout << "| " << left << setw(16) << temp->data.description
                	<< "| $" << right << setw(7) << fixed << setprecision(2) << temp->data.amount
                 	<< "| " << temp->data.date << " |\n";
            	totalCategoryAmount += temp->data.amount;
            	found = true;
        	}
        	temp = temp->next;
    	}

    	if (!found) {
        	cout << "No expenses found for category: " << category;
        	if (currentMonthOnly) cout << " in " << currentMonth;
        	cout << endl;
    	} else {
        	cout << "-----------------------------------------\n";
        	cout << "Total in " << category << ": $" << totalCategoryAmount;
        	if (currentMonthOnly && budget > 0) {
            	cout << " (" << (totalCategoryAmount/budget)*100 << "% of monthly budget)";
        	}
        	cout << endl;
    	}
	}

    // ==================== SEARCH Functions ====================
    void searchExpenseByDescription(const string& description) {
    	clearScreen();
    	cout << "Search in current month only? (y/n): ";
    	char filter;
    	cin >> filter;
    	cin.ignore();
    
    	bool currentMonthOnly = (filter == 'y' || filter == 'Y');
    	string currentMonth = getCurrentMonth();
    
    	Node* temp = head;
    	bool found = false;
    	double total = 0.0;

    	cout << "==================== Search Results ====================\n";
    	if (currentMonthOnly) {
    	    cout << "(Current month: " << currentMonth << ")\n";
    	}
    	
    	while (temp) {
    	    if (temp->data.description.find(description) != string::npos && 
    	        (!currentMonthOnly || temp->data.date.substr(0, 7) == currentMonth)) {
    	        temp->data.display();
    	        total += temp->data.amount;
    	        found = true;
    	    }
    	    temp = temp->next;
    	}

    	if (found) {
    	    cout << "\nTotal found: $" << total;
    	    if (currentMonthOnly && budget > 0) {
    	        cout << " (" << (total/budget)*100 << "% of monthly budget)";
    	    }
    	    cout << endl;
    	} else {
    	    cout << "No matching expenses found";
    	    if (currentMonthOnly) cout << " in " << currentMonth;
    	    cout << endl;
    	}
	}
	//==========SORTING FUNCTIONS==========
    void sortExpensesByAmount(bool ascending = true) {
        if (!head || !head->next) return;

        bool swapped;
        do {
            swapped = false;
            Node* current = head;
            while (current && current->next) {
                if ((ascending && current->data.amount > current->next->data.amount) ||
                    (!ascending && current->data.amount < current->next->data.amount)) {
                    swap(current->data, current->next->data);
                    swapped = true;
                }
                current = current->next;
            }
        } while (swapped);

        cout << "Expenses sorted by amount:\n";
        viewAllExpenses();
        operationHistory.push("Sorted Expenses by Amount");
        if (operationHistory.size() > 5) operationHistory.pop();
        saveExpensesToFile();
    }
    
    void sortExpensesByDate(bool ascending = true) {
        if (!head || !head->next) return;

        bool swapped;
        do {
            swapped = false;
            Node* current = head;
            while (current && current->next) {
                if ((ascending && current->data.date > current->next->data.date) ||
                    (!ascending && current->data.date < current->next->data.date)) {
                    swap(current->data, current->next->data);
                    swapped = true;
                }
                current = current->next;
            }
        } while (swapped);

        cout << "Expenses sorted by date:\n";
        viewAllExpenses();
        operationHistory.push("Sorted Expenses by Date");
        if (operationHistory.size() > 5) operationHistory.pop();
        saveExpensesToFile();
    }
	//==========SUMMARY FUNCTIONS==========
    void viewTotalExpenseSummary() {
        clearScreen();
        double total = 0.0;
        Node* temp = head;

        while (temp) {
            total += temp->data.amount;
            temp = temp->next;
        }

        cout << "==================== Expense Summary ====================\n";
        cout << "Total expenses: $" << fixed << setprecision(2) << total << endl;
        cout << "========================================================\n";
    }
    
    void viewMonthlySummary() {
    	clearScreen();
    	string months[12];
    	double monthlyTotals[12] = {0};
    	int currentYear = stoi(getCurrentMonth().substr(0, 4));
    
    	// Initialize month labels
    	for (int i = 0; i < 12; i++) {
    	    char buffer[8];
    	    struct tm tm = {0};
    	    tm.tm_year = currentYear - 1900;
    	    tm.tm_mon = i;
    	    strftime(buffer, sizeof(buffer), "%Y-%m", &tm);
    	    months[i] = buffer;
    	}
    
    	// Calculate monthly totals
    	Node* temp = head;
    	while (temp) {
    	    string month = temp->data.date.substr(0, 7);
    		for (int i = 0; i < 12; i++) {
        	    if (months[i] == month) {
        	        monthlyTotals[i] += temp->data.amount;
        	        break;
        	    }
        	}
        	temp = temp->next;
    	}
    
    	// Display summary
    	cout << "==================== Monthly Summary (" << currentYear << ") ====================\n";
    	cout << "| Month    | Expenses  |\n";
    	cout << "-------------------------\n";
    
    	for (int i = 0; i < 12; i++) {
    	    cout << "| " << months[i] << " | $" << right << setw(9) << fixed << setprecision(2) << monthlyTotals[i] << " |\n";
    	}
    	cout << "-------------------------\n";
    
    	// Find min/max months
    	int minMonth = 0, maxMonth = 0;
    	for (int i = 1; i < 12; i++) {
    	    if (monthlyTotals[i] < monthlyTotals[minMonth]) minMonth = i;
    	    if (monthlyTotals[i] > monthlyTotals[maxMonth]) maxMonth = i;
    	}
    
    	cout << "Highest Spending: " << months[maxMonth] << " ($" << monthlyTotals[maxMonth] << ")\n";
    	cout << "Lowest Spending: " << months[minMonth] << " ($" << monthlyTotals[minMonth] << ")\n";
    	cout << "======================================================\n";
	}
    
    void viewBudgetSummary() {
    	clearScreen();
    	string currentMonth = getCurrentMonth();
    	double totalExpenses = 0.0;
    	double prevMonthExpenses = 0.0;
    	Node* temp = head;

    	while (temp) {
        	if (temp->data.date.substr(0, 7) == currentMonth) {
            	totalExpenses += temp->data.amount;
        	} else if (temp->data.date.substr(0, 7) == getPreviousMonth()) {
            	prevMonthExpenses += temp->data.amount;
        	}
        	temp = temp->next;
    	}

    	double remainingBudget = budget - totalExpenses;
    	double percentageUsed = (totalExpenses / budget) * 100;

    	cout << "==================== Budget Summary ====================\n";
    	cout << "Current Month: " << currentMonth << "\n";
    	cout << "Budget: $" << fixed << setprecision(2) << budget << "\n";
    	cout << "Spent This Month: $" << totalExpenses << "\n";
    	cout << "Remaining Budget: $" << remainingBudget << "\n";
    	cout << "Percentage Used: " << percentageUsed << "%\n";
    
    	if (prevMonthExpenses > 0) {
    	    cout << "\nPrevious Month (" << getPreviousMonth() << ") Spending: $" << prevMonthExpenses << "\n";
    	    double difference = totalExpenses - prevMonthExpenses;
    	    if (difference > 0) {
    	        cout << "You're spending " << difference << " more than last month\n";
    	    } else {
    	        cout << "You're spending " << -difference << " less than last month\n";
    	    }
    	}
    	cout << "======================================================\n";
	}

	// Add this helper function to ExpenseTracker class
	string getPreviousMonth() {
    	time_t t = time(0);
    	struct tm* now = localtime(&t);
    
    	// Handle January case
    	if (now->tm_mon == 0) {
        	now->tm_year--;
        	now->tm_mon = 11;
    	} else {
        	now->tm_mon--;
    	}
    
    	char buffer[8];
    	strftime(buffer, sizeof(buffer), "%Y-%m", now);
    	return string(buffer);
	}
	//===============OTHER FUNCTIONS===============
    void viewOperationHistory() {
        clearScreen();
        cout << "\n==================== Recent Operations ====================\n";
        cout << "| Operation                                                  |\n";
        cout << "-------------------------------------------------------------\n";

        queue<string> tempQueue = operationHistory;
        while (!tempQueue.empty()) {
            cout << "| " << left << setw(60) << tempQueue.front() << "|\n";
            tempQueue.pop();
        }
        cout << "-------------------------------------------------------------\n";
    }

    void clearAllExpenses() {
    	clearScreen();
    	cout << "==================== Clear Expenses ====================\n";
    	cout << "1. Clear ALL expenses\n";
    	cout << "2. Clear current month's expenses (" << getCurrentMonth() << ")\n";
    	cout << "0. Cancel\n";
    	int choice = getValidatedChoice();
    	if (choice == 1) {
        	char confirm;         
			cout << "==================== Clear All Expenses ====================\n";         
			cout << "Are you sure you want to delete ALL expenses? (y/n): ";         
			cin >> confirm;                  
			if (confirm == 'y' || confirm == 'Y') {             
			while (head) {                 
			Node* temp = head;                 
			head = head->next;                 
			delete temp;             
			}             
			cout << "All expenses deleted successfully!\n";             
			saveExpensesToFile();         
			} else {            
		 	cout << "Deletion canceled.\n";         
			} 
    	} 
    	else if (choice == 2) {
        	char confirm;
        	cout << "Clear ALL expenses for " << getCurrentMonth() << "? (y/n): ";
        	cin >> confirm;
        
        	if (confirm == 'y' || confirm == 'Y') {
        	    Node* current = head;
        	    Node* prev = nullptr;
        	    
            	while (current) {
            	    if (current->data.date.substr(0, 7) == getCurrentMonth()) {
            	        if (prev) {
                	        prev->next = current->next;
                    	    delete current;
                    	    current = prev->next;
                    	} else {
                    	    head = current->next;
                    	    delete current;
                    	    current = head;
                    	}
                	} else {
                	    prev = current;
                	    current = current->next;
                	}
            	}
            	cout << "All expenses for " << getCurrentMonth() << " cleared!\n";
            	saveExpensesToFile();
        	}
    	}
	}
    
    // ==================== Simplified Analysis Function ====================
void generateBudgetSuggestions() {
    clearScreen();
    
    if (budget <= 0) {
        cout << "Please set a budget first.\n";
        return;
    }

    // 1. Calculate spending by category for current month
    double categorySpending[NUM_DEFAULT_CATEGORIES] = {0};
    double customCategorySpending[20] = {0};
    string customCategoryNames[20];
    int numCustomCategories = 0;
    double totalSpent = 0.0;
    string currentMonth = getCurrentMonth();
    
    Node* current = head;
    while (current) {
        if (current->data.date.substr(0, 7) == currentMonth) {
            bool isDefaultCategory = false;
            
            // Check if it's a default category
            for (int i = 0; i < NUM_DEFAULT_CATEGORIES; i++) {
                if (current->data.category == defaultCategories[i].name) {
                    categorySpending[i] += current->data.amount;
                    isDefaultCategory = true;
                    break;
                }
            }
            
            // If not default, track as custom category
            if (!isDefaultCategory) {
                bool found = false;
                for (int i = 0; i < numCustomCategories; i++) {
                    if (current->data.category == customCategoryNames[i]) {
                        customCategorySpending[i] += current->data.amount;
                        found = true;
                        break;
                    }
                }
                
                if (!found && numCustomCategories < 20) {
                    customCategoryNames[numCustomCategories] = current->data.category;
                    customCategorySpending[numCustomCategories] = current->data.amount;
                    numCustomCategories++;
                }
            }
            
            totalSpent += current->data.amount;
        }
        current = current->next;
    }

    // 2. Display results
    cout << "\n=== BUDGET SUGGESTIONS FOR " << currentMonth << " ===\n";
    cout << "Total Budget: $" << fixed << setprecision(2) << budget 
         << " | Spent: $" << totalSpent << "\n\n";
    
    // Display default categories
    cout << "ESSENTIAL CATEGORIES:\n";
    cout << "+---------------------+------------+------------+----------------+\n";
    cout << "| Category            | Spent      | Ideal      | Recommendation  |\n";
    cout << "+---------------------+------------+------------+----------------+\n";
    
    for (int i = 0; i < NUM_DEFAULT_CATEGORIES; i++) {
        if (!defaultCategories[i].isEssential) continue;
        
        double idealAmount = budget * (defaultCategories[i].idealPercent / 100);
        cout << "| " << left << setw(19) << defaultCategories[i].name
             << "| $" << right << setw(9) << fixed << setprecision(2) << categorySpending[i]
             << " | $" << setw(9) << idealAmount << " | ";
        
        if (categorySpending[i] > idealAmount * 1.2) {
            cout << "Reduce spending";
        } else if (categorySpending[i] > idealAmount) {
            cout << "Monitor spending";
        } else if (categorySpending[i] < idealAmount * 0.8) {
            cout << "You can spend more";
        } else {
            cout << "On track";
        }
        cout << " |\n";
    }
    cout << "+---------------------+------------+------------+----------------+\n";

    // Display non-essential categories
    cout << "\nNON-ESSENTIAL CATEGORIES:\n";
    cout << "+---------------------+------------+------------+----------------+\n";
    cout << "| Category            | Spent      | Ideal      | Recommendation  |\n";
    cout << "+---------------------+------------+------------+----------------+\n";
    
    for (int i = 0; i < NUM_DEFAULT_CATEGORIES; i++) {
        if (defaultCategories[i].isEssential) continue;
        
        double idealAmount = budget * (defaultCategories[i].idealPercent / 100);
        cout << "| " << left << setw(19) << defaultCategories[i].name
             << "| $" << right << setw(9) << categorySpending[i]
             << " | $" << setw(9) << idealAmount << " | ";
        
        if (categorySpending[i] > idealAmount * 1.3) {
            cout << "Reduce significantly";
        } else if (categorySpending[i] > idealAmount) {
            cout << "Consider reducing";
        } else {
            cout << "Within limits";
        }
        cout << " |\n";
    }
    cout << "+---------------------+------------+------------+----------------+\n";

    // Display custom categories if they exist
    if (numCustomCategories > 0) {
        cout << "\nCUSTOM CATEGORIES:\n";
        cout << "+---------------------+------------+\n";
        cout << "| Category            | Spent      |\n";
        cout << "+---------------------+------------+\n";
        
        for (int i = 0; i < numCustomCategories; i++) {
            cout << "| " << left << setw(19) << customCategoryNames[i]
                 << "| $" << right << setw(9) << customCategorySpending[i] << " |\n";
        }
        cout << "+---------------------+------------+\n";
    }

    // Budget summary
    cout << "\nBUDGET SUMMARY:\n";
    if (totalSpent > budget) {
        cout << "WARNING: You've exceeded your budget by $" 
             << (totalSpent - budget) << "!\n";
        cout << "Focus on reducing non-essential spending.\n";
    } else {
        cout << "You have $" << (budget - totalSpent) 
             << " remaining in your budget this month.\n";
        
        if ((budget - totalSpent) < budget * 0.2) {
            cout << "NOTE: You have less than 20% of your budget remaining.\n";
        }
    }
}

    
    //==========Loan Payment==========
    void makeLoanPayment() {
        clearScreen();
        
        // Check if budget is set
        if (budget <= 0) {
            cout << "ERROR: No budget set. Please set a budget first.\n";
            cout << "\nPress Enter to return to menu...";
            cin.ignore();
            return;
        }

        // Display current budget status
        cout << "===========================================\n";
        cout << "          MAKE LOAN PAYMENT\n";
        cout << "===========================================\n";
        cout << " Current Budget: $" << fixed << setprecision(2) << budget << "\n\n";

        // Get payment percentage with validation
        double percentage;
        while (true) {
            cout << "Enter percentage of budget to pay (0-100): ";
            if (!(cin >> percentage)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            
            if (percentage < 0 || percentage > 100) {
                cout << "Percentage must be between 0 and 100.\n";
                continue;
            }

            // Check if payment leaves reasonable budget
            double remaining = budget - (percentage/100)*budget;
            if (remaining < budget*0.2) { // Less than 20% remains
                cout << "WARNING: This will leave only $" << remaining 
                     << " (" << (remaining/budget)*100 << "%) of your budget.\n";
                cout << "Are you sure? (y/n): ";
                char confirm;
                cin >> confirm;
                if (tolower(confirm) != 'y') {
                    cout << "Please enter a different percentage.\n";
                    continue;
                }
            }
            break;
        }

        // Calculate payment amount
        double paymentAmount = (percentage / 100) * budget;
        string currentDate = getCurrentDate();

        // Payment confirmation screen
        clearScreen();
        cout << "----------------------------------------------------|\n";
        cout << "|           CONFIRM LOAN PAYMENT                    |\n";
        cout << "----------------------------------------------------|\n";
        cout << "|  Payment Amount:  $" << setw(10) << left << paymentAmount << " (" 
             << setw(5) << percentage << "% of budget)  |\n";
        cout << "|  Remaining Budget: $" << setw(10) << left << (budget - paymentAmount) 
             << "                    |\n";
        cout << "|  Payment Date:     " << setw(10) << left << currentDate 
             << "                     |\n";
        cout << "|---------------------------------------------------|\n";
        cout << "|  [Y] Confirm Payment                              |\n";
        cout << "|  [N] Cancel                                       |\n";
        cout << "----------------------------------------------------|\n";
       
        cout << "Enter choice: ";
        
        char confirm;
        cin >> confirm;
        cin.ignore();

        if (tolower(confirm) == 'y') {
            // Create and add loan payment expense
            Expense loanPayment(paymentAmount, "Loan Repayment", "Debt Payments", currentDate);
            Node* newNode = new Node(loanPayment);
            
            // Add to linked list
            if (!head) {
                head = newNode;
            } else {
                Node* temp = head;
                while (temp->next) temp = temp->next;
                temp->next = newNode;
            }

            // Update budget and save
            budget -= paymentAmount;
            operationHistory.push("Paid $" + to_string(paymentAmount) + " towards loan");
            if (operationHistory.size() > 5) operationHistory.pop();
            saveExpensesToFile();
            
            // Payment receipt
            clearScreen();
            cout << "-------------------------------------------\n";
            cout << "            PAYMENT CONFIRMED\n";
            cout << "-------------------------------------------\n";
            cout << " Amount:       $" << setw(10) << paymentAmount << "\n";
            cout << " Category:     Debt Payment\n";
            cout << " Date:         " << currentDate << "\n";
            cout << " Remaining:    $" << setw(10) << budget << "\n";
            cout << "-------------------------------------------\n";
            cout << "Note: This has been recorded as an expense.\n";
        } else {
            cout << "\nPayment canceled. No changes were made.\n";
        }
        
    }
};

// ==================== Main Function ====================
int main() {
    LoginSystem loginSystem;
    int choice;
    string username;
    
    // Initial screen
    clearScreen();
    while (true) {
        cout << "==================== Welcome to Expense Tracker ====================\n";
        cout << "1. Create Account\n";
        cout << "2. Log In\n";
        cout << "0. Exit\n";
        choice = getValidatedChoice();
        
        if (choice == 1) {
            loginSystem.registerUser();
            clearScreen();
        } else if (choice == 2) {
            if (loginSystem.authenticate(username)) break;
        } else if (choice == 0) {
            return 0;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    ExpenseTracker tracker(username);
    
    while (true) {
        clearScreen();
        cout << "==================== Expense Tracker Menu ====================\n";
        cout << "1.  Add Expense\n";
        cout << "2.  Remove Expense\n";
        cout << "3.  View Expenses\n";
        cout << "4.  Edit Expense\n";
        cout << "5.  Search Expense by Description\n";
        cout << "6.  Sort Expenses \n";
        cout << "7.  Set Budget\n";
        cout << "8.  Check Budget\n";
        cout << "9.  Make Loan Payment\n";
        cout << "10. Get Smart Budget Suggestions\n";
        cout << "11. View Total Expense Summary\n";
        cout << "12. View Budget Summary\n";
        cout << "13. View Operation History\n";
        cout << "14. View Monthly Summary\n";
        cout << "15. Clear All Expenses\n";
        cout << "16. Display Help\n";
        cout << "0.  Exit\n";
        cout << "=============================================================\n";
		choice = getValidatedChoice();
        switch (choice) {
            case 1: {
                while (true) {
                    double amount;
                    string description;
                    
                    clearScreen();
                    cout << "==================== Add Expense ====================\n";
                    amount = validatedAmount();
                    cin.ignore();
                    do {
                         cout << "Enter description: ";
                         getline(cin, description);
                         if (description.empty()) {
                              cout << "Error: Description cannot be empty.\n";
                         }
                    } while (description.empty());
    
                    tracker.addExpense(amount, description); 
                    char addMore;
                    cout << "Add another expense? (y/n): ";
                    cin >> addMore;
                    if (addMore != 'y' && addMore != 'Y') break;
                }
                break;
            }
            case 2: {
                clearScreen();
                string description;
                cout << "==================== Remove Expense ====================\n";
                cout << "Enter description of expense to remove: ";
                getline(cin, description);
                tracker.removeExpense(description);
                break;
            }
            case 3:
            	int subChoice;
            		cout << "1. All Expenses\n";
            		cout << "2. Current Month Expenses\n";
            		cout << "3. Expenses By Category\n";
            		subChoice = getValidatedChoice();
            		switch (subChoice)
            		{
            			case 1: 
							tracker.viewAllExpenses(); break;
            			case 2: tracker.viewAllExpenses(true); break;
            			case 3:{
            				char choice;
                			string category = getCategoryFromUser();
                			cout << "Show for current month only? (y/n): ";
                			cin >> choice;
                			tracker.viewExpensesByCategory(category, choice == 'y' || choice == 'Y');
               				break;
               			}
               			default: cout << "Invalid Option!\n";		  
					}
					break;
            case 4: {
                clearScreen();
                string description;
                cout << "==================== Edit Expense ====================\n";
                cout << "Enter description of expense to edit: ";
                getline(cin, description);
                tracker.editExpense(description);
                break;
            }
            case 5: {
                clearScreen();
                string description;
                cout << "==================== Search Expense ====================\n";
                cout << "Enter description to search for: ";
                cin.ignore();
                getline(cin, description);
                tracker.searchExpenseByDescription(description);
                break;
            }
            case 6: {
                clearScreen();
                bool ascending;
                cout << "==================== Sort Expenses ====================\n";
                cout << "1. By Amount\n";
                cout << "2. By Date\n";
                int subChoice = getValidatedChoice();
                cout << "Sort in ascending order (1) or descending order (0): ";
                cin >> ascending;
                if (subChoice == 1)
                tracker.sortExpensesByAmount(ascending);
                else if (subChoice == 2)
                tracker.sortExpensesByDate();
                else
                cout << "Invalid Option!\n";
                break;
            }
            case 7: {
                clearScreen();
                double amount;
                cout << "==================== Set Monthly Budget ====================\n";
                cout << "Setting Budget for "<< getCurrentMonth() << endl;
                amount = validatedAmount();
                tracker.setBudget(amount);
                break;
            }
            case 8:
                tracker.checkBudget();
                break;
            case 9:
                tracker.makeLoanPayment();
        		break;
            case 10:
                tracker.generateBudgetSuggestions();
            	break;
            case 11:
                tracker.viewTotalExpenseSummary();
                break;
            case 12:
                tracker.viewBudgetSummary();
                break;
            case 13:
                tracker.viewOperationHistory();
                break;
            case 14:
            	tracker.viewMonthlySummary();
            	break;
            case 15:
            	tracker.clearAllExpenses();
                break;
        	case 16:
        		displayHelp();
                break;
            case 0:
                cout << "Exiting the program. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }  
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    return 0;
}