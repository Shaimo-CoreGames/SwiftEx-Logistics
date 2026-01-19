# 🚚 SwiftEx Logistics Engine

**SwiftEx** is a high-performance, console-based Logistics & Supply Chain Management System architected in **C++**. It simulates a full-scale industrial environment—from secure admin authentication to complex graph-based route optimization—across a detailed network of over 40 cities in Pakistan.

---

## 🌟 Key Technical Highlights

### 📍 Advanced Route Optimization
The engine utilizes a custom implementation of **Dijkstra’s Algorithm** and **Yen’s K-Shortest Paths**. This allows the system to provide not just the fastest route, but also viable backup paths in case of network congestion or delivery blockages.

### 🛠️ Custom Data Structures (Zero-STL Emphasis)
To maximize performance and demonstrate low-level mastery, this project implements several core data structures from scratch:
* **Min-Heap Priority Queue:** Dynamically sorts parcels so "Overnight" shipments are always processed first.
* **Chained Hash Map:** Achieves near $O(1)$ time complexity for parcel tracking and database lookups.
* **Linked List Queue:** Manages the sequential flow of warehouse transit and final delivery pipelines.
* **Dynamic Operation Stack:** Maintains a rigorous log of system activities, supporting session history and undo operations.

### 🔐 Security & Fleet Management
* **Caesar Cipher Encryption:** Admin credentials (Passwords and CNICs) are encrypted before being persisted to the database.
* **Fleet Health Tracking:** Trucks are monitored for load utilization (kg), parcel counts, and mechanical status.

---

## 🏗️ System Architecture

The project follows a modular, header-only design for clean separation of concerns:

| Component | Responsibility |
| :--- | :--- |
| `routing.h` | Graph adjacency lists, Dijkstra, and Yen’s algorithms. |
| `hashmap.h` | High-speed indexing for the parcel registry. |
| `datastructures.h` | Core logic for Heaps, Queues, and the Operation Stack. |
| `logistics.h` | Central Logistics Engine coordinating all modules. |
| `auth.h` | Secure login, password recovery, and identity verification. |
| `parcel.h` / `truck.h` | Entity definitions for shipments and vehicles. |

---

## 📊 Data Flow & Lifecycle

1. **Registration:** Inputs are validated (Names, Weights, Cities) and stored in `data/parcels_db.txt`.
2. **Sorting:** Parcels enter the **Min-Heap** where they are prioritized by service level (Overnight > 2-Day > Standard).
3. **Pathfinding:** The system analyzes the city graph to compute the most efficient delivery route.
4. **Dispatch:** Parcels are assigned to trucks based on available weight capacity and volume limits.
5. **Tracking:** Every movement is logged into a **Tracking History** timeline for the end-user.

---

## ⚙️ Core Modules & Functionality

### 1. Intelligent Sorting
* **Priority-Based:** Overnight, 2-Day, and Normal categories.
* **Weight Constraints:** Automatically prevents overloading vehicles (Max 500kg per parcel).

### 2. Routing & Topology
* **Network Resilience:** Detects blocked routes and offers alternative paths.
* **Distance Metrics:** Calculates exact distances and estimated travel times.

### 3. Operations & Logs
* **Real-time Dashboard:** Displays fleet utilization percentages and warehouse throughput.
* **History & Logs:** Operation logs allow admins to review recent system changes.

---

## 🛠️ Installation & Usage

### Prerequisites
* **Compiler:** GCC / MinGW (C++11 or higher).
* **OS:** Windows (required for `windows.h` console management).

### Compilation
```bash
g++ main.cpp -o SwiftEx.exe
./SwiftEx.exe


-------------------------
🔑 Default Admin Credentials
Username: admin

Password: admin123

CNIC: 3520212345671
-------------------------
