#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib> // For system("cls")
#include <iomanip> // For std::setw (Makes text alignment perfect)

using namespace std;

// --- FILE CONSTANTS ---
const string FILE_ADMIN = "account.txt";
const string FILE_PATIENTS = "patients.txt";
const string FILE_DOCTORS = "doctors.txt";
const string FILE_APPOINTMENTS = "appointments.txt";

// --- DATA STRUCTURES ---
struct Doctor {
    string name;
    string specialization;
    int roomNumber;
    int startHour;
    int endHour;
};

struct Patient {
    string name;
    int age;
    string gender;
    string phone;
    string reason;
};

// --- VISUAL HELPER FUNCTIONS ---

// Draws a specific line for headers
void drawLine(int width) {
    cout << "  "; // Margin
    for (int i = 0; i < width; i++) cout << "=";
    cout << endl;
}

// Clears screen and creates a clean pause prompt
void pause() {
    cout << "\n  >> Press Enter to continue...";
    cin.ignore(1000, '\n'); 
    cin.get();              
    system("cls");          
}

// Ensures user enters a valid number (Input Validation)
int getValidInt() {
    int x;
    while (!(cin >> x)) {
        cin.clear(); 
        cin.ignore(1000, '\n'); 
        cout << "    [!] Invalid input. Please enter a number: ";
    }
    return x;
}

