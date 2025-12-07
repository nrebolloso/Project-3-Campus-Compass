#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream> 
#include <list>

using namespace std;

struct Edge {
    int to;
    double weight;
    bool isOpen = true;
};

struct ClassInfo {
    string code;
    int locationId;
    int startTime;
    int endTime;
};

struct Student {
    string name;
    string ufid;
    int residenceId;
    vector<string> classCodes;
};

class CampusCompass {
private:
    unordered_map<int, vector<Edge>> adjList;
    unordered_map<string, ClassInfo> classes;
    unordered_map<string, Student> students;

    void insertStudent(stringstream& ss);
    void removeStudent(string ufid);
    void dropClass(string ufid, string classCode);
    void replaceClass(string ufid, string oldCode, string newCode);
    void removeClassFromAll(string classCode);
    
    void addEdge(int u, int v, double weight);
    void toggleEdges(int n, stringstream& ss);
    void checkEdgeStatus(int u, int v);
    void isConnected(int u, int v);

    unordered_map<int, double> getShortestPaths(int startNode);
    pair<unordered_map<int, double>, unordered_map<int, int>> getShortestPathsWithParents(int startNode);

    void printShortestEdges(string ufid);
    void printStudentZone(string ufid);

    void verifySchedule(string ufid);

    int parseTime(string timeStr);
    bool isValidUFID(const string& id);
    bool isValidName(const string& name);
    bool isValidClassCode(const string& code);
public:
    CampusCompass();
    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseCommand(const string &command);
};