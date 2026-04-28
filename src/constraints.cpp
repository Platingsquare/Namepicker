#include "../include/namepicker.h"
#include <algorithm>
#include <random>
#include <numeric>

std::string ConstraintManager::normalizePair(const std::string& a, const std::string& b) const {
    if (a < b) return a + "," + b;
    return b + "," + a;
}

void ConstraintManager::addConstraint(const std::string& name1, const std::string& name2) {
    if (name1 != name2) {
        forbiddenPairs.insert({name1, name2});
    }
}

bool ConstraintManager::hasConflict(const std::vector<Student>& group) const {
    for (size_t i = 0; i < group.size(); ++i) {
        for (size_t j = i + 1; j < group.size(); ++j) {
            std::string pairKey = normalizePair(group[i].name, group[j].name);
            if (forbiddenPairs.count(pairKey)) {
                return true;
            }
        }
    }
    return false;
}

bool ConstraintManager::isValidConfiguration(const std::vector<Group>& groups) const {
    for (const auto& group : groups) {
        if (hasConflict(group.members)) {
            return false;
        }
    }
    return true;
}

std::vector<Group> generateGroups(std::vector<Student> names, int numGroups, 
                                  const ConstraintManager& constraints, int maxAttempts) {
    std::vector<Group> groups(numGroups);
    for (int i = 0; i < numGroups; ++i) {
        groups[i].id = i + 1;
    }

    if (names.empty()) return groups;
    if (numGroups <= 0) numGroups = 1;

    std::random_device rd;
    std::mt19937 g(rd());

    int attempts = 0;
    while (attempts < maxAttempts) {
        // Shuffle names
        std::shuffle(names.begin(), names.end(), g);

        // Distribute (Round Robin)
        for (size_t i = 0; i < groups.size(); ++i) {
            groups[i].members.clear();
        }
        for (size_t i = 0; i < names.size(); ++i) {
            groups[i % numGroups].members.push_back(names[i]);
        }

        // Check constraints
        if (constraints.isValidConfiguration(groups)) {
            return groups; // Success!
        }

        attempts++;
    }

    std::cerr << "Warning: Could not satisfy all constraints after " << maxAttempts << " attempts.\n";
    std::cerr << "Returning best effort (may contain conflicts).\n";
    return groups;
}