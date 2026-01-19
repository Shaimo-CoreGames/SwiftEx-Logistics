#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <iostream>
#include <cstring>
#include "parcel.h"
#include "datetime.h"

// ==================== PRIORITY QUEUE ====================
class PriorityQueue {
private:
    Parcel** heap;
    int capacity;
    int size;
    
    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx]->priority < heap[parent]->priority) {
                Parcel* temp = heap[idx];
                heap[idx] = heap[parent];
                heap[parent] = temp;
                idx = parent;
            } else break;
        }
    }
    
    void heapifyDown(int idx) {
        while (true) {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int smallest = idx;
            
            if (left < size && heap[left]->priority < heap[smallest]->priority)
                smallest = left;
            if (right < size && heap[right]->priority < heap[smallest]->priority)
                smallest = right;
                
            if (smallest != idx) {
                Parcel* temp = heap[idx];
                heap[idx] = heap[smallest];
                heap[smallest] = temp;
                idx = smallest;
            } else break;
        }
    }
    
public:
    PriorityQueue(int cap = 1000) : capacity(cap), size(0) {
        heap = new Parcel*[capacity];
    }
    
    void insert(Parcel* p) {
        if (size >= capacity) {
            std::cout << "  [ERROR] Queue at capacity\n";
            return;
        }
        heap[size] = p;
        heapifyUp(size);
        size++;
    }
    
    Parcel* extractMin() {
        if (size == 0) return nullptr;
        
        Parcel* minParcel = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        return minParcel;
    }
    
    bool isEmpty() const { return size == 0; }
    int getSize() const { return size; }
    
    ~PriorityQueue() { delete[] heap; }
};

// ==================== STANDARD QUEUE ====================
template<typename T>
class QueueNode {
public:
    T data;
    QueueNode* next;
    QueueNode(T d) : data(d), next(nullptr) {}
};

template<typename T>
class Queue {
private:
    QueueNode<T>* front;
    QueueNode<T>* rear;
    int count;
    
public:
    Queue() : front(nullptr), rear(nullptr), count(0) {}
    
    void enqueue(T data) {
        QueueNode<T>* newNode = new QueueNode<T>(data);
        if (!rear) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        count++;
    }
    
    T dequeue() {
        if (!front) return nullptr;
        QueueNode<T>* temp = front;
        T data = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        count--;
        return data;
    }
    
    bool isEmpty() const { return front == nullptr; }
    int size() const { return count; }
    
    ~Queue() {
        while (front) {
            QueueNode<T>* temp = front;
            front = front->next;
            delete temp;
        }
    }
};

// ==================== STACK FOR UNDO OPERATIONS ====================
struct Operation {
    char type[30];
    char parcelID[20];
    char details[100];
    DateTime timestamp;
};

class OperationStack {
private:
    struct StackNode {
        Operation op;
        StackNode* next;
        StackNode(Operation o) : op(o), next(nullptr) {}
    };
    
    StackNode* top;
    int count;
    
public:
    OperationStack() : top(nullptr), count(0) {}
    
    void push(Operation op) {
        StackNode* newNode = new StackNode(op);
        newNode->next = top;
        top = newNode;
        count++;
    }
    
    Operation pop() {
        if (!top) {
            Operation empty;
            strcpy(empty.type, "NONE");
            return empty;
        }
        StackNode* temp = top;
        Operation op = top->op;
        top = top->next;
        delete temp;
        count--;
        return op;
    }
    
    bool isEmpty() const { return top == nullptr; }
    
    void displayHistory(int n = 10) {
        std::cout << "\n  ┌─────────────────────────────────────────────────────────────┐\n";
        std::cout << "  │  RECENT OPERATIONS                                          │\n";
        std::cout << "  ├─────────────────────────────────────────────────────────────┤\n";
        
        StackNode* curr = top;
        int displayed = 0;
        
        while (curr && displayed < n) {
            std::cout << "  │  ";
            curr->op.timestamp.print();
            std::cout << " | " << curr->op.type << ": " << curr->op.parcelID;
            
            int len = strlen(curr->op.type) + strlen(curr->op.parcelID) + 2;
            for (int i = len; i < 35; i++) std::cout << " ";
            std::cout << "│\n";
            
            curr = curr->next;
            displayed++;
        }
        
        std::cout << "  └─────────────────────────────────────────────────────────────┘\n";
    }
    
    ~OperationStack() {
        while (top) {
            StackNode* temp = top;
            top = top->next;
            delete temp;
        }
    }
};

#endif // DATASTRUCTURES_H
