#ifndef LOGISTICS_ENGINE_H
#define LOGISTICS_ENGINE_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#include "validator.h"
#include "parcel.h"
#include "truck.h"
#include "hashmap.h"
#include "datastructures.h"
#include "routing.h"

using namespace std;

class SwiftExLogistics
{
private:
    HashMap<const char *, Parcel *> parcelRegistry;
    Truck trucks[20];
    int truckCount;
    PriorityQueue sortingQueue;
    Queue<Parcel *> transitQueue;
    Queue<Parcel *> deliveryQueue;
    Queue<Parcel *> waitingQueue;
    OperationStack operationLog;
    RoutingGraph routingNetwork;

    /* ===================== ADDED (SAFE) ===================== */
    void seedTrucksIfEmpty()
    {
        system("if not exist data mkdir data");
        ifstream f("data/trucks_db.txt");
        if (f.is_open())
        {
            f.close();
            return;
        }

        ofstream out("data/trucks_db.txt");
        out << "1|Ali Khan|5000|40|Lahore|Excellent\n";
        out << "2|Ahmed Raza|4000|35|Karachi|Good\n";
        out << "3|Usman Tariq|4500|38|Islamabad|Excellent\n";
        out.close();
    }

    /* ======================================================== */

    void logOperation(const char *type, const char *parcelID, const char *details)
    {
        Operation op;
        op.timestamp = DateTime();
        strcpy(op.type, type);
        strcpy(op.parcelID, parcelID);
        strcpy(op.details, details);
        operationLog.push(op);
    }

    void updateDatabase()
    {
        // Rewrite the entire file to ensure status updates are reflected
        ofstream file("parcels_db.txt", ios::trunc);
        if (!file.is_open())
        {
            file.open("data/parcels_db.txt", ios::trunc);
        }

        if (!file.is_open())
        {
            cout << "\n  [ERROR] Could not update database file!\n";
            return;
        }

        Parcel *parcels[1000];
        int count = 0;
        parcelRegistry.getAllValues(parcels, count, 1000);

        for (int i = 0; i < count; i++)
        {
            file << parcels[i]->trackingID << "|" << parcels[i]->sender << "|" << parcels[i]->recipient << "|"
                 << parcels[i]->origin << "|" << parcels[i]->destination << "|" << parcels[i]->destinationAddress << "|"
                 << parcels[i]->priority << "|" << parcels[i]->weight << "|" << (parcels[i]->isFragile ? "1" : "0") << "|"
                 << parcels[i]->currentStatus << "|" << parcels[i]->parcelStatus << "|" << parcels[i]->deliveryAttempts << "|"
                 << parcels[i]->truckID << "\n";
        }
        file.close();
    }

    // Legacy support wrapper
    void saveToFile(Parcel *p)
    {
        updateDatabase();
    }

    void loadFromFile()
    {
        // Try root file first
        ifstream file("parcels_db.txt");
        if (!file.is_open())
        {
            // Fallback to data directory
            file.open("data/parcels_db.txt");
        }

        if (!file.is_open())
            return;

        string line;
        while (getline(file, line))
        {
            vector<string> fields;
            string field;
            for (char c : line)
            {
                if (c == '|')
                {
                    fields.push_back(field);
                    field.clear();
                }
                else
                {
                    field += c;
                }
            }
            fields.push_back(field);

            if (fields.size() < 11)
                continue;

            Parcel *p = new Parcel();
            strcpy(p->trackingID, fields[0].c_str());
            strcpy(p->sender, fields[1].c_str());
            strcpy(p->recipient, fields[2].c_str());
            strcpy(p->origin, fields[3].c_str());
            strcpy(p->destination, fields[4].c_str());
            strcpy(p->destinationAddress, fields[5].c_str());

            stringstream ss_priority(fields[6]);
            stringstream ss_weight(fields[7]);

            ss_priority >> p->priority;
            ss_weight >> p->weight;
            p->isFragile = (fields[8] == "1");
            strcpy(p->currentStatus, fields[9].c_str());
            strcpy(p->parcelStatus, fields[10].c_str());

            if (fields.size() > 11)
            {
                stringstream ss_attempts(fields[11]);
                ss_attempts >> p->deliveryAttempts;
            }
            else
            {
                p->deliveryAttempts = 0;
            }

            if (fields.size() > 12)
            {
                stringstream ss_truck(fields[12]);
                ss_truck >> p->truckID;
            }
            else
            {
                p->truckID = -1;
            }

            parcelRegistry.insert(p->trackingID, p);
            sortingQueue.insert(p);

            // Reconstruct logic state & History
            p->history.addEvent("Registered", p->origin);
            if (strcmp(p->currentStatus, "Registered") != 0)
            {
                p->history.addEvent(p->currentStatus, "System Update");
            }

            if (p->truckID != -1)
            {
                // Should add to delivery queue logic if needed, but for now registry is key
            }
        }
        file.close();
    }

