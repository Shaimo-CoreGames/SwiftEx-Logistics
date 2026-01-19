#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <cstring>
#include <cctype>

// ==================== VALIDATION UTILITIES ====================
class Validator {
public:
    static bool isValidName(const char* name) {
        if (strlen(name) == 0 || strlen(name) > 49) return false;
        for (int i = 0; name[i] != '\0'; i++) {
            if (!isalpha(name[i]) && name[i] != ' ') return false;
        }
        return true;
    }
    
    static bool isValidCity(const char* city) {
        if (strlen(city) == 0 || strlen(city) > 29) return false;
        for (int i = 0; city[i] != '\0'; i++) {
            if (!isalpha(city[i]) && city[i] != ' ') return false;
        }
        return true;
    }
    
    static bool isValidAddress(const char* address) {
        if (strlen(address) == 0 || strlen(address) > 99) return false;
        return true;
    }
    
    static bool isValidWeight(double weight) {
        return weight > 0 && weight <= 500;
    }
    
    static bool isValidPriority(int priority) {
        return priority >= 0 && priority <= 2;
    }
};

#endif // VALIDATOR_H
