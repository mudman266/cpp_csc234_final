// Josh Williams
// 4/23/2020
// CSC - 234 - 81A
// Final Project

// Write a program that uses a structure to store the following information about a customer account:
// Name, Address, City, State, Zip, Telephone, Account balance, Date of last payment.
// Use the structure to store customer account records in a file.
// Print a menu that lets the user choose their option.
// VALIDATION: All fields must be entered for a new account. No negative balances should be entered.

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cctype> // For toUpper

using namespace std;

// Global Varibles
const string FILENAME = "C:\\cabs\\customers.txt";
const int ADDRESS_SIZE = 50;

// Account structure to group records
struct account {
    int index = -1;
    char name[ADDRESS_SIZE] = "";
    char address[ADDRESS_SIZE] = "";
    char city[ADDRESS_SIZE] = "";
    char state[ADDRESS_SIZE] = "";
    char zip[ADDRESS_SIZE] = "";
    char telephone[ADDRESS_SIZE] = "";
    char last_payment_date[ADDRESS_SIZE] = "";
    double balance = 0.00;
};

// Function declarations
int showMenu();
account searchFile();
void addNewRecord();
void displayRecord();
void editRecord();
void displayFile();
void deleteRecord();
void printRecord(account record);
long byteNum(int indexNum);
account gatherInput();

int main()
{
    // Variables
    int selection;
    
    // Welcome message
    cout << "Customer Accounts\n\n";

    
    while (true) {
        // Show the menu
        selection = showMenu();
        if (selection == 1) {
            // Enter new record
            addNewRecord();
        }
        else if (selection == 2) {
            // Display record
            displayRecord();
        }
        else if (selection == 3) {
            // Delete record
            deleteRecord();
        }
        else if (selection == 4) {
            // Edit record
            editRecord();
        }
        else if (selection == 5) {
            // Print the entire file to the screen
            displayFile();
        }
        else if (selection == 6) {
            // Exit
            exit(0);
        }
    }
}

int showMenu() {
    // Prints the selection menu and returns the choice made by the user
    int choice = 0;
    printf("Menu\n");
    printf("1 - Enter New Records\n");
    printf("2 - Display Record\n");
    printf("3 - Delete Record\n");
    printf("4 - Edit Record\n");
    printf("5 - Display Entire File\n");
    printf("6 - Exit Program\n");
    printf("Selection: ");
    cin >> choice;

    // Verify choice
    while (choice < 1 || choice > 6) {
        printf("Enter a valid selection (1-6): ");
        cin >> choice;
    }
    return choice;
}

void addNewRecord() {
    // Adds an entirely new record to the file.
    
    account toAdd;
    toAdd = gatherInput();

    // === Begin Index Handler ===

    account searching;
    fstream customers(FILENAME);
    customers.seekg(0, customers.end);
    __int64 length = customers.tellg();
    customers.seekg(length - sizeof(account), ios_base::beg);
    customers.read(reinterpret_cast<char*>(&searching), sizeof(searching));
    toAdd.index = (searching.index == -1) ? 0 : searching.index + 1;
    customers.close();

    // === End Index Handler ===

    // Write to file
    fstream cust(FILENAME, ios::out | ios::app | ios::binary);
    cust.write(reinterpret_cast<char*>(&toAdd), sizeof(toAdd));
    cust.close();
}

account gatherInput() {
    // Gathers the input for a record and returns a structure
    string input;
    account newCustomer;
    cin.ignore();
    cout << "Name: ";
    getline(cin, input);
    strcpy_s(newCustomer.name, input.c_str());
    cout << "Address: ";
    getline(cin, input);
    strcpy_s(newCustomer.address, input.c_str());
    cout << "City: ";
    getline(cin, input);
    strcpy_s(newCustomer.city, input.c_str());
    cout << "State: ";
    getline(cin, input);
    strcpy_s(newCustomer.state, input.c_str());
    cout << "Zip: ";
    getline(cin, input);
    strcpy_s(newCustomer.zip, input.c_str());
    cout << "Telephone: ";
    getline(cin, input);
    strcpy_s(newCustomer.telephone, input.c_str());
    cout << "Balance: ";
    cin >> newCustomer.balance;

    // Check for negative balance
    if (newCustomer.balance < 0.0) {
        // Negative balance found. Setting to 0.00
        printf("Invalid balance entered. Setting to $0.00");
        newCustomer.balance = 0.0;
    }

    // === Begin Date Handler ===

    time_t cur_time;
    struct tm cur_tm; // Holds info returned from localtime_s()
    time(&cur_time);
    localtime_s(&cur_tm, &cur_time);
    strftime(newCustomer.last_payment_date, 50, "%m/%d/%Y", &cur_tm);

    // === End Date Handler ===
    return newCustomer;
}