    void initializeTrucks()
    {
        ifstream truckFile("data/trucks_db.txt");
        if (!truckFile.is_open())
            return;

        string line;
        truckCount = 0;
        while (getline(truckFile, line) && truckCount < 20)
        {
            vector<string> fields;
            string field;
            for (char c : line)
            {
                if (c == '|')
                {
                    fields.push_back(field);
                    field.clear();
                }
                else
                {
                    field += c;
                }
            }
            fields.push_back(field);

            if (fields.size() < 6)
                continue;

            stringstream ss_id(fields[0]);
            stringstream ss_cap(fields[2]);
            stringstream ss_maxp(fields[3]);
            int id, cap, maxP;
            ss_id >> id;
            ss_cap >> cap;
            ss_maxp >> maxP;

            trucks[truckCount] = Truck(id, fields[1].c_str(), cap, maxP);
            strcpy(trucks[truckCount].location, fields[4].c_str());
            strcpy(trucks[truckCount].truckHealth, fields[5].c_str());
            truckCount++;
        }
        truckFile.close();
    }

public:
    SwiftExLogistics() : truckCount(0)
    {
        loadFromFile();
        seedTrucksIfEmpty();
        initializeTrucks();

        /* ==================== PAKISTAN ROUTING NETWORK ==================== */

        // --- Add Cities ---
        const char *cities[] = {
            "Karachi", "Hyderabad", "Thatta", "Mirpurkhas", "Nawabshah", "Sukkur", "Larkana", "Jacobabad",
            "Multan", "Bahawalpur", "Bahawalnagar", "Vehari", "Khanewal", "Sahiwal", "Okara",
            "Lahore", "Kasur", "Sheikhupura", "Faisalabad", "Jhang", "Gujranwala", "Gujrat", "Sialkot",
            "Islamabad", "Rawalpindi", "Abbottabad", "Mansehra", "Muzaffarabad",
            "Peshawar", "Mardan", "Swabi", "Kohat", "Bannu", "Dera Ismail Khan",
            "Quetta", "Khuzdar", "Loralai", "Zhob", "Turbat", "Gwadar",
            "Gilgit", "Skardu", "Mirpur"};

        for (const char *c : cities)
            routingNetwork.addCity(c);

        // Helper for bidirectional roads
        auto connect = [&](const char *a, const char *b, int d)
        {
            routingNetwork.addRoute(a, b, d);
            routingNetwork.addRoute(b, a, d);
        };

        // --- Sindh ---
        connect("Karachi", "Hyderabad", 160);
        connect("Karachi", "Thatta", 100);
        connect("Hyderabad", "Mirpurkhas", 65);
        connect("Hyderabad", "Nawabshah", 140);
        connect("Nawabshah", "Sukkur", 270);
        connect("Sukkur", "Larkana", 85);
        connect("Larkana", "Jacobabad", 95);

        // --- Punjab (South & Central) ---
        connect("Sukkur", "Multan", 400);
        connect("Multan", "Khanewal", 100);
        connect("Multan", "Bahawalpur", 90);
        connect("Bahawalpur", "Bahawalnagar", 110);
        connect("Bahawalnagar", "Vehari", 120);
        connect("Vehari", "Sahiwal", 95);
        connect("Sahiwal", "Okara", 40);
        connect("Okara", "Lahore", 120);

        // --- Punjab (Central & North) ---
        connect("Lahore", "Kasur", 55);
        connect("Lahore", "Sheikhupura", 40);
        connect("Sheikhupura", "Faisalabad", 85);
        connect("Faisalabad", "Jhang", 85);
        connect("Faisalabad", "Gujranwala", 90);
        connect("Gujranwala", "Gujrat", 45);
        connect("Gujrat", "Sialkot", 40);

        // --- Capital & KPK ---
        connect("Lahore", "Islamabad", 380);
        connect("Islamabad", "Rawalpindi", 15);
        connect("Islamabad", "Abbottabad", 120);
        connect("Abbottabad", "Mansehra", 25);
        connect("Mansehra", "Muzaffarabad", 140);
        connect("Islamabad", "Peshawar", 180);
        connect("Peshawar", "Mardan", 70);
        connect("Mardan", "Swabi", 35);
        connect("Peshawar", "Kohat", 60);
        connect("Kohat", "Bannu", 115);
        connect("Bannu", "Dera Ismail Khan", 140);

        // --- Balochistan ---
        connect("Dera Ismail Khan", "Zhob", 200);
        connect("Zhob", "Loralai", 120);
        connect("Loralai", "Quetta", 160);
        connect("Quetta", "Khuzdar", 300);
        connect("Khuzdar", "Turbat", 450);
        connect("Turbat", "Gwadar", 190);
        connect("Quetta", "Gwadar", 690);

        // --- Gilgit Baltistan / AJK ---
        connect("Islamabad", "Gilgit", 520);
        connect("Gilgit", "Skardu", 230);
        connect("Islamabad", "Mirpur", 130);
    }

