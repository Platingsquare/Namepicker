#include "../include/namepicker.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

// Helper to trim whitespace
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<Student> loadNamesFromFile(const std::string& filename) {
    std::vector<Student> names;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return names;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (!trimmed.empty()) {
            names.push_back({trimmed});
        }
    }
    file.close();
    return names;
}

std::vector<Student> collectNamesInteractive() {
    std::vector<Student> names;
    std::string input;
    
    std::cout << "\nEnter names (press Enter on empty line to finish):\n";
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        std::string trimmed = trim(input);
        if (trimmed.empty()) break;
        names.push_back({trimmed});
    }
    return names;
}

void saveGroupsToFile(const std::vector<Group>& groups, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file " << filename << std::endl;
        return;
    }

    for (const auto& group : groups) {
        file << "Group " << group.id << ":\n";
        for (const auto& student : group.members) {
            file << "  - " << student.name << "\n";
        }
        file << "\n";
    }
    file.close();
    std::cout << "Groups saved to " << filename << std::endl;
}

void printGroups(const std::vector<Group>& groups) {
    std::cout << "\n--- Generated Groups ---\n";
    for (const auto& group : groups) {
        std::cout << "Group " << group.id << " (" << group.members.size() << " members):\n";
        for (const auto& student : group.members) {
            std::cout << "  - " << student.name << "\n";
        }
        std::cout << "\n";
    }
}