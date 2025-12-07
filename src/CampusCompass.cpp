#include "CampusCompass.h"

#include <iomanip>
#include <algorithm>
#include <queue>
#include <limits>
#include <unordered_set>

using namespace std;

CampusCompass::CampusCompass() {
    //empty
}

//                                                                           SECTION 1: UTILITY/HELPERS

// ####################################################################################################


int CampusCompass::parseTime(string timeStr) {
    size_t splitIndex = timeStr.find(':');
    if (splitIndex == string::npos) return 0;
    int h = stoi(timeStr.substr(0, splitIndex));
    int m = stoi(timeStr.substr(splitIndex + 1));
    return (h * 60) + m;
}

bool CampusCompass::isValidUFID(const string& id) {
    if (id.length() != 8) return false;
    for (char c : id)
    {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool CampusCompass::isValidName(const string& name) {
    for (char c : name)
    {
        if (!isalpha(c) && c != ' ') return false;
    }
    return true;
}

bool CampusCompass::isValidClassCode(const string& code) {
    if (code.length() != 7) return false;
    for (int i = 0; i < 3; i++) if (!isupper(code[i])) return false;
    for (int i = 3; i < 7; i++) if (!isdigit(code[i])) return false;
    return true;
}

//                                                                          SECTION 2: GRAPH ALGORITHMS

// ####################################################################################################

void CampusCompass::addEdge(int u, int v, double weight) {
    adjList[u].push_back({v, weight, true}); //compiler passes as an edge struct
    adjList[v].push_back({u, weight, true});
}

void CampusCompass::toggleEdges(int n, stringstream& ss) {
    vector<int> nodes;
    int id;
    while(ss >> id) nodes.push_back(id);

    for (size_t i = 0; i < nodes.size(); i += 2)
    {
        if (i + 1 >= nodes.size()) break;
        int u = nodes[i];
        int v = nodes[i+1];

        bool found = false;
        if (adjList.find(u) != adjList.end())
        {
            for (auto& edge : adjList[u])
            {
                if (edge.to == v)
                {
                    edge.isOpen = !edge.isOpen;
                    found = true;
                    break;
                }
            }
        }
        if (found && adjList.find(v) != adjList.end())
        {
            for (auto& edge : adjList[v])
            {
                if (edge.to == u)
                {
                    edge.isOpen = !edge.isOpen;
                    break;
                }
            }
        }
    }
    cout << "successful" << endl;
}

void CampusCompass::checkEdgeStatus(int u, int v) {
    if (adjList.find(u) == adjList.end()) {
        cout << "DNE" << endl; 
        return;
    }
    for (const auto& edge : adjList[u]) {
        if (edge.to == v) {
            cout << (edge.isOpen ? "open" : "closed") << endl;
            return;
        }
    }
    cout << "DNE" << endl;
}

void CampusCompass::isConnected(int u, int v) {
    if (adjList.find(u) == adjList.end() || adjList.find(v) == adjList.end()) {
        cout << "unsuccessful" << endl; 
        return;
    }
    
    unordered_set<int> visited;
    queue<int> q;
    q.push(u);
    visited.insert(u);
    
    while(!q.empty()){
        int curr = q.front();
        q.pop();
        
        if (curr == v) {
            cout << "successful" << endl; 
            return;
        }
        
        for(const auto& edge : adjList[curr]){
            if(edge.isOpen && visited.find(edge.to) == visited.end()){
                visited.insert(edge.to);
                q.push(edge.to);
            }
        }
    }
    cout << "unsuccessful" << endl;
}

unordered_map<int, double> CampusCompass::getShortestPaths(int startNode) {
    unordered_map<int, double> dists;
    
    for(auto const& [node, edges] : adjList)
    {
        dists[node] = numeric_limits<double>::max(); // set every node to max
    }
    dists[startNode] = 0;

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push({0, startNode});

    while(!pq.empty())
    {
        double d = pq.top().first; //time
        int u = pq.top().second; //locationID
        pq.pop();

        if (d > dists[u]) continue;

        if (adjList.find(u) != adjList.end())
        {
            for(const auto& edge : adjList[u])
            {
                if (edge.isOpen)
                {
                    if(dists[u] + edge.weight < dists[edge.to])
                    {
                        dists[edge.to] = dists[u] + edge.weight;
                        pq.push({dists[edge.to], edge.to});
                    }
                }
            }
        }
    }
    return dists;
}

void CampusCompass::printShortestEdges(string ufid) {
    if (students.find(ufid) == students.end())
    {
        cout << "unsuccessful" << endl; 
        return;
    }

    Student& s = students[ufid];
    unordered_map<int, double> dists = getShortestPaths(s.residenceId);

    cout << "Name: " << s.name << endl;
    
    vector<string> sortedCodes = s.classCodes;
    sort(sortedCodes.begin(), sortedCodes.end());

    for (const string& code : sortedCodes)
    {
        int locationId = classes[code].locationId;
        cout << code << " | Total Time: ";
        
        if (dists.find(locationId) == dists.end() || dists[locationId] == numeric_limits<double>::max())
        {
            cout << -1 << endl;
        }
        else cout << dists[locationId] << endl;
    }
}

pair<unordered_map<int, double>, unordered_map<int, int>> CampusCompass::getShortestPathsWithParents(int startNode) {
    unordered_map<int, double> dists;
    unordered_map<int, int> parent;
    
    for(auto const& [node, edges] : adjList) {
        dists[node] = numeric_limits<double>::max();
    }
    dists[startNode] = 0;
    parent[startNode] = -1;

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push({0, startNode});

    while (!pq.empty()) {
        double d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > dists[u]) continue;

        if (adjList.find(u) != adjList.end()) {
            for (const auto& edge : adjList[u]) {
                if (edge.isOpen) {
                    if (dists.find(edge.to) == dists.end()) dists[edge.to] = numeric_limits<double>::max();
                    
                    if (dists[u] + edge.weight < dists[edge.to]) {
                        dists[edge.to] = dists[u] + edge.weight;
                        parent[edge.to] = u;
                        pq.push({dists[edge.to], edge.to});
                    }
                }
            }
        }
    }
    return {dists, parent};
}

void CampusCompass::printStudentZone(string ufid) {
    if (students.find(ufid) == students.end()) {
        cout << "unsuccessful" << endl; return;
    }
    Student& s = students[ufid];

    auto result = getShortestPathsWithParents(s.residenceId);
    auto& dists = result.first;
    auto& parents = result.second;

    unordered_set<int> subgraphNodes;
    subgraphNodes.insert(s.residenceId);

    for (const string& code : s.classCodes) {
        int target = classes[code].locationId;
        if (dists[target] == numeric_limits<double>::max()) continue; 

        int curr = target;
        while (curr != -1 && curr != s.residenceId) {
            subgraphNodes.insert(curr);
            if (parents.find(curr) == parents.end()) break;
            curr = parents[curr];
        }
    }
    
    if (subgraphNodes.size() <= 1) {
        cout << "Student Zone Cost For " << s.name << ": 0" << endl;
        return;
    }

    double mstCost = 0;
    unordered_set<int> visited;
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    pq.push({0, s.residenceId});

    while (!pq.empty())
    {
        double weight = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (visited.find(u) != visited.end()) continue;
        
        visited.insert(u);
        mstCost += weight;

        if (adjList.find(u) != adjList.end())
        {
            for (const auto& edge : adjList[u]) //prim's algorithm
            {
                if (edge.isOpen && subgraphNodes.find(edge.to) != subgraphNodes.end())
                {
                    if (visited.find(edge.to) == visited.end())
                    {
                        pq.push({edge.weight, edge.to});
                    }
                }
            }
        }
    }
    cout << "Student Zone Cost For " << s.name << ": " << mstCost << endl;
}

void CampusCompass::verifySchedule(string ufid) {
    if (students.find(ufid) == students.end())
    {
        cout << "unsuccessful" << endl; 
        return;
    }

    Student& s = students[ufid];
    
    if (s.classCodes.size() <= 1)
    {
        cout << "unsuccessful" << endl;
        return;
    }

    vector<pair<int, string>> schedule;
    for (const string& code : s.classCodes)
    {
        schedule.push_back({classes[code].startTime, code});
    }
    sort(schedule.begin(), schedule.end());

    cout << "Schedule Check for " << s.name << ":" << endl;

    for (size_t i = 0; i < schedule.size() - 1; i++)
    {
        string code1 = schedule[i].second;
        string code2 = schedule[i+1].second;
        
        ClassInfo& c1 = classes[code1];
        ClassInfo& c2 = classes[code2];

        int timeDif = c2.startTime - c1.endTime;

        auto dists = getShortestPaths(c1.locationId);
        double travelTime = dists[c2.locationId];

        cout << code1 << " - " << code2 << " ";

        if (travelTime != numeric_limits<double>::max() && travelTime <= timeDif)
        {
            cout << "\"Can make it!\"" << endl;
        }
        else cout << "\"Cannot make it!\"" << endl;
    }
}

//                                                                        SECTION 3: STUDENT MANAGEMENT

// ####################################################################################################

void CampusCompass::insertStudent(stringstream& ss) {
    string name, ufid, resIdStr, nStr;
    
    ss >> ws;
    if (ss.peek() == '"')
    {
        ss >> quoted(name);
    }
    else
    {
        ss >> name;
    }

    ss >> ufid >> resIdStr >> nStr;

    if (!isValidName(name) || !isValidUFID(ufid))
    {
        cout << "unsuccessful" << endl;
        return;
    }

    if (students.find(ufid) != students.end())
    {
        cout << "unsuccessful" << endl;
        return;
    }

    int resId, n;
    try {
        resId = stoi(resIdStr);
        n = stoi(nStr);
    } catch (...) { cout << "unsuccessful" << endl; return; }

    if (n < 1 || n > 6)
    { 
         cout << "unsuccessful" << endl; return; 
    }

    vector<string> classCodes;
    for (int i = 0; i < n; i++)
    {
        string code;
        ss >> code;
        
        if (!isValidClassCode(code) || classes.find(code) == classes.end())
        {
            cout << "unsuccessful" << endl; return; 
        }
        classCodes.push_back(code);
    }
    
    string garbage;
    if (ss >> garbage)
    {
        cout << "unsuccessful" << endl; return; 
    }

    students[ufid] = {name, ufid, resId, classCodes};
    cout << "successful" << endl;
}

void CampusCompass::removeStudent(string ufid) {
    if (students.erase(ufid))
    {
        cout << "successful" << endl;
    } else {
        cout << "unsuccessful" << endl;
    }
}

void CampusCompass::dropClass(string ufid, string classCode) {
    if (students.find(ufid) == students.end()) { cout << "unsuccessful" << endl; return; }
    
    Student& s = students[ufid];
    auto it = find(s.classCodes.begin(), s.classCodes.end(), classCode);
    
    if (it == s.classCodes.end()) { cout << "unsuccessful" << endl; return; }

    s.classCodes.erase(it);

    if (s.classCodes.empty())
    {
        students.erase(ufid);
    }
    cout << "successful" << endl;
}

void CampusCompass::replaceClass(string ufid, string oldCode, string newCode) {
    if (students.find(ufid) == students.end() || classes.find(newCode) == classes.end())
    {
        cout << "unsuccessful" << endl; return;
    }

    Student& s = students[ufid];
    
    auto itOld = find(s.classCodes.begin(), s.classCodes.end(), oldCode);
    if (itOld == s.classCodes.end()) { cout << "unsuccessful" << endl; return; }

    auto itNew = find(s.classCodes.begin(), s.classCodes.end(), newCode);
    if (itNew != s.classCodes.end()) { cout << "unsuccessful" << endl; return; }

    *itOld = newCode;
    cout << "successful" << endl;
}

void CampusCompass::removeClassFromAll(string classCode) {
    int count = 0;
    
    for (auto it = students.begin(); it != students.end(); )
    {
        auto& codes = it->second.classCodes;
        auto codeIt = find(codes.begin(), codes.end(), classCode);
        
        if (codeIt != codes.end())
        {
            codes.erase(codeIt);
            
            if (codes.empty())
            {
                it = students.erase(it);
                count++;
            }
            else
            {
                count++;
                ++it;
            }
        }
        else ++it;
    }
    cout << count << endl; 
}

//                                                                           SECTION 4: COMMAND PARSING

// ####################################################################################################

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    ifstream edgeFile(edges_filepath);
    if (!edgeFile.is_open())
    {
        cerr << "Error: Unable to open " << edges_filepath << endl;
        return false;
    }

    string line;
    getline(edgeFile, line); // skip header

    while (getline(edgeFile, line))
    {
        if (line.empty()) continue;
        stringstream ss(line);
        string segment;
        vector<string> parts;
        
        while (getline(ss, segment, ',')) parts.push_back(segment);

        if (parts.size() >= 5) // ensure 5 columns
        {
            try {
                int u = stoi(parts[0]);
                int v = stoi(parts[1]);
                double w = stod(parts[4]);
                addEdge(u, v, w);
            } catch (...) { continue; }
        }
    }
    edgeFile.close();

    ifstream classFile(classes_filepath);
    if (!classFile.is_open())
    {
        cerr << "Error: Unable to open " << classes_filepath << endl;
        return false;
    }

    getline(classFile, line); // skip header

    while (getline(classFile, line))
    {
        if (line.empty()) continue;
        stringstream ss(line);
        string segment;
        vector<string> parts;

        while (getline(ss, segment, ',')) parts.push_back(segment);

        if (parts.size() >= 4) //ensure 4 columns
        {
            try{
                ClassInfo c;
                c.code = parts[0];
                c.locationId = stoi(parts[1]);
                c.startTime = parseTime(parts[2]);
                c.endTime = parseTime(parts[3]);
                classes[c.code] = c;
            } catch (...) { continue; }
        }
    }
    classFile.close();
    return true;
}