    void registerParcel()
    {
        system("cls || clear");
        Parcel *p = new Parcel();
        p->generateTrackingID();

        cout << "\n╔═════════════════════════════════════════════════════════════╗\n";
        cout << "║          PARCEL REGISTRATION SYSTEM                         ║\n";
        cout << "╠═════════════════════════════════════════════════════════════╣\n";
        cout << "║  Generated Tracking ID: " << p->trackingID;
        for (int i = strlen(p->trackingID); i < 36; i++)
            cout << " ";
        cout << "║\n";
        cout << "╚═════════════════════════════════════════════════════════════╝\n\n";

        bool validInput = false;
        do
        {
            cout << "  Sender Name (letters only): ";
            cin.ignore();
            cin.getline(p->sender, 50);
            if (Validator::isValidName(p->sender))
                validInput = true;
            else
                cout << "  [ERROR] Invalid name! Use letters and spaces only (max 49 chars)\n";
        } while (!validInput);

        validInput = false;
        do
        {
            cout << "  Recipient Name (letters only): ";
            cin.getline(p->recipient, 50);
            if (Validator::isValidName(p->recipient))
                validInput = true;
            else
                cout << "  [ERROR] Invalid name! Use letters and spaces only (max 49 chars)\n";
        } while (!validInput);

        validInput = false;
        do
        {
            cout << "\n  Registered Cities:\n  ";
            routingNetwork.displayCityList();
            cout << "\n";

            cout << "  Origin City (letters only): ";
            cin.getline(p->origin, 30);
            if (Validator::isValidCity(p->origin) && routingNetwork.cityExists(p->origin))
                validInput = true;
            else
                cout << "  [ERROR] City not registered in routing network!\n";

        } while (!validInput);

        validInput = false;
        do
        {
            cout << "  Destination City (letters only): ";
            cin.getline(p->destination, 30);
            if (Validator::isValidCity(p->destination) && routingNetwork.cityExists(p->destination))
                validInput = true;
            else
                cout << "  [ERROR] City not registered in routing network!\n";

        } while (!validInput);

        validInput = false;
        do
        {
            cout << "  Destination Address: ";
            cin.getline(p->destinationAddress, 100);
            if (Validator::isValidAddress(p->destinationAddress))
                validInput = true;
            else
                cout << "  [ERROR] Address too long (max 99 chars)\n";
        } while (!validInput);

        validInput = false;
        do
        {
            cout << "  Priority (0=Overnight, 1=2-Day, 2=Standard): ";
            if (!(cin >> p->priority))
            {
                cout << "  [ERROR] Please enter a number!\n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            if (Validator::isValidPriority(p->priority))
                validInput = true;
            else
                cout << "  [ERROR] Priority must be 0-2\n";
        } while (!validInput);

        validInput = false;
        do
        {
            cout << "  Weight (kg, 0< w <=500): ";
            if (!(cin >> p->weight))
            {
                cout << "  [ERROR] Please enter a valid number!\n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            if (Validator::isValidWeight(p->weight))
                validInput = true;
            else
                cout << "  [ERROR] Weight must be >0 and <=500 kg\n";
        } while (!validInput);

        validInput = false;
        do
        {
            cout << "  Is Fragile? (1=Yes, 0=No): ";
            int fragileInput;
            if (!(cin >> fragileInput))
            {
                cout << "  [ERROR] Please enter 0 or 1!\n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            if (fragileInput == 0 || fragileInput == 1)
            {
                p->isFragile = (fragileInput == 1);
                validInput = true;
            }
            else
            {
                cout << "  [ERROR] Enter 0 for No or 1 for Yes\n";
            }
        } while (!validInput);

        saveToFile(p);
        parcelRegistry.insert(p->trackingID, p);
        sortingQueue.insert(p);
        logOperation("REGISTER", p->trackingID, "New parcel registered");

        p->display();
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void manageParcelStatus()
    {
        system("cls || clear");
        char trackingID[20];
        cout << "\n  Enter Tracking ID: ";
        cin >> trackingID;

        Parcel **result = parcelRegistry.search(trackingID);
        if (result && *result)
        {
            (*result)->display();
            int choice;
            cout << "\n  [1] Accept | [2] Reject | [3] Waiting | [4] View Location | [5] Back\n";
            cout << "  Select: ";
            cin >> choice;
            switch (choice)
            {
            case 1:
                strcpy((*result)->parcelStatus, "Accepted");
                logOperation("ACCEPT", trackingID, "Accepted");
                updateDatabase();
                break;
            case 2:
                strcpy((*result)->parcelStatus, "Rejected");
                logOperation("REJECT", trackingID, "Rejected");
                updateDatabase();
                break;
            case 3:
                strcpy((*result)->parcelStatus, "Waiting");
                logOperation("WAITING", trackingID, "Waiting");
                updateDatabase();
                break;
            case 4:
                cout << "  Location: " << (*result)->currentLocation << "\n";
                (*result)->history.display();
                break;
            }
        }
        else
        {
            cout << "\n  [ERROR] Parcel not found!\n";
        }
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void listAllParcels()
    {
        system("cls || clear");
        cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
        cout << "  │  ALL PARCELS                                                │\n";
        cout << "  └─────────────────────────────────────────────────────────────┘\n\n";

        int count = 0;
        Parcel *parcels[1000];
        parcelRegistry.getAllValues(parcels, count, 1000);

        if (count == 0)
        {
            cout << "  [INFO] No parcels in system.\n";
        }
        else
        {
            // Sort by priority first
            for (int i = 0; i < count - 1; i++)
            {
                for (int j = 0; j < count - i - 1; j++)
                {
                    if (parcels[j]->priority > parcels[j + 1]->priority)
                    {
                        Parcel *temp = parcels[j];
                        parcels[j] = parcels[j + 1];
                        parcels[j + 1] = temp;
                    }
                }
            }

            // ACTIVE TABLE
            cout << "  >>> ACTIVE SHIPMENTS (Pending/In-Transit)\n";
            cout << "  ┌──────────────┬────────────────┬────────────────┬──────────┬──────────────┐\n";
            cout << "  │ Tracking ID  │ Sender         │ Recipient      │ Priority │ Status       │\n";
            cout << "  ├──────────────┼────────────────┼────────────────┼──────────┼──────────────┤\n";

            bool activeFound = false;
            for (int i = 0; i < count; i++)
            {
                if (strcmp(parcels[i]->currentStatus, "Delivered") != 0 &&
                    strcmp(parcels[i]->parcelStatus, "Rejected") != 0 &&
                    strcmp(parcels[i]->currentStatus, "Rejected") != 0)
                {

                    activeFound = true;
                    cout << "  │ " << left << setw(12) << parcels[i]->trackingID
                         << " │ " << setw(14) << string(parcels[i]->sender).substr(0, 14)
                         << " │ " << setw(14) << string(parcels[i]->recipient).substr(0, 14)
                         << " │ " << setw(8) << (parcels[i]->priority == 0 ? "Overnight" : (parcels[i]->priority == 1 ? "2-Day" : "Standard"))
                         << " │ " << setw(12) << string(parcels[i]->currentStatus).substr(0, 12) << " │\n";
                }
            }
            if (!activeFound)
                cout << "  │ [INFO] No active shipments found.                                     │\n";
            cout << "  └──────────────┴────────────────┴────────────────┴──────────┴──────────────┘\n\n";

            // HISTORY TABLE
            cout << "  >>> DELIVERY HISTORY (Delivered/Rejected)\n";
            cout << "  ┌──────────────┬────────────────┬────────────────┬──────────┬──────────────┐\n";
            cout << "  │ Tracking ID  │ Sender         │ Destination    │ Priority │ Final Status │\n";
            cout << "  ├──────────────┼────────────────┼────────────────┼──────────┼──────────────┤\n";

            bool historyFound = false;
            for (int i = 0; i < count; i++)
            {
                if (strcmp(parcels[i]->currentStatus, "Delivered") == 0 ||
                    strcmp(parcels[i]->parcelStatus, "Rejected") == 0 ||
                    strcmp(parcels[i]->currentStatus, "Rejected") == 0)
                {

                    historyFound = true;
                    cout << "  │ " << left << setw(12) << parcels[i]->trackingID
                         << " │ " << setw(14) << string(parcels[i]->sender).substr(0, 14)
                         << " │ " << setw(14) << string(parcels[i]->destination).substr(0, 14)
                         << " │ " << setw(8) << (parcels[i]->priority == 0 ? "Overnight" : (parcels[i]->priority == 1 ? "2-Day" : "Standard"))
                         << " │ " << setw(12) << string(parcels[i]->currentStatus).substr(0, 12) << " │\n";
                }
            }
            if (!historyFound)
                cout << "  │ [INFO] No history records found.                                      │\n";
            cout << "  └──────────────┴────────────────┴────────────────┴──────────┴──────────────┘\n";
        }
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void listAllTrucks()
    {
        system("cls || clear");
        cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
        cout << "  │  FLEET STATUS                                               │\n";
        cout << "  └─────────────────────────────────────────────────────────────┘\n\n";

        if (truckCount == 0)
        {
            cout << "  [INFO] No trucks in fleet.\n";
        }
        else
        {
            for (int i = 0; i < truckCount; i++)
            {
                trucks[i].display();
            }
        }
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void trackParcel()
    {
        system("cls || clear");
        char trackingID[20];
        cout << "  Enter Tracking ID: ";
        cin >> trackingID;

        Parcel **result = parcelRegistry.search(trackingID);
        if (result && *result)
        {
            (*result)->display();
            (*result)->history.display();
        }
        else
        {
            cout << "\n  [ERROR] Parcel not found!\n";
        }
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void processSortingQueue()
    {
        system("cls || clear");
        cout << "  Processing priority queue...\n\n";
        int processed = 0;
        while (!sortingQueue.isEmpty() && processed < 10)
        {
            Parcel *p = sortingQueue.extractMin();
            if (p->priority == 0)
            {
                transitQueue.enqueue(p);
                strcpy(p->currentStatus, "In Transit (Overnight)");
            }
            else if (p->priority == 1)
            {
                waitingQueue.enqueue(p);
                strcpy(p->currentStatus, "Waiting (2-Day)");
            }
            else
            {
                deliveryQueue.enqueue(p);
                strcpy(p->currentStatus, "Scheduled (Standard)");
            }
            logOperation("SORT", p->trackingID, "Moved to queue");
            processed++;
        }
        cout << "  ✓ " << processed << " items processed.\n";
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void dispatchForDelivery()
    {
        system("cls || clear");
        cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
        cout << "  │  DISPATCH PARCELS (SELECT FOR DELIVERY)                     │\n";
        cout << "  └─────────────────────────────────────────────────────────────┘\n\n";

        if (truckCount == 0)
        {
            cout << "  [ERROR] No trucks available.\n";
            cout << "\n  Press Enter to continue...";
            cin.ignore();
            cin.get();
            return;
        }

        // List eligible parcels (Status: Accepted, Waiting, Registered)
        Parcel *parcels[1000];
        int count = 0;
        int eligibleCount = 0;
        parcelRegistry.getAllValues(parcels, count, 1000);

        cout << "  ┌──────────────┬────────────────┬──────────────┬──────────────┐\n";
        cout << "  │ Tracking ID  │ Origin         │ Destination  │ Status       │\n";
        cout << "  ├──────────────┼────────────────┼──────────────┼──────────────┤\n";

        for (int i = 0; i < count; i++)
        {
            // Check eligibility: Not Dispatched, Not Delivered
            if (strstr(parcels[i]->currentStatus, "Dispatched") == nullptr &&
                strstr(parcels[i]->currentStatus, "Delivered") == nullptr)
            {

                cout << "  │ " << left << setw(12) << parcels[i]->trackingID
                     << " │ " << setw(14) << parcels[i]->origin
                     << " │ " << setw(12) << parcels[i]->destination
                     << " │ " << setw(12) << parcels[i]->currentStatus << " │\n";
                eligibleCount++;
            }
        }
        cout << "  └──────────────┴────────────────┴──────────────┴──────────────┘\n";

        if (eligibleCount == 0)
        {
            cout << "\n  [INFO] No parcels ready for dispatch.\n";
            cout << "\n  Press Enter to continue...";
            cin.ignore();
            cin.get();
            return;
        }

        char trackingID[20];
        cout << "\n  Enter Tracking ID to Dispatch: ";
        cin >> trackingID;

        Parcel **result = parcelRegistry.search(trackingID);
        if (result && *result)
        {
            Parcel *p = *result;

            // Dispatch Logic
            cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
            cout << "  │  DISPATCH ROUTING ANALYSIS                                  │\n";
            cout << "  └─────────────────────────────────────────────────────────────┘\n";
            cout << "\n  Parcel: " << p->trackingID << "\n";
            cout << "  Route: " << p->origin << " → " << p->destination << "\n\n";

            cout << "  Computing optimal delivery route...\n";
            routingNetwork.findShortestPath(p->origin, p->destination, 2);

            bool assigned = false;
            for (int i = 0; i < truckCount && !assigned; i++)
            {
                if (trucks[i].canAddParcel(p->weight))
                {
                    trucks[i].addParcel(p->weight);
                    p->truckID = trucks[i].truckID;
                    strcpy(p->currentStatus, "Dispatched");
                    p->history.addEvent("Dispatched", p->origin);
                    logOperation("DISPATCH", p->trackingID, "Assigned to truck");
                    cout << "\n  ✓ Assigned to Truck #" << trucks[i].truckID << " (Driver: " << trucks[i].driverName << ")\n";
                    assigned = true;
                    updateDatabase();
                }
            }
            if (!assigned)
                cout << "  [ERROR] No suitable truck found or fleet full.\n";
        }
        else
        {
            cout << "\n  [ERROR] Parcel not found!\n";
        }

        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void attemptDelivery()
    {
        system("cls || clear");
        cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
        cout << "  │  EXECUTE DELIVERY (SELECT PARCEL)                           │\n";
        cout << "  └─────────────────────────────────────────────────────────────┘\n\n";

        // List dispatched parcels
        Parcel *parcels[1000];
        int count = 0;
        int eligibleCount = 0;
        parcelRegistry.getAllValues(parcels, count, 1000);

        cout << "  ┌──────────────┬────────────────┬──────────────┬──────────────┐\n";
        cout << "  │ Tracking ID  │ Origin         │ Destination  │ Status       │\n";
        cout << "  ├──────────────┼────────────────┼──────────────┼──────────────┤\n";

        for (int i = 0; i < count; i++)
        {
            // Check eligibility: Status contains Dispatch
            if (strstr(parcels[i]->currentStatus, "Dispatched") != nullptr)
            {
                cout << "  │ " << left << setw(12) << parcels[i]->trackingID
                     << " │ " << setw(14) << parcels[i]->origin
                     << " │ " << setw(12) << parcels[i]->destination
                     << " │ " << setw(12) << parcels[i]->currentStatus << " │\n";
                eligibleCount++;
            }
        }
        cout << "  └──────────────┴────────────────┴──────────────┴──────────────┘\n";

        if (eligibleCount == 0)
        {
            cout << "\n  [INFO] No parcels currently dispatched.\n";
            cout << "\n  Press Enter to continue...";
            cin.ignore();
            cin.get();
            return;
        }

        char trackingID[20];
        cout << "\n  Enter Tracking ID to Deliver: ";
        cin >> trackingID;

        Parcel **result = parcelRegistry.search(trackingID);
        if (result && *result)
        {
            (*result)->deliveryAttempts++;
            strcpy((*result)->currentStatus, "Delivered");
            (*result)->history.addEvent("Delivered", (*result)->destination);
            logOperation("DELIVER", trackingID, "Delivery attempted");
            cout << "\n  ✓ Delivery recorded successfully!\n";
            cout << "  Delivery Attempts: " << (*result)->deliveryAttempts << "\n";
            cout << "  Final Status: " << (*result)->currentStatus << "\n";
            updateDatabase();
        }
        else
        {
            cout << "\n  [ERROR] Parcel not found!\n";
        }
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void viewRoutingNetwork()
    {
        system("cls || clear");
        routingNetwork.displayNetwork();
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void systemDashboard()
    {
        system("cls || clear");
        cout << "\n  ╔═════════════════════════════════════════════════════════════╗\n";
        cout << "  ║  SYSTEM DASHBOARD - REAL-TIME STATISTICS                    ║\n";
        cout << "  ╚═════════════════════════════════════════════════════════════╝\n\n";

        int totalParcels = parcelRegistry.getCount();
        int sortingCount = sortingQueue.getSize();
        int transitCount = transitQueue.size();
        int deliveryCount = deliveryQueue.size();
        int waitingCount = waitingQueue.size();

        cout << "  ┌─────────────────────────────────────────────────────────────┐\n";
        cout << "  │  PARCEL STATISTICS                                          │\n";
        cout << "  ├─────────────────────────────────────────────────────────────┤\n";
        cout << "  │  Total Registered Parcels:  " << setw(30) << totalParcels << "  │\n";
        cout << "  │  In Sorting Queue:          " << setw(30) << sortingCount << "  │\n";
        cout << "  │  In Transit:                " << setw(30) << transitCount << "  │\n";
        cout << "  │  Awaiting Delivery:         " << setw(30) << deliveryCount << "  │\n";
        cout << "  │  In Waiting Queue:          " << setw(30) << waitingCount << "  │\n";
        cout << "  └─────────────────────────────────────────────────────────────┘\n\n";

        cout << "  ┌─────────────────────────────────────────────────────────────┐\n";
        cout << "  │  FLEET STATISTICS                                           │\n";
        cout << "  ├─────────────────────────────────────────────────────────────┤\n";
        cout << "  │  Active Trucks:             " << setw(30) << truckCount << "  │\n";

        double totalCapacity = 0;
        double totalLoad = 0;
        for (int i = 0; i < truckCount; i++)
        {
            totalCapacity += trucks[i].capacity;
            totalLoad += trucks[i].currentLoad;
        }

        double utilization = (totalCapacity > 0) ? (totalLoad / totalCapacity * 100) : 0;
        cout << "  │  Total Capacity (kg):       " << setw(30) << (int)totalCapacity << "  │\n";
        cout << "  │  Current Load (kg):         " << setw(30) << (int)totalLoad << "  │\n";
        cout << "  │  Fleet Utilization:         " << setw(28) << fixed << setprecision(1) << utilization << "%  │\n";
        cout << "  └─────────────────────────────────────────────────────────────┘\n\n";

        cout << "  ┌─────────────────────────────────────────────────────────────┐\n";
        cout << "  │  SYSTEM STATUS                                              │\n";
        cout << "  ├─────────────────────────────────────────────────────────────┤\n";
        cout << "  │  Status:                    " << setw(30) << "OPERATIONAL" << "  │\n";
        cout << "  │  Cities in Network:         " << setw(30) << routingNetwork.getCityCount() << "  │\n";
        cout << "  │  Routes Available:          " << setw(30) << routingNetwork.getRouteCount() << "  │\n";
        cout << "  └─────────────────────────────────────────────────────────────┘\n";

        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void viewOperationLog()
    {
        system("cls || clear");
        operationLog.displayHistory();
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }
    void addNewTruck()
    {
        system("cls || clear");

        if (truckCount >= 20)
        {
            cout << "\n  [ERROR] Fleet capacity reached. Cannot add more trucks.\n";
            cout << "\n  Press Enter to continue...";
            cin.ignore();
            cin.get();
            return;
        }

        /* ===== Auto-generate next Truck ID ===== */
        int id = 1;
        for (int i = 0; i < truckCount; i++)
        {
            if (trucks[i].truckID >= id)
                id = trucks[i].truckID + 1;
        }

        char driver[50];
        char city[30];
        char skill[20];
        int cap = 0, maxP = 0;

        cout << "\n  ┌────────────── ADD NEW TRUCK ──────────────┐\n";
        cout << "  │  Auto Assigned Truck ID: " << id << "\n";
        cout << "  └──────────────────────────────────────────┘\n\n";

        /* ===== Driver Name ===== */
        while (true)
        {
            cout << "  Driver Name: ";
            cin.ignore();
            cin.getline(driver, 50);

            if (strlen(driver) == 0)
                cout << "  [ERROR] Driver name cannot be empty.\n";
            else
                break;
        }

        /* ===== Driver City ===== */
        cout << "\n  Registered Cities:\n  ";
        routingNetwork.displayCityList();
        cout << "\n";

        while (true)
        {
            cout << "  Driver City: ";
            cin.getline(city, 30);

            if (!Validator::isValidCity(city))
                cout << "  [ERROR] Invalid city format.\n";
            else if (!routingNetwork.cityExists(city))
                cout << "  [ERROR] City not registered in routing network.\n";
            else
                break;
        }

        /* ===== Driver Skill ===== */
        while (true)
        {
            cout << "  Driver Skill (Excellent / Good / Fair): ";
            cin.getline(skill, 20);

            if (strcmp(skill, "Excellent") == 0 ||
                strcmp(skill, "Good") == 0 ||
                strcmp(skill, "Fair") == 0)
                break;
            else
                cout << "  [ERROR] Skill must be Excellent, Good, or Fair.\n";
        }

        /* ===== Capacity ===== */
        while (true)
        {
            cout << "  Capacity (kg): (NO space or comma!)  :";
            if (!(cin >> cap))
            {
                cout << "  [ERROR] Capacity must be numeric.\n";
                cin.clear();
                cin.ignore(10000, '\n');
            }
            else if (cap <= 0)
            {
                cout << "  [ERROR] Capacity must be greater than zero.\n";
            }
            else
            {
                break;
            }
        }

        /* ===== Max Parcels ===== */
        while (true)
        {
            cout << "  Max Parcels: (NO space or comma!)  :";
            if (!(cin >> maxP))
            {
                cout << "  [ERROR] Parcel limit must be numeric.\n";
                cin.clear();
                cin.ignore(10000, '\n');
            }
            else if (maxP <= 0)
            {
                cout << "  [ERROR] Parcel limit must be greater than zero.\n";
            }
            else
            {
                break;
            }
        }

        /* ===== Persist to file ===== */
        system("if not exist data mkdir data");
        ofstream file("data/trucks_db.txt", ios::app);

        if (!file.is_open())
        {
            cout << "\n  [ERROR] Unable to open truck database.\n";
            cout << "\n  Press Enter to continue...";
            cin.ignore();
            cin.get();
            return;
        }

        file << id << "|" << driver << "|" << cap << "|" << maxP
             << "|" << city << "|" << skill << "\n";
        file.close();

        /* ===== Runtime insert ===== */
        trucks[truckCount] = Truck(id, driver, cap, maxP);
        strcpy(trucks[truckCount].location, city);
        strcpy(trucks[truckCount].truckHealth, skill);
        truckCount++;

        char idStr[20];
        sprintf(idStr, "%d", id);
        logOperation("ADD_TRUCK", idStr, "New truck added");

        cout << "\n  ✓ Truck added successfully.\n";
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void addRoute()
    {
        system("cls || clear");
        char from[30], to[30];
        int dist;
        cout << "  From City: ";
        cin >> from;
        cout << "  To City: ";
        cin >> to;
        cout << "  Distance (km): ";
        cin >> dist;

        routingNetwork.addCity(from);
        routingNetwork.addCity(to);
        routingNetwork.addRoute(from, to, dist);

        char routeDetails[100];
        sprintf(routeDetails, "%s -> %s (%d km)", from, to, dist);
        logOperation("ADD_ROUTE", "", routeDetails);

        cout << "\n  ✓ Route added. Updated network:\n";
        routingNetwork.displayNetwork();
        cout << "\n  Press Enter to continue...";
        cin.ignore();
        cin.get();
    }
};

#endif // LOGISTICS_ENGINE_H