#pragma once

#include <vector>
#include <string>
#include <random>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>

// Forward declaration
class Constraints;

/**
 * @struct SortResult
 * @brief Container for the final grouping results
 */
struct SortResult {
    std::vector<std::vector<std::string>> groups;
    bool success;
    std::string error_message;
};

/**
 * @class NameSorter
 * @brief Core class responsible for randomizing and grouping names into balanced teams
 * 
 * Handles constraints (like separating specific pairs) and ensures group sizes
 * are as equal as possible (difference <= 1).
 */
class NameSorter {
public:
    /**
     * @brief Construct a new Name Sorter object
     * @param num_groups The number of groups to create
     * @param rng Reference to a seeded random number generator
     */
    explicit NameSorter(int num_groups, std::mt19937_64&& rng);

    /**
     * @brief Set constraints for the sorting process
     * @param constraints Unique pointer to constraint manager
     */
    void set_constraints(std::unique_ptr<Constraints> constraints);

    /**
     * @brief Perform the grouping operation
     * @param names List of names to be grouped
     * @return SortResult containing the groups or an error
     */
    std::optional<SortResult> sort_names(const std::vector<std::string>& names);

private:
    int m_num_groups;
    std::mt19937_64 m_rng;
    std::unique_ptr<Constraints> m_constraints;

    // Group tracking
    std::vector<std::vector<std::string>> m_groups;
    
    /**
     * @brief Helper to check if placing 'name' in 'group_idx' violates constraints
     */
    bool is_valid_placement(size_t group_idx, const std::string& name) const;

    /**
     * @brief Recursive backtracking function to assign names
     */
    bool backtrack_assign(size_t name_index, const std::vector<std::string>& names);

    /**
     * @brief Shuffle the names list before assignment to ensure randomness
     */
    void shuffle_names(std::vector<std::string>& names);
};
