/*
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "CampusCompass.h"

using namespace std;

string captureOutput(CampusCompass& cc, string command) {
    stringstream buffer;

    streambuf* old = cout.rdbuf(buffer.rdbuf());
    cc.ParseCommand(command);
    cout.rdbuf(old);
    
    return buffer.str();
}

void setupTestFiles() {
    ofstream edges("test_edges.csv");
    edges << "LocationID_1,LocationID_2,Name_1,Name_2,Time\n";
    edges << "1,2,Hall A,Hall B,10\n";
    edges << "2,3,Hall B,Hall C,5\n";
    edges << "1,4,Hall A,Hall D,20\n";
    edges.close();

    ofstream classes("test_classes.csv");
    classes << "ClassCode,LocationID,StartTime,EndTime\n";
    classes << "COP3530,2,10:00,11:00\n";
    classes << "CDA3101,3,11:15,12:05\n";
    classes << "MAC2311,4,13:00,14:00\n";
    classes.close();
}

TEST_CASE("1. Incorrect Command Handling", "[rubric1]") {
    setupTestFiles();
    CampusCompass cc;
    cc.ParseCSV("test_edges.csv", "test_classes.csv");

    // 1. numbers in name
    REQUIRE(captureOutput(cc, "insert \"A11y\" 45679999 1 1 COP3530") == "unsuccessful\n");
    // 2. short UFID
    REQUIRE(captureOutput(cc, "insert \"John\" 123 1 1 COP3530") == "unsuccessful\n");
    // 3. invalid class code
    REQUIRE(captureOutput(cc, "insert \"John\" 88888888 1 1 FRM123") == "unsuccessful\n");
    // 4. invalid class count
    REQUIRE(captureOutput(cc, "insert \"John\" 88888888 1 0 COP3530") == "unsuccessful\n");
    // 5. duplicate ID insertion
    cc.ParseCommand("insert \"John\" 11112222 1 1 COP3530");
    REQUIRE(captureOutput(cc, "insert \"Copy\" 11112222 1 1 COP3530") == "unsuccessful\n");
}

TEST_CASE("2. Edge Cases", "[rubric2]") {
    setupTestFiles();
    CampusCompass cc;
    cc.ParseCSV("test_edges.csv", "test_classes.csv");
    cc.ParseCommand("insert \"Alice\" 11112222 1 2 COP3530 CDA3101");

    // 1. removing student that doesnt exist
    REQUIRE(captureOutput(cc, "remove 99999999") == "unsuccessful\n");

    // 2. drop a class student does not have
    REQUIRE(captureOutput(cc, "dropClass 11112222 MAC2311") == "unsuccessful\n");

    // 3. replacing a class with itself (or replace nonexistent class)
    REQUIRE(captureOutput(cc, "replaceClass 11112222 MAC2311 COP3530") == "unsuccessful\n");
}

TEST_CASE("3. Modification Commands (Drop, Remove, Replace)", "[rubric3]") {
    setupTestFiles();
    CampusCompass cc;
    cc.ParseCSV("test_edges.csv", "test_classes.csv");
    
    // Bob has COP3530 and CDA3101
    cc.ParseCommand("insert \"Bob\" 19284930 1 2 COP3530 CDA3101");

    SECTION("Test replaceClass") {
        // replace CDA3101 with MAC2311
        REQUIRE(captureOutput(cc, "replaceClass 19284930 CDA3101 MAC2311") == "successful\n");
    }

    SECTION("Test dropClass") {
        REQUIRE(captureOutput(cc, "dropClass 19284930 COP3530") == "successful\n");
    }

    SECTION("Test remove") {
        REQUIRE(captureOutput(cc, "remove 19284930") == "successful\n");
    }

    SECTION("Test removeClass") {
        // removes COP3530 from all students
        // Bob has it, count should be 1
        REQUIRE(captureOutput(cc, "removeClass COP3530") == "1\n");
    }
}

TEST_CASE("4. Path Reachability Toggle", "[rubric4]") {
    setupTestFiles();
    CampusCompass cc;
    cc.ParseCSV("test_edges.csv", "test_classes.csv");

    // student at location 1, COP3530 is at location 2
    // edge 1-2 exists with weight 10
    cc.ParseCommand("insert \"Johnny\" 56393054 1 1 COP3530");

    // 1. initial state: reachable
    string expectedReachable = "Name: Johnny\nCOP3530 | Total Time: 10\n";
    REQUIRE(captureOutput(cc, "printShortestEdges 56393054") == expectedReachable);

    // 2. edge OFF
    cc.ParseCommand("toggleEdgesClosure 1 1 2");

    // 3. new state: unreachable (should be -1)
    string expectedUnreachable = "Name: Johnny\nCOP3530 | Total Time: -1\n";
    REQUIRE(captureOutput(cc, "printShortestEdges 56393054") == expectedUnreachable);
}

TEST_CASE("5. Student Zone MST Logic", "[logic]") {
    setupTestFiles();
    CampusCompass cc;
    cc.ParseCSV("test_edges.csv", "test_classes.csv");

    // student at location 1, path to 2 (cost 10), path to 3 (cost 15) -> via 2. subgraph is nodes {1, 2, 3}
    // edges: (1-2, weight 10) + (2-3, weight 5) = 15
    cc.ParseCommand("insert \"JohhnyWalker\" 67676767 1 2 COP3530 CDA3101");

    REQUIRE(captureOutput(cc, "printStudentZone 67676767") == "Student Zone Cost For JohhnyWalker: 15\n");
}
*/