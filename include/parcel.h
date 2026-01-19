#ifndef PARCEL_H
#define PARCEL_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include "tracking.h"

// ==================== PARCEL ENTITY ====================
class Parcel {
public:
    char trackingID[20];
    char sender[50];
    char recipient[50];
    char origin[30];
    char destination[30];
    char destinationAddress[100];
    int priority; // 0=overnight, 1=2day, 2=normal
    double weight;
    bool isFragile;
    char currentStatus[30];
    char currentLocation[30];
    char parcelStatus[30]; // Accepted, Rejected, Waiting, InDelivery
    int deliveryAttempts;
    int truckID;
    TrackingHistory history;
    
    Parcel() : deliveryAttempts(0), truckID(-1) {
        strcpy(currentStatus, "Registered");
        strcpy(currentLocation, "Origin Hub");
        strcpy(parcelStatus, "Waiting");
    }
    
    void generateTrackingID() {
        sprintf(trackingID, "SWX%d%d%d", rand() % 1000, rand() % 1000, rand() % 100);
    }
    
    void display() const {
        std::cout << "\n╔═════════════════════════════════════════════════════════════╗\n";
        std::cout << "║              PARCEL INFORMATION                             ║\n";
        std::cout << "╠═════════════════════════════════════════════════════════════╣\n";
        
        char buf[200];
        
        sprintf(buf, " Tracking ID: %s", trackingID);
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        sprintf(buf, " Status: %s", currentStatus);
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        sprintf(buf, " Parcel Status: %s", parcelStatus);
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        sprintf(buf, " Location: %s", currentLocation);
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        sprintf(buf, " Route: %s → %s", origin, destination);
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        sprintf(buf, " Dest. Address: %s", destinationAddress);
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        const char* priorityStr[] = {"OVERNIGHT", "2-DAY", "STANDARD"};
        sprintf(buf, " Priority: %s", priorityStr[priority]);
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        sprintf(buf, " Weight: %.1f kg", weight);
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        sprintf(buf, " Fragile: %s", isFragile ? "YES" : "NO");
        std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        
        if (truckID != -1) {
            sprintf(buf, " Truck ID: %d", truckID);
            std::cout << "║ " << std::left << std::setw(60) << buf << "║\n";
        }
        
        std::cout << "╚═════════════════════════════════════════════════════════════╝\n";
    }
};

#endif // PARCEL_H
