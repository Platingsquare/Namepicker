#pragma once

#include <string>
#include <set>
#include <utility> // for std::pair

/**
 * @class Constraints
 * @brief Manages rules about who cannot be in the same group.
 * 
 * Stores pairs of names that must be separated.
 */
class Constraints {
public:
    /**
     * @brief Add a rule that two people must NOT be in the same group.
     */
    void add_separation(const std::string& name1, const std::string& name2);

    /**
     * @brief Check if two people must be separated.
     * @return true if they cannot be together, false otherwise.
     */
    bool must_be_separated(const std::string& name1, const std::string& name2) const;

private:
    // Stores normalized pairs (alphabetical order) to ensure A,B is same as B,A
    std::set<std::pair<std::string, std::string>> m_forbidden_pairs;

    /**
     * Helper to normalize pair order for consistent storage
     */
    std::pair<std::string, std::string> normalize_pair(const std::string& a, const std::string& b) const;
};