void displayRecord() {
    // Searches the file for matching information and displays the record if found.
    account records = searchFile();
    if (records.index < 0) {
        printf("Index not found.\n");
    }
    else {
        printRecord(records);
    }
}

void editRecord() {
    // Edits information for a single record.

    account toEdit;
    account toUpdate;
    toEdit = searchFile();
    printRecord(toEdit);
    printf("\nEnter new info\n");
    toUpdate = gatherInput();
    toUpdate.index = toEdit.index;
    // Write the new info to the file
    fstream custFile(FILENAME, ios::binary | ios::in | ios::out);
    if (custFile) {
        custFile.seekp(byteNum(toUpdate.index), custFile.beg);
        custFile.write(reinterpret_cast<char*>(&toUpdate), sizeof(toUpdate));
        custFile.close();
    }
    else printf("Failed to open file.\n");
}

account searchFile() {
    // Searches a file of structures and returns the matching record

    int searchIndex;
    account searching;

    // Gather search info from user
    printf("Enter the index number: ");
    cin >> searchIndex;
    // Open the file
    fstream customers(FILENAME, ios::in | ios::binary);

    if (customers) {
        // Skip to the index provided
        customers.seekg(byteNum(searchIndex), ios_base::beg);
        customers.read(reinterpret_cast<char*>(&searching), sizeof(searching));
        customers.close();
        return searching;
        }
    customers.close();
    // No match found. Set index to -1
    searching.index = -1;
    return searching;
}

long byteNum(int indexNum) {
    // Accepts an integer and returns the byte number in the file for the index provided.
    return sizeof(account) * (indexNum);
}

void displayFile() {
    // Displays the entire contents of the file.

    account holder;
    fstream cust(FILENAME, ios::in | ios::binary);
    if (!cust) {
        printf("Error opening file.");
        return;
    }
    // Label
    printf("\nFile Contents: \n");
    // Loop through each record and print field labels with data
    cust.read(reinterpret_cast<char*>(&holder), sizeof(holder));
    while (!cust.eof()) {
        cout << "Index: " << holder.index << endl;
        cout << "Name: " << holder.name << endl;
        cout << "Address: " << holder.address << endl;
        cout << "Telephone: " << holder.telephone << endl;
        cout << "City: " << holder.city << endl;
        cout << "State: " << holder.state << endl;
        cout << "Balance: $" << fixed << setprecision(2) << holder.balance << endl;
        cout << "Last Payment Date: " << holder.last_payment_date << endl << endl;
        cust.read(reinterpret_cast<char*>(&holder), sizeof(holder));
    }
    printf("Reached end of file.\n");
    cust.close();
}

void printRecord(account record) {
    // Prints a record to the screen

    cout << "Index: " << record.index << endl;
    cout << "Name: " << record.name << endl;
    cout << "Address: " << record.address << endl;
    cout << "Telephone: " << record.telephone << endl;
    cout << "City: " << record.city << endl;
    cout << "State: " << record.state << endl;
    cout << "Balance: $" << fixed << setprecision(2) << record.balance << endl;
    cout << "Last Payment Date: " << record.last_payment_date << endl;
}

void deleteRecord() {
    // Removes a record from the file

    account toDelete;
    account blank;
    toDelete = searchFile();
    blank.index = toDelete.index;
    fstream custFile(FILENAME, ios::binary | ios::in | ios::out);
    if (custFile) {
        custFile.seekp(byteNum(toDelete.index), custFile.beg);
        custFile.write(reinterpret_cast<char*>(&blank), sizeof(blank));
        custFile.close();
    }
}