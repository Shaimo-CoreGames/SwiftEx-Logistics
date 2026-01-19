#ifndef TRACKING_H
#define TRACKING_H

#include <iostream>
#include <cstring>
#include "datetime.h"

// ==================== PARCEL TRACKING HISTORY ====================
struct TrackingEvent {
    char status[50];
    char location[50];
    DateTime timestamp;
    TrackingEvent* next;
    
    TrackingEvent(const char* s, const char* loc) : next(nullptr) {
        strcpy(status, s);
        strcpy(location, loc);
    }
};

class TrackingHistory {
private:
    TrackingEvent* head;
    TrackingEvent* tail;
    
public:
    TrackingHistory() : head(nullptr), tail(nullptr) {}
    
    void addEvent(const char* status, const char* location) {
        TrackingEvent* newEvent = new TrackingEvent(status, location);
        
        if (!head) {
            head = tail = newEvent;
        } else {
            tail->next = newEvent;
            tail = newEvent;
        }
    }
    
    void display() const {
        TrackingEvent* curr = head;
        std::cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
        std::cout << "  │  TRACKING TIMELINE                                          │\n";
        std::cout << "  ├─────────────────────────────────────────────────────────────┤\n";
        
        while (curr) {
            std::cout << "  │  ";
            curr->timestamp.print();
            std::cout << " | " << curr->status << " at " << curr->location;
            
            int padding = 55 - strlen(curr->status) - strlen(curr->location);
            for (int i = 0; i < padding; i++) std::cout << " ";
            std::cout << "│\n";
            
            curr = curr->next;
        }
        std::cout << "  └─────────────────────────────────────────────────────────────┘\n";
    }
    
    ~TrackingHistory() {
        while (head) {
            TrackingEvent* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

#endif // TRACKING_H
