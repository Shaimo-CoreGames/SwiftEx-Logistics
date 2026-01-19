#ifndef AUTH_H
#define AUTH_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

class AdminAuth {
private:
    char username[50];
    char password[50];   // encrypted
    char cnic[20];       // encrypted

    /* ===== Simple Encryption (Caesar +3) ===== */
    void encrypt(char* s) {
        for (int i = 0; s[i] != '\0'; i++)
            s[i] = s[i] + 3;
    }

    void decrypt(char* s) {
        for (int i = 0; s[i] != '\0'; i++)
            s[i] = s[i] - 3;
    }

    void ensureAdminFile() {
        system("if not exist data mkdir data");

        ifstream in("data/admin_db.txt");
        if (in.is_open()) {
            in.getline(username, 50, '|');
            in.getline(password, 50, '|');
            in.getline(cnic, 20);
            in.close();
            return;
        }

        // Default admin (first run)
        strcpy(username, "admin");
        strcpy(password, "admin123");
        strcpy(cnic, "3520212345671");

        encrypt(password);
        encrypt(cnic);

        ofstream out("data/admin_db.txt");
        out << username << "|" << password << "|" << cnic;
        out.close();
    }

    void saveAdmin() {
        ofstream out("data/admin_db.txt", ios::trunc);
        out << username << "|" << password << "|" << cnic;
        out.close();
    }

public:
    AdminAuth() {
        ensureAdminFile();
    }
bool login() {
    char inputUser[50];
    char inputPass[50];

    cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
    cout << "  │  ADMIN AUTHENTICATION                                       │\n";
    cout << "  ├─────────────────────────────────────────────────────────────┤\n";
    cout << "  │  Secure access required to continue                         │\n";
    cout << "  │                                                             │\n";
    cout << "  │  Enter administrator credentials below                      │\n";
    cout << "  └─────────────────────────────────────────────────────────────┘\n\n";

    cout << "  Username: ";
    cin >> inputUser;

    if (strcmp(inputUser, username) != 0) {
        cout << "  [ERROR] Wrong username\n";
        return false;
    }

    cout << "  Password: ";
    cin >> inputPass;

    encrypt(inputPass);

    if (strcmp(inputPass, password) == 0) {
        cout << "  ✓ Login successful\n";
        return true;
    }

    cout << "  [ERROR] Wrong password\n";
    char choice;
    cout << "  Forgot password? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y')
        return forgotPassword();

    return false;
}
bool forgotPassword() {
    char inputCnic[20];

    cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
    cout << "  │  PASSWORD RECOVERY                                          │\n";
    cout << "  ├─────────────────────────────────────────────────────────────┤\n";
    cout << "  │  Identity verification required                             │\n";
    cout << "  │                                                             │\n";
    cout << "  │  Please provide your registered CNIC                        │\n";
    cout << "  └─────────────────────────────────────────────────────────────┘\n\n";

    cout << "  Enter CNIC: ";
    cin >> inputCnic;

    encrypt(inputCnic);

    if (strcmp(inputCnic, cnic) != 0) {
        cout << "  [ERROR] CNIC verification failed\n";
        return false;
    }

    char newPass[50];
    cout << "  Enter new password: ";
    cin >> newPass;

    encrypt(newPass);
    strcpy(password, newPass);

    saveAdmin();

    cout << "  ✓ Password changed successfully\n";
    return true;
}
  
    
    void changeCNIC() {
    char oldCnic[20], newCnic[20];

    cout << "Enter current CNIC: ";
    cin >> oldCnic;
    encrypt(oldCnic);

    if (strcmp(oldCnic, cnic) != 0) {
        cout << "[ERROR] CNIC verification failed\n";
        return;
    }

    cout << "Enter new CNIC: ";
    cin >> newCnic;
    encrypt(newCnic);
    strcpy(cnic, newCnic);

    saveAdmin();
    cout << "CNIC updated successfully\n";
}
    
};

#endif // AUTH_H
