#pragma once

#include <string>
#include <vector>
#include "constraints.h" // We need this type for loading constraints

/**
 * @class IoUtils
 * @brief Static helper class for file input/output operations
 */
class IoUtils {
public:
    /**
     * @brief Load names from a text file (one name per line)
     * @param filename Path to the roster file
     * @return Vector of names, trimmed of whitespace
     */
    static std::vector<std::string> load_names_from_file(const std::string& filename);

    /**
     * @brief Load separation constraints from a text file
     * Format: "Name1, Name2" per line
     * @param filename Path to the constraints file
     * @return Unique pointer to a populated Constraints object
     */
    static std::unique_ptr<Constraints> load_constraints(const std::string& filename);

    /**
     * @brief Save the generated groups to a formatted text file
     * @param filename Output path
     * @param result The sorting result containing groups
     * @return true if successful, false otherwise
     */
    static bool save_groups_to_file(const std::string& filename, const struct SortResult& result);

private:
    // Helper to trim whitespace from strings
    static std::string trim(const std::string& str);
};
