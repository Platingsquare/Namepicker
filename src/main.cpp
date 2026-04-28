#include "../include/namepicker.h"
#include <iostream>
#include <string>
#include <cstdlib>

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "Options:\n"
              << "  -g, --groups <N>     Number of groups (default: interactive)\n"
              << "  -i, --input <file>   Load names from file\n"
              << "  -o, --output <file>  Save groups to file\n"
              << "  -x, --exclude <A,B>  Exclude pair A and B from same group (can be used multiple times)\n"
              << "  -s, --seed <N>       Set random seed for reproducibility\n"
              << "  -h, --help           Show this help message\n";
}

int main(int argc, char* argv[]) {
    int numGroups = 0;
    std::string inputFile, outputFile;
    std::vector<std::pair<std::string, std::string>> excludePairs;
    unsigned int seed = std::time(nullptr);

    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-g" || arg == "--groups") {
            if (i + 1 < argc) numGroups = std::atoi(argv[++i]);
        } else if (arg == "-i" || arg == "--input") {
            if (i + 1 < argc) inputFile = argv[++i];
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) outputFile = argv[++i];
        } else if (arg == "-x" || arg == "--exclude") {
            if (i + 1 < argc) {
                std::string pairStr = argv[++i];
                size_t commaPos = pairStr.find(',');
                if (commaPos != std::string::npos) {
                    std::string p1 = pairStr.substr(0, commaPos);
                    std::string p2 = pairStr.substr(commaPos + 1);
                    excludePairs.push_back({p1, p2});
                }
            }
        } else if (arg == "-s" || arg == "--seed") {
            if (i + 1 < argc) seed = std::atoi(argv[++i]);
        }
    }

    // Seed the random generator
    std::srand(seed);

    // Load Names
    std::vector<Student> names;
    if (!inputFile.empty()) {
        names = loadNamesFromFile(inputFile);
    } else {
        names = collectNamesInteractive();
    }

    if (names.empty()) {
        std::cout << "No names provided. Exiting.\n";
        return 0;
    }

    // Setup Constraints
    ConstraintManager constraints;
    for (const auto& pair : excludePairs) {
        constraints.addConstraint(pair.first, pair.second);
    }

    // Determine Group Count
    if (numGroups <= 0) {
        std::cout << "How many groups? ";
        std::cin >> numGroups;
        if (numGroups <= 0) numGroups = 1;
    }

    // Generate Groups
    std::vector<Group> groups = generateGroups(names, numGroups, constraints);

    // Output
    if (!outputFile.empty()) {
        saveGroupsToFile(groups, outputFile);
    } else {
        printGroups(groups);
    }

    return 0;
}