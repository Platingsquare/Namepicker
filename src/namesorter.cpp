#include "namesorter.h"
#include "constraints.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>

NameSorter::NameSorter(int num_groups, std::mt19937_64&& rng)
    : m_num_groups(num_groups), m_rng(std::move(rng)) {
    
    if (m_num_groups <= 0) {
        throw std::invalid_argument("Number of groups must be positive.");
    }
}

void NameSorter::set_constraints(std::unique_ptr<Constraints> constraints) {
    m_constraints = std::move(constraints);
}

/**
 * Check if placing a name in a specific group violates any "separation" constraints.
 * If no constraints exist, this always returns true.
 */
bool NameSorter::is_valid_placement(size_t group_idx, const std::string& name) const {
    if (!m_constraints) {
        return true;
    }

    // Check against all members already in this group
    for (const auto& existing_member : m_groups[group_idx]) {
        if (m_constraints->must_be_separated(name, existing_member)) {
            return false;
        }
    }
    return true;
}

void NameSorter::shuffle_names(std::vector<std::string>& names) {
    std::shuffle(names.begin(), names.end(), m_rng);
}

/**
 * Backtracking algorithm to assign names to groups.
 * 
 * Strategy:
 * 1. Try to place the current name in a valid group.
 * 2. "Valid" means:
 *    - Group has capacity (logic handles balance automatically via round-robin/shuffle approach, 
 *      but strictly we just need to fill up).
 *    - No constraint violation with existing members.
 * 3. If a name cannot be placed anywhere, backtrack.
 * 
 * Note: For very large datasets, pure backtracking can be slow. 
 * However, for typical classroom sizes (<100), it works well with randomized order.
 */
bool NameSorter::backtrack_assign(size_t name_index, const std::vector<std::string>& names) {
    if (name_index == names.size()) {
        return true; // All names assigned successfully
    }

    const std::string& current_name = names[name_index];

    // Determine target group sizes to ensure balance
    // Total names / num_groups = base size
    // remainder names get +1
    size_t total_names = names.size();
    size_t base_size = total_names / m_num_groups;
    size_t remainder = total_names % m_num_groups;

    // We want to try groups that have room first.
    // To ensure randomness, we shuffle the order of group indices to try.
    std::vector<size_t> group_indices(m_num_groups);
    std::iota(group_indices.begin(), group_indices.end(), 0);
    std::shuffle(group_indices.begin(), group_indices.end(), m_rng);

    for (size_t group_idx : group_indices) {
        // Calculate max allowed size for this specific group
        // Groups 0 to remainder-1 should have base_size + 1
        // Groups remainder to end should have base_size
        size_t max_allowed = (group_idx < remainder) ? (base_size + 1) : base_size;

        if (m_groups[group_idx].size() >= max_allowed) {
            continue; // This group is full
        }

        // Check constraints
        if (!is_valid_placement(group_idx, current_name)) {
            continue; // Violation
        }

        // Assign
        m_groups[group_idx].push_back(current_name);

        // Recurse
        if (backtrack_assign(name_index + 1, names)) {
            return true;
        }

        // Backtrack
        m_groups[group_idx].pop_back();
    }

    return false; // No valid placement found for this name
}

std::optional<SortResult> NameSorter::sort_names(const std::vector<std::string>& names) {
    if (names.empty()) {
        SortResult res;
        res.success = true;
        res.groups.resize(m_num_groups);
        return res;
    }

    if (m_num_groups > static_cast<int>(names.size()) && names.size() > 0) {
        // If more groups than people, some will be empty. This is allowed but odd.
        // We proceed, but the user might want to know.
    }

    // Initialize empty groups
    m_groups.assign(m_num_groups, {});

    // Make a copy of names to shuffle
    std::vector<std::string> working_names = names;
    
    // Randomize order to prevent bias (e.g., always putting A-Z first in group 1)
    shuffle_names(working_names);

    // Attempt to assign
    if (!backtrack_assign(0, working_names)) {
        SortResult res;
        res.success = false;
        res.error_message = "Could not satisfy all constraints while maintaining balanced groups. "
                            "Try removing some separation constraints or adding more slots.";
        return res;
    }

    SortResult result;
    result.success = true;
    result.groups = std::move(m_groups);
    return result;
}
