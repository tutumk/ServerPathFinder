#ifndef ALGORITMS_H
#define ALGORITMS_H
#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

vector<vector<int>> AdjustBySize(const vector<vector<int>>& edges, int size) {
	vector<vector<int>> result;
	for (const auto& edge : edges) {
		int time = (size/edge[3])+edge[2];
		result.push_back({edge[0], edge[1], time});
    }
    return result;
}



pair<int, vector<int>> Bellman_Ford(int cities, vector<vector<int>>& flights, int src, int dst) {
    int stops = 1000;
    // Distance and predecessor arrays
    vector<int> dist(cities, INT_MAX);
    vector<int> pred(cities, -1);  // Tracks the predecessor for path reconstruction
    dist[src] = 0;

    for (int i = 0; i <= stops; i++) {
        vector<int> intermediate(dist);

        for (auto& flight : flights) {
            int u = flight[0];
            int v = flight[1];
            int cost = flight[2];

            // Relax edge (u, v) if reachable and results in a shorter path
            if (dist[u] != INT_MAX && intermediate[v] > dist[u] + cost) {
                intermediate[v] = dist[u] + cost;
                pred[v] = u;  // Update predecessor
            }
        }
        
        // Update distances for the next iteration
        dist = intermediate;
    }

    // If destination is unreachable within K stops, return -1
    if (dist[dst] == INT_MAX) return {-1, {}};

    // Reconstruct path from src to dst
    vector<int> path;
    for (int at = dst; at != -1; at = pred[at]) {
        path.push_back(at);
    }
    reverse(path.begin(), path.end());

    return {dist[dst], path};
}

string printPathAndCost(const vector<vector<int>>& edges, const pair<int, vector<int>>& path) {
    ostringstream output;

    if (path.first == -1) {
        output << "No path available.\n";
    } else {
        output << "Total time it took: " << path.first << "\n";
        output << "Path:\n";
        int previous = path.second.front();
        for (int node : path.second) {
            for (const auto& edge : edges) {
                if (edge[0] == previous && edge[1] == node) {
                    output << edge[0] << " to " << edge[1] << " with time " << edge[2] << "\n";
                    break;
                }
            }
            previous = node;
        }
    }

    return output.str();
}

string printGraph(const vector<vector<int>>& edges) {
    // Organize edges by their source vertex
    map<int, vector<pair<int, int>>> adjList;
    for (const auto& edge : edges) {
        int source = edge[0];
        int destination = edge[1];
        int cost = edge[2];
        adjList[source].push_back({destination, cost});
    }

    ostringstream output;
    for (const auto& [source, destinations] : adjList) {
        output << "Vertex " << source << ": ";
        for (size_t i = 0; i < destinations.size(); ++i) {
            output << "to " << destinations[i].first << " with cost " << destinations[i].second;
            if (i < destinations.size() - 1) output << ", ";
        }
        output << "\n";
    }
    return output.str();
}

string printGraphUnadjusted(const vector<vector<int>>& edges) {
    // Organize edges by their source vertex
    map<int, vector<pair<int, pair<int, int>>>> adjList;
    for (const auto& edge : edges) {
        int source = edge[0];
        int destination = edge[1];
        int delay = edge[2];
        int speed = edge[3];
        adjList[source].push_back({destination, {delay, speed}});
    }

    // Use ostringstream to build the string
    ostringstream output;
    for (const auto& [source, destinations] : adjList) {
        output << "Vertex " << source << ": ";
        for (size_t i = 0; i < destinations.size(); ++i) {
            output << "to " << destinations[i].first 
                   << " with delay " << destinations[i].second.first 
                   << " and speed " << destinations[i].second.second;
            if (i < destinations.size() - 1) output << ", ";
        }
        output << "\n";
    }

    return output.str();
}
typedef tuple<int, int, int, vector<int>> tupl;

pair<long, vector<int>> Dijkstra(int cities, vector<vector<int>>& flights, int src, int dst) {
    vector<vector<pair<int, int>>> adjList(cities);
    int stops = 1000;

    for (auto& flight : flights) {
        adjList[flight[0]].emplace_back(flight[1], flight[2]);
    }

    priority_queue<tupl, vector<tupl>, greater<tupl>> pq;
    pq.emplace(0, src, stops, vector<int>{src});

    while (!pq.empty()) {
        auto [cost, current, remaining_stops, path] = pq.top();
        pq.pop();

        if (current == dst) {
            return {cost, path};
        }

        if (remaining_stops >= 0) {
            for (auto& next : adjList[current]) {
                int next_city = next.first;
                int next_cost = next.second;

                vector<int> new_path = path;
                new_path.push_back(next_city);

                pq.emplace(cost + next_cost, next_city, remaining_stops - 1, new_path);
            }
        }
    }

    // If destination cannot be reached within given stops, return -1 and an empty path
    return {-1, {}};
}

bool readGraphFromFile(const string& filename, int& N, int& source, int& destination, vector<vector<int>>& edges) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }

    // Read number of vertices and edges
    string st;
    file >> N >>source>>destination;
    // Read each edge
    edges.clear();
    int source1, destination1, delay1, speed1;
        
    for (int i = 0; true; ++i) {
        
        int source, destination, delay, speed;
        file >> source >> destination >> delay >> speed;
        if(i=!0)
            if(source==source1&&destination==destination1&&delay==delay1&&speed==speed1)
                return true;
        source1=source;
        destination1=destination;
        delay1=delay;
        speed1=speed;
        edges.push_back({source, destination, delay, speed});
    }

    file.close();
    return true;
}

#endif