bool CampusCompass::ParseCommand(const string &command)
{
    if (command.empty()) return true;

    stringstream ss(command);
    string cmd;
    ss >> cmd; 

    if (cmd == "insert") {
        insertStudent(ss); 
    }
    else if (cmd == "remove") {
        string id; 
        ss >> id;
        removeStudent(id);
    }
    else if (cmd == "dropClass") {
        string id, code; 
        ss >> id >> code;
        dropClass(id, code);
    }
    else if (cmd == "replaceClass") {
        string id, c1, c2; 
        ss >> id >> c1 >> c2;
        replaceClass(id, c1, c2);
    }
    else if (cmd == "removeClass") {
        string code; 
        ss >> code;
        removeClassFromAll(code);
    }
    else if (cmd == "printShortestEdges") {
        string id; 
        ss >> id;
        printShortestEdges(id);
    }
    else if (cmd == "toggleEdgesClosure") {
        int n; ss >> n;
        toggleEdges(n, ss);
    }
    else if (cmd == "checkEdgeStatus") {
        int u, v; ss >> u >> v;
        checkEdgeStatus(u, v);
    }
    else if (cmd == "isConnected") {
        int u, v; ss >> u >> v;
        isConnected(u, v);
    }
    else if (cmd == "printStudentZone") {
        string id; ss >> id;
        printStudentZone(id);
    }
    else if (cmd == "verifySchedule") {
        string id; ss >> id;
        verifySchedule(id);
    }
    else {
        cout << "unsuccessful" << endl;
    }
    
    return true;
}