// Check if a string contains only digits
bool isDigits(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// Splits string data from files
vector<string> split(const string& line, char delimiter) {
    vector<string> items;
    stringstream ss(line);
    string item;
    while (getline(ss, item, delimiter)) {
        items.push_back(item);
    }
    return items;
}

// --- CORE FUNCTIONS ---

void signup() {
    string id, password;
    system("cls");
    cout << "\n";
    drawLine(50);
    cout << "  |             REGISTER NEW ADMIN             |\n";
    drawLine(50);

    cout << "\n    Enter New Admin ID: ";
    cin >> id;

    // Check existing
    ifstream inFile(FILE_ADMIN);
    string fId, fPass;
    bool exists = false;
    if (inFile.is_open()) {
        while (inFile >> fId >> fPass) {
            if (fId == id) exists = true;
        }
        inFile.close();
    }

    if (exists) {
        cout << "\n    [Error] This ID is already registered.\n";
        pause();
        return;
    }

    cout << "    Enter Password: ";
    cin >> password;

    ofstream outFile(FILE_ADMIN, ios::app);
    outFile << id << " " << password << endl;
    outFile.close();

    cout << "\n    [Success] New admin registered successfully.\n";
    pause();
}

void addDoctor() {
    Doctor doc;
    system("cls");
    cout << "\n";
    drawLine(50);
    cout << "  |               ADD NEW DOCTOR               |\n";
    drawLine(50);

    cin.ignore(1000, '\n'); 
    cout << "\n    Doctor Name: ";
    getline(cin, doc.name);

    cout << "    Specialization (e.g., Heart, Bone): "; 
    getline(cin, doc.specialization);

    cout << "    Room Number: ";
    doc.roomNumber = getValidInt();
    while (doc.roomNumber <= 0) {
        cout << "    [!] Room must be a positive number: ";
        doc.roomNumber = getValidInt();
    }

    cout << "    Shift Start Hour (0-23): ";
    doc.startHour = getValidInt();
    while (doc.startHour < 0 || doc.startHour > 23) {
        cout << "    [!] Invalid Hour (0-23): ";
        doc.startHour = getValidInt();
    }

    cout << "    Shift End Hour (0-23): ";
    doc.endHour = getValidInt();
    while (doc.endHour < 0 || doc.endHour > 23) {
        cout << "    [!] Invalid Hour (0-23): ";
        doc.endHour = getValidInt();
    }

    if (doc.startHour >= doc.endHour) {
        cout << "\n    [Error] Start time must be before End time.\n";
        pause();
        return;
    }

    ofstream outFile(FILE_DOCTORS, ios::app);
    outFile << doc.name << "|" << doc.specialization << "|" << doc.roomNumber << "|" 
            << doc.startHour << "|" << doc.endHour << endl;
    outFile.close();

    cout << "\n    [Success] Doctor added to database.\n";
    pause();
}

// Logic: Check if appointment time is taken
bool isTimeSlotTaken(string doctorName, int time) {
    ifstream inFile(FILE_APPOINTMENTS);
    string line;
    while (getline(inFile, line)) {
        vector<string> data = split(line, '|');
        if (data.size() >= 4) {
            if (data[1] == doctorName && stoi(data[3]) == time) {
                return true; 
            }
        }
    }
    return false;
}

void addPatient() {
    ifstream docCheck(FILE_DOCTORS);
    if (docCheck.peek() == ifstream::traits_type::eof()) {
        cout << "\n    [System] No doctors found. Add a doctor first.\n";
        pause();
        return;
    }
    docCheck.close();

    Patient pat;
    int desiredTime;

    system("cls");
    cout << "\n";
    drawLine(50);
    cout << "  |          PATIENT REGISTRATION              |\n";
    drawLine(50);

    cin.ignore(1000, '\n');
    cout << "\n    Name: "; getline(cin, pat.name);
    
    // Age Validation
    cout << "    Age: "; 
    pat.age = getValidInt();
    while (pat.age <= 0 || pat.age > 120) {
        cout << "    [!] Invalid Age. Please enter realistic age: ";
        pat.age = getValidInt();
    }
    cin.ignore(1000, '\n');

    // Gender Validation
    while (true) {
        cout << "    Gender (M/F): "; 
        getline(cin, pat.gender);
        if (pat.gender == "M" || pat.gender == "F" || pat.gender == "m" || pat.gender == "f") {
            // Standardize to uppercase
            if (pat.gender == "m") pat.gender = "M";
            if (pat.gender == "f") pat.gender = "F";
            break;
        }
        cout << "    [!] Please enter M or F only.\n";
    }

    // Phone Validation
    while (true) {
        cout << "    Phone: "; 
        getline(cin, pat.phone);
        if (isDigits(pat.phone) && pat.phone.length() > 5) break;
        cout << "    [!] Invalid phone (digits only). Try again.\n";
    }

    cout << "    Reason: "; getline(cin, pat.reason);

    // Show Doctors Table
    system("cls");
    drawLine(75);
    cout << "  |                       AVAILABLE DOCTORS LIST                      |\n";
    drawLine(75);
    cout << "   " << left << setw(5) << "No" << setw(20) << "Name" << setw(15) << "Specialist" << "Availability" << endl;
    cout << "   -----------------------------------------------------------------------" << endl;
    
    ifstream docFile(FILE_DOCTORS);
    string line;
    vector<Doctor> doctors; 
    int index = 1;

    while (getline(docFile, line)) {
        vector<string> data = split(line, '|');
        if (data.size() >= 5) {
            Doctor d;
            d.name = data[0]; d.specialization = data[1];
            d.roomNumber = stoi(data[2]); d.startHour = stoi(data[3]); d.endHour = stoi(data[4]);
            doctors.push_back(d);

            cout << "   " << left << setw(5) << index 
                 << setw(20) << ("Dr. " + d.name)
                 << setw(15) << d.specialization
                 << d.startHour << ":00 - " << d.endHour << ":00 (Room " << d.roomNumber << ")" << endl;
            index++;
        }
    }
    docFile.close();

    cout << "\n    Select Doctor (Enter No): ";
    int choice = getValidInt();

    if (choice < 1 || choice > doctors.size()) {
        cout << "\n    [Error] Invalid selection.\n";
        pause();
        return;
    }

    Doctor selectedDoc = doctors[choice - 1];

    cout << "    Enter Time Slot (" << selectedDoc.startHour << " - " << selectedDoc.endHour << "): ";
    desiredTime = getValidInt();

    // Constraints Check
    if (desiredTime < selectedDoc.startHour || desiredTime >= selectedDoc.endHour) {
        cout << "\n    [Error] Doctor is not available at this hour.\n";
        pause();
        return;
    }

    if (isTimeSlotTaken(selectedDoc.name, desiredTime)) {
        cout << "\n    [Busy] Dr. " << selectedDoc.name << " already has a patient at " << desiredTime << ":00.\n";
        cout << "    Please choose a different time.\n";
        pause();
        return;
    }

    // Save Data
    ofstream appFile(FILE_APPOINTMENTS, ios::app);
    appFile << pat.name << "|" << selectedDoc.name << "|" << pat.reason << "|" << desiredTime << endl;
    appFile.close();

    ofstream patFile(FILE_PATIENTS, ios::app);
    patFile << pat.name << "|" << pat.age << "|" << pat.gender << "|" << pat.phone << "|" << pat.reason << endl; 
    patFile.close();

    cout << "\n    [Success] Appointment confirmed for " << desiredTime << ":00 hours.\n";
    pause();
}

void showAppointments() {
    system("cls");
    cout << "\n";
    drawLine(65);
    cout << "  |                  APPOINTMENT SCHEDULE                 |\n";
    drawLine(65);

    ifstream inFile(FILE_APPOINTMENTS);
    string line;
    bool found = false;

    cout << "   " << left << setw(20) << "Patient" 
         << left << setw(20) << "Doctor" 
         << left << setw(15) << "Time" << endl;
    cout << "   -------------------------------------------------------" << endl;

    while (getline(inFile, line)) {
        vector<string> data = split(line, '|');
        if (data.size() >= 4) {
            cout << "   " << left << setw(20) << data[0] 
                 << left << setw(20) << ("Dr. " + data[1]) 
                 << data[3] << ":00" << endl;
            found = true;
        }
    }
    
    if (!found) cout << "\n    [Info] No appointments scheduled yet.\n";
    inFile.close();
    pause();
}

void mainMenu() {
    int choice;
    while (true) {
        system("cls");
        cout << "\n";
        cout << "   ________________________________________________________  " << endl;
        cout << "  |                                                        | " << endl;
        cout << "  |              HOSPITAL MANAGEMENT SYSTEM                | " << endl;
        cout << "  |________________________________________________________| " << endl;
        cout << "  |                                                        | " << endl;
        cout << "  |   [1] Add New Doctor                                   | " << endl;
        cout << "  |   [2] Add Patient & Book Appointment                   | " << endl;
        cout << "  |   [3] View All Appointments                            | " << endl;
        cout << "  |   [4] Logout                                           | " << endl;
        cout << "  |________________________________________________________| " << endl;
        cout << "\n    Enter Choice: ";
        
        choice = getValidInt();

        switch (choice) {
            case 1: addDoctor(); break;
            case 2: addPatient(); break;
            case 3: showAppointments(); break;
            case 4: return; 
            default: 
                cout << "\n    [!] Invalid Choice.\n"; 
                pause();
        }
    }
}

void login() {
    string id, password, fId, fPass;
    system("cls");
    cout << "\n\n";
    cout << "           ========================================\n";
    cout << "                       SECURE LOGIN                \n";
    cout << "           ========================================\n\n";
    
    cout << "             User ID  : "; cin >> id;
    cout << "             Password : "; cin >> password;

    ifstream inFile(FILE_ADMIN);
    bool found = false;
    
    if (inFile.is_open()) {
        while (inFile >> fId >> fPass) {
            if (fId == id && fPass == password) {
                found = true;
                break;
            }
        }
        inFile.close();
    }

    if (found) {
        mainMenu();
    } else {
        cout << "\n             [!] Invalid Credentials.\n";
        pause();
    }
}

int main() {
    int choice;
    while (true) {
        system("cls");
        cout << "\n\n";
        cout << "       ************************************************\n";
        cout << "       * WELCOME TO HOSPITAL SYSTEM            *\n";
        cout << "       ************************************************\n\n";
        cout << "          1. Login (Admin)\n";
        cout << "          2. Register (New Admin)\n";
        cout << "          3. Exit\n\n";
        cout << "          Select Option: ";
        
        choice = getValidInt();

        switch (choice) {
            case 1: login(); break;
            case 2: signup(); break;
            case 3: 
                cout << "\n          Exiting System... Goodbye!\n"; 
                return 0;
            default: 
                cout << "\n          [!] Invalid Selection.\n"; 
                pause();
        }
    }
}