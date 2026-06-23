#include "io_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

std::string IoUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> IoUtils::load_names_from_file(const std::string& filename) {
    std::vector<std::string> names;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        std::string trimmed = trim(line);
        
        // Skip empty lines or comments starting with #
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        names.push_back(trimmed);
    }

    file.close();
    return names;
}

std::unique_ptr<Constraints> IoUtils::load_constraints(const std::string& filename) {
    auto constraints = std::make_unique<Constraints>();
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open constraints file: " + filename);
    }

    std::string line;
    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        std::string trimmed = trim(line);

        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        // Expected format: "Name1, Name2"
        size_t comma_pos = trimmed.find(',');
        if (comma_pos == std::string::npos) {
            std::cerr << "Warning: Skipping malformed constraint at line " 
                      << line_number << ": " << trimmed << "\n";
            continue;
        }

        std::string name1 = trim(trimmed.substr(0, comma_pos));
        std::string name2 = trim(trimmed.substr(comma_pos + 1));

        if (name1.empty() || name2.empty()) {
            std::cerr << "Warning: Empty name in constraint at line " << line_number << "\n";
            continue;
        }

        // Add constraint if both names exist in our system? 
        // For now, we just add them. The sorter will ignore pairs not in the roster if needed,
        // or strictly enforce them (better to enforce and warn if missing).
        constraints->add_separation(name1, name2);
    }

    file.close();
    return constraints;
}

bool IoUtils::save_groups_to_file(const std::string& filename, const struct SortResult& result) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Header
    file << "=== NAME GROUPING RESULTS ===\n";
    file << "Generated on: " << __DATE__ << " " << __TIME__ << "\n";
    file << "Total Groups: " << result.groups.size() << "\n";
    file << "Status: " << (result.success ? "SUCCESS" : "WARNING: Constraints not fully met") << "\n";
    file << "=================================\n\n";

    for (size_t i = 0; i < result.groups.size(); ++i) {
        file << "--- Group " << (i + 1) << " (" << result.groups[i].size() << " members) ---\n";
        
        if (result.groups[i].empty()) {
            file << "  [Empty]\n";
        } else {
            for (const auto& name : result.groups[i]) {
                file << "  • " << name << "\n";
            }
        }
        file << "\n";
    }

    if (!result.success) {
        file << "⚠️  NOTE: " << result.error_message << "\n";
    }

    file.close();
    return true;
}
