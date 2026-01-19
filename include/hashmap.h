#ifndef HASHMAP_H
#define HASHMAP_H

#include <iostream>
#include <cstring>

// ==================== HASH MAP TEMPLATE ====================
template<typename K, typename V>
struct HashNode {
    K key;
    V value;
    HashNode* next;
    
    HashNode(K k, V v) : key(k), value(v), next(nullptr) {}
};

template<typename K, typename V>
class HashMap {
private:
    static const int TABLE_SIZE = 100;
    HashNode<K, V>* table[TABLE_SIZE];
    
    int hashFunction(K key) {
        int hash = 0;
        for (int i = 0; key[i] != '\0'; i++) {
            hash = (hash * 31 + key[i]) % TABLE_SIZE;
        }
        return (hash < 0) ? -hash : hash;
    }
    
public:
    HashMap() {
        for (int i = 0; i < TABLE_SIZE; i++)
            table[i] = nullptr;
    }
    
    void insert(K key, V value) {
        int idx = hashFunction(key);
        HashNode<K, V>* newNode = new HashNode<K, V>(key, value);
        
        if (!table[idx]) {
            table[idx] = newNode;
        } else {
            HashNode<K, V>* curr = table[idx];
            while (curr->next)
                curr = curr->next;
            curr->next = newNode;
        }
    }
    
    V* search(K key) {
        int idx = hashFunction(key);
        HashNode<K, V>* curr = table[idx];
        
        while (curr) {
            if (strcmp(curr->key, key) == 0)
                return &curr->value;
            curr = curr->next;
        }
        return nullptr;
    }
    
    // Get all values for iteration
    void getAllValues(V* values, int& count, int maxSize) {
        count = 0;
        for (int i = 0; i < TABLE_SIZE && count < maxSize; i++) {
            HashNode<K, V>* curr = table[i];
            while (curr && count < maxSize) {
                values[count++] = curr->value;
                curr = curr->next;
            }
        }
    }
    
    // Get count of all items
    int getCount() {
        int count = 0;
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode<K, V>* curr = table[i];
            while (curr) {
                count++;
                curr = curr->next;
            }
        }
        return count;
    }
    
    ~HashMap() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode<K, V>* curr = table[i];
            while (curr) {
                HashNode<K, V>* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }
};

#endif // HASHMAP_H
