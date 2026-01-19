#ifndef DATETIME_H
#define DATETIME_H

#include <iostream>
#include <ctime>

// ==================== UTILITY & TIME MANAGEMENT ====================
class DateTime {
public:
    int day, month, year, hour, minute;
    
    DateTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        
        day = ltm->tm_mday;
        month = 1 + ltm->tm_mon;
        year = 1900 + ltm->tm_year;
        hour = ltm->tm_hour;
        minute = ltm->tm_min;
    }
    
    void print() const {
        if (day < 10) std::cout << "0";
        std::cout << day << "/";
        if (month < 10) std::cout << "0";
        std::cout << month << "/" << year << " ";
        if (hour < 10) std::cout << "0";
        std::cout << hour << ":";
        if (minute < 10) std::cout << "0";
        std::cout << minute;
    }
};

#endif // DATETIME_H
