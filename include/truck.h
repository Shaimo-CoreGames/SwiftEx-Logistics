#ifndef TRUCK_H
#define TRUCK_H

#include <iostream>
#include <cstring>
#include <iomanip>

// ==================== TRUCK MANAGEMENT SYSTEM ====================
class Truck {
public:
    int truckID;
    char driverName[50];
    int capacity;
    int currentLoad;
    int parcelCount;
    char location[30];
    char truckHealth[20]; // Excellent, Good, Fair, Poor
    int maxParcels;
    
    Truck() : truckID(-1), capacity(1000), currentLoad(0), parcelCount(0), maxParcels(50) {
        strcpy(driverName, "Unassigned");
        strcpy(location, "Depot");
        strcpy(truckHealth, "Excellent");
    }
    
    Truck(int id, const char* driver, int cap, int maxP) 
        : truckID(id), capacity(cap), currentLoad(0), parcelCount(0), maxParcels(maxP) {
        strcpy(driverName, driver);
        strcpy(location, "Depot");
        strcpy(truckHealth, "Excellent");
    }
    
    bool canAddParcel(double weight) const {
        return (currentLoad + weight <= capacity) && (parcelCount < maxParcels);
    }
    
    void addParcel(double weight) {
        currentLoad += weight;
        parcelCount++;
        updateHealth();
    }
    
    void removeParcel(double weight) {
        currentLoad -= weight;
        parcelCount--;
        if (currentLoad < 0) currentLoad = 0;
        updateHealth();
    }
    
    void updateHealth() {
        double usage = (double)currentLoad / capacity * 100;
        if (usage < 30) {
            strcpy(truckHealth, "Excellent");
        } else if (usage < 60) {
            strcpy(truckHealth, "Good");
        } else if (usage < 85) {
            strcpy(truckHealth, "Fair");
        } else {
            strcpy(truckHealth, "Poor");
        }
    }
    
    int getAvailableSpace() const {
        return capacity - currentLoad;
    }
    
    void display() const {
        std::cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
        
        char buf[100];
        
        sprintf(buf, " Truck ID: %d", truckID);
        std::cout << "  │ " << std::left << std::setw(61) << buf << "│\n";
        
        sprintf(buf, " Driver: %s", driverName);
        std::cout << "  │ " << std::left << std::setw(61) << buf << "│\n";
        
        sprintf(buf, " Location: %s", location);
        std::cout << "  │ " << std::left << std::setw(61) << buf << "│\n";
        
        sprintf(buf, " Capacity: %d/%d kg", currentLoad, capacity);
        std::cout << "  │ " << std::left << std::setw(61) << buf << "│\n";
        
        sprintf(buf, " Parcels: %d/%d", parcelCount, maxParcels);
        std::cout << "  │ " << std::left << std::setw(61) << buf << "│\n";
        
        sprintf(buf, " Health Status: %s", truckHealth);
        std::cout << "  │ " << std::left << std::setw(61) << buf << "│\n";
        
        sprintf(buf, " Space Left: %d kg", getAvailableSpace());
        std::cout << "  │ " << std::left << std::setw(61) << buf << "│\n";
        
        std::cout << "  └─────────────────────────────────────────────────────────────┘\n";
    }
};

#endif // TRUCK_H
