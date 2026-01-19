#include <iostream>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <string>
#include <algorithm>

#include "../include/logistics.h"
#include "../include/auth.h"

using namespace std;

const int ADMIN_ROLE = 2;

void displayBanner()
{
    system("cls || clear");
    cout << "\n  ███████╗██╗    ██╗██╗███████╗████████╗███████╗██╗  ██╗\n";
    cout << "  ██╔════╝██║    ██║██║██╔════╝╚══██╔══╝██╔════╝╚██╗██╔╝\n";
    cout << "  ███████╗██║ █╗ ██║██║█████╗     ██║   █████╗   ╚███╔╝ \n";
    cout << "  ╚════██║██║███╗██║██║██╔══╝     ██║   ██╔══╝   ██╔██╗ \n";
    cout << "  ███████║╚███╔███╔╝██║██║        ██║   ███████╗██╔╝ ██╗\n";
    cout << "  ╚══════╝ ╚══╝╚══╝ ╚═╝╚═╝        ╚═╝   ╚══════╝╚═╝  ╚═╝\n\n";
    cout << "  ═══════════════════════════════════════════════════════════\n";
    cout << "   Courier System v2.4 (Admin Secured)\n";
    cout << "   Real-Time Parcel Management & Routing System\n";
    cout << "  ═══════════════════════════════════════════════════════════\n\n";
}

void displayMenu(int currentRole)
{
    cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
    cout << "  │  OPERATIONS MENU (Role: Admin)                              │\n";
    cout << "  ├─────────────────────────────────────────────────────────────┤\n";

    cout << "  │  [1] Register New Parcel                                    │\n";
    cout << "  │  [2] Manage Parcel Status                                   │\n";
    cout << "  │  [3] List All Parcels (Sorted)                              │\n";
    cout << "  │  [4] List All Trucks & Information                          │\n";
    cout << "  │  [5] Track Parcel Status                                    │\n";
    cout << "  │  [6] Process Sorting Queue (Priority-Based)                 │\n";
    cout << "  │  [7] Dispatch Parcels for Delivery                          │\n";
    cout << "  │  [8] Execute Delivery Attempt                               │\n";
    cout << "  │  [9] View Routing Network                                   │\n";
    cout << "  │  [10] System Dashboard                                      │\n";
    cout << "  │  [11] Operation Logs (Audit Trail)                          │\n";
    cout << "  │  [12] Add New Truck to Fleet                                │\n";
    cout << "  │  [13] Exit System                                           │\n";
    cout << "  │  [14] Admin Settings                                        │\n";

    cout << "  └─────────────────────────────────────────────────────────────┘\n\n";
    cout << "  Select operation: ";
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    srand(time(0));

    /* ================= AUTHENTICATION (ONLY CHANGE AREA) ================= */

    AdminAuth auth;
    bool authenticated = false;

    displayBanner();

    while (!authenticated)
    {
        authenticated = auth.login();
        if (!authenticated)
        {
            cout << "\n  Press Enter to retry...";
            cin.ignore();
            cin.get();
            displayBanner();
        }
    }

    int currentRole = ADMIN_ROLE;

    /* ================= END AUTHENTICATION AREA ================= */

    SwiftExLogistics system;

    cout << "\n  Press Enter to continue...";
    cin.ignore();
    cin.get();

    int userChoice;
    bool running = true;

    while (running)
    {
        displayBanner();
        displayMenu(currentRole);

        if (!(cin >> userChoice))
        {
            cout << "\n  [ERROR] Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n  Press Enter to continue...";
            cin.get();
            continue;
        }

        switch (userChoice)
        {
        case 1:
            system.registerParcel();
            break;
        case 2:
            system.manageParcelStatus();
            break;
        case 3:
            system.listAllParcels();
            break;
        case 4:
            system.listAllTrucks();
            break;
        case 5:
            system.trackParcel();
            break;
        case 6:
            system.processSortingQueue();
            break;
        case 7:
            system.dispatchForDelivery();
            break;
        case 8:
            system.attemptDelivery();
            break;
        case 9:
            system.viewRoutingNetwork();
            break;
        case 10:
            system.systemDashboard();
            break;
        case 11:
            system.viewOperationLog();
            break;

        case 12:
            system.addNewTruck();
            break;

        case 13:
            cout << "\n  [SYSTEM] Shutting down logistics engine...\n";
            cout << "  [SYSTEM] ✓ Shutdown complete. Goodbye!\n\n";
            running = false;
            break;

        case 14:
        {

            int subChoice;
            cout << "\n  ┌──────────────── ADMIN SETTINGS ────────────────┐\n";
            cout << "  │  [1] Logout                                    │\n";
            cout << "  │  [2] Change Password                           │\n";
            cout << "  │  [3] Change CNIC                               │\n";
            cout << "  └───────────────────────────────────────────────┘\n";
            cout << "  Select: ";
            cin >> subChoice;

            if (subChoice == 1)
            {
                authenticated = false;
                while (!authenticated)
                {
                    authenticated = auth.login();
                    if (!authenticated)
                    {
                        cout << "\n  Press Enter to retry...";
                        cin.ignore();
                        cin.get();
                        displayBanner();
                    }
                }
            }
            else if (subChoice == 2)
            {
                auth.forgotPassword();
            }
            else if (subChoice == 3)
            {
                auth.changeCNIC(); // ← NEW FEATURE
            }
            else
            {
                cout << "\n  [ERROR] Invalid option.\n";
            }
            break;
        }

        default:
            cout << "\n  [ERROR] Invalid operation code.\n";
        }
    }

    return 0;
}
