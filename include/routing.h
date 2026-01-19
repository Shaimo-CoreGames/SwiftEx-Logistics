#ifndef ROUTING_H
#define ROUTING_H

#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

const int MAX_CITIES = 60;
const int INF = INT_MAX / 2;

/* ==================== GRAPH STRUCTURES ==================== */

class GraphNode {
public:
    char city[30];
    int weight;
    GraphNode* next;

    GraphNode(const char* c, int w) : weight(w), next(nullptr) {
        strcpy(city, c);
    }
};

class RoutingGraph {
private:
    struct CityNode {
        char name[30];
        GraphNode* edges;
        CityNode* next;

        CityNode(const char* n) : edges(nullptr), next(nullptr) {
            strcpy(name, n);
        }
    };

    CityNode* cities;

/* ==================== INTERNAL HELPERS ==================== */

    CityNode* findCity(const char* name) {
        CityNode* cur = cities;
        while (cur) {
            if (!strcmp(cur->name, name))
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    void getAllCities(char names[MAX_CITIES][30], int& count) {
        count = 0;
        CityNode* cur = cities;
        while (cur && count < MAX_CITIES) {
            strcpy(names[count++], cur->name);
            cur = cur->next;
        }
    }

    struct PathInfo {
        int dist;
        vector<int> path;
    };

    /* ==================== DIJKSTRA (WITH BLOCKING) ==================== */

    bool dijkstraInternal(
        int startIdx,
        int endIdx,
        const vector<pair<int,int>>& blockedEdges,
        const vector<int>& blockedNodes,
        PathInfo& out,
        char cityNames[MAX_CITIES][30],
        int cityCount
    ) {
        int dist[MAX_CITIES];
        int prev[MAX_CITIES];
        bool used[MAX_CITIES];

        for (int i = 0; i < cityCount; i++) {
            dist[i] = INF;
            prev[i] = -1;
            used[i] = false;
        }

        dist[startIdx] = 0;

        while (true) {
            int u = -1, best = INF;
            for (int i = 0; i < cityCount; i++) {
                if (!used[i] && dist[i] < best) {
                    best = dist[i];
                    u = i;
                }
            }

            if (u == -1) break;
            used[u] = true;

            // block nodes (Yen rule)
            for (int bn : blockedNodes)
                if (bn == u) goto skipNode;

            {
                CityNode* c = findCity(cityNames[u]);
                for (GraphNode* e = c->edges; e; e = e->next) {
                    int v = -1;
                    for (int i = 0; i < cityCount; i++)
                        if (!strcmp(cityNames[i], e->city))
                            v = i;

                    if (v == -1) continue;

                    bool blocked = false;
                    for (auto& b : blockedEdges)
                        if (b.first == u && b.second == v)
                            blocked = true;

                    if (blocked) continue;

                    if (dist[u] + e->weight < dist[v]) {
                        dist[v] = dist[u] + e->weight;
                        prev[v] = u;
                    }
                }
            }

        skipNode:;
        }

        if (dist[endIdx] == INF) return false;

        out.dist = dist[endIdx];
        out.path.clear();
        for (int v = endIdx; v != -1; v = prev[v])
            out.path.push_back(v);

        reverse(out.path.begin(), out.path.end());
        return true;
    }

/* ==================== PUBLIC INTERFACE ==================== */

public:
    RoutingGraph() : cities(nullptr) {}

    void addCity(const char* name) {
        if (findCity(name)) return;
        CityNode* c = new CityNode(name);
        c->next = cities;
        cities = c;
    }
int getCityCount() const {
    int count = 0;
    CityNode* curr = cities;
    while (curr) {
        count++;
        curr = curr->next;
    }
    return count;
}

int getRouteCount() const {
    int count = 0;
    CityNode* curr = cities;
    while (curr) {
        GraphNode* edge = curr->edges;
        while (edge) {
            count++;
            edge = edge->next;
        }
        curr = curr->next;
    }
    return count / 2; // because routes are bidirectional
}

    void addRoute(const char* from, const char* to, int dist) {
        CityNode* f = findCity(from);
        if (!f) return;
        GraphNode* e = new GraphNode(to, dist);
        e->next = f->edges;
        f->edges = e;
    }

    /* ==================== YEN’S ALGORITHM (k = 2) ==================== */

    void findShortestPath(const char* start, const char* end, int k = 2) {
        char cityNames[MAX_CITIES][30];
        int cityCount;
        getAllCities(cityNames, cityCount);

        int s = -1, t = -1;
        for (int i = 0; i < cityCount; i++) {
            if (!strcmp(cityNames[i], start)) s = i;
            if (!strcmp(cityNames[i], end))   t = i;
        }

        if (s == -1 || t == -1) {
            cout << "  [ERROR] City not found\n";
            return;
        }

        vector<PathInfo> A;
        vector<PathInfo> B;

        PathInfo shortest;
        if (!dijkstraInternal(s, t, {}, {}, shortest, cityNames, cityCount)) {
            cout << "  [INFO] No route available\n";
            return;
        }

        A.push_back(shortest);

        for (size_t i = 0; i + 1 < shortest.path.size(); i++) {
            vector<pair<int,int>> blockedEdges;
            vector<int> blockedNodes;

            blockedEdges.push_back({ shortest.path[i], shortest.path[i+1] });

            for (size_t j = 0; j < i; j++)
                blockedNodes.push_back(shortest.path[j]);

            PathInfo spur;
            if (dijkstraInternal(
                shortest.path[i],
                t,
                blockedEdges,
                blockedNodes,
                spur,
                cityNames,
                cityCount
            )) {
                PathInfo total;
                total.path.assign(shortest.path.begin(), shortest.path.begin() + i);
                total.path.insert(total.path.end(), spur.path.begin(), spur.path.end());
                total.dist = 0;

                for (size_t j = 0; j + 1 < total.path.size(); j++) {
                    CityNode* c = findCity(cityNames[total.path[j]]);
                    for (GraphNode* e = c->edges; e; e = e->next)
                        if (!strcmp(e->city, cityNames[total.path[j+1]]))
                            total.dist += e->weight;
                }

                B.push_back(total);
            }
        }

        sort(B.begin(), B.end(), [](const PathInfo& a, const PathInfo& b) {
            return a.dist < b.dist;
        });

        if (!B.empty())
            A.push_back(B[0]);

        cout << "\n  ═══════════════════════════════════════════════════════════\n";
        for (size_t i = 0; i < A.size(); i++) {
            cout << "   ROUTE " << i + 1 << ": ";
            for (size_t j = 0; j < A[i].path.size(); j++) {
                cout << cityNames[A[i].path[j]];
                if (j + 1 < A[i].path.size()) cout << " → ";
            }
            cout << "\n   Distance: " << A[i].dist
                 << " km | Time: " << (A[i].dist / 60.0) << " hours\n\n";
        }

        if (A.size() == 1)
            cout << "  [INFO] Only one viable route exists for this dispatch.\n";

        cout << "  ═══════════════════════════════════════════════════════════\n";
    }

    void displayNetwork() {
        CityNode* c = cities;
        cout << "\n  ROUTING NETWORK\n";
        while (c) {
            cout << "  " << c->name << " -> ";
            GraphNode* e = c->edges;
            while (e) {
                cout << e->city << "(" << e->weight << "km) ";
                e = e->next;
            }
            cout << "\n";
            c = c->next;
        }
    }
    bool cityExists(const char* name) {
    return findCity(name) != nullptr;
}
void displayCityList() {
    CityNode* curr = cities;
    while (curr) {
        cout << curr->name;
        if (curr->next) cout << ", ";
        curr = curr->next;
    }
    cout << "\n";
}


    ~RoutingGraph() {
        while (cities) {
            CityNode* c = cities;
            GraphNode* e = c->edges;
            while (e) {
                GraphNode* t = e;
                e = e->next;
                delete t;
            }
            cities = c->next;
            delete c;
        }
    }
};

#endif // ROUTING_H
