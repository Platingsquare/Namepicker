/**
 * @file main.cpp
 * @brief Name Sorter - A CLI utility for random group allocation with constraints
 * 
 * Features:
 * - Balanced group distribution
 * - Constraint handling (e.g., separate specific people)
 * - Reproducible randomization via seed
 * - File I/O support (roster.txt)
 * - Modern C++17
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <optional>

// Include our custom modules
#include "namesorter.h"
#include "constraints.h"
#include "io_utils.h"

namespace fs = std::filesystem;

/**
 * Print usage information for the CLI
 */
void print_usage(const char* program_name) {
    std::cout << "🎲 Namepicker / Namesorter v1.0\n";
    std::cout << "================================\n\n";
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --input FILE       Load names from FILE (required)\n";
    std::cout << "  -o, --output FILE      Export groups to FILE (default: groups.txt)\n";
    std::cout << "  -n, --groups NUM       Number of groups to create (required)\n";
    std::cout << "  -s, --seed VALUE       Seed for reproducible randomization\n";
    std::cout << "  -c, --constraint FILE  Load constraints from FILE\n";
    std::cout << "  --help                 Show this help message\n";
    std::cout << "  --version              Show version information\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " -i roster.txt -n 3\n";
    std::cout << "  " << program_name << " -i roster.txt -n 4 -s 42 -o results.txt\n";
    std::cout << "  " << program_name << " -i roster.txt -n 5 -c constraints.txt\n";
}

/**
 * Parse command line arguments
 */
struct CommandLineArgs {
    std::optional<std::string> input_file;
    std::optional<std::string> output_file;
    std::optional<int> num_groups;
    std::optional<uint64_t> seed;
    std::optional<std::string> constraint_file;
    bool show_help = false;
    bool show_version = false;
};

CommandLineArgs parse_arguments(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.show_help = true;
            return args;
        }
        
        if (arg == "--version" || arg == "-v") {
            args.show_version = true;
            return args;
        }
        
        if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            args.input_file = argv[++i];
        }
        else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            args.output_file = argv[++i];
        }
        else if ((arg == "-n" || arg == "--groups") && i + 1 < argc) {
            try {
                args.num_groups = std::stoi(argv[++i]);
                if (*args.num_groups <= 0) {
                    std::cerr << "Error: Number of groups must be positive.\n";
                    exit(1);
                }
            } catch (...) {
                std::cerr << "Error: Invalid number for groups.\n";
                exit(1);
            }
        }
        else if ((arg == "-s" || arg == "--seed") && i + 1 < argc) {
            try {
                args.seed = std::stoull(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid seed value.\n";
                exit(1);
            }
        }
        else if ((arg == "-c" || arg == "--constraint") && i + 1 < argc) {
            args.constraint_file = argv[++i];
        }
    }
    
    return args;
}

/**
 * Main entry point
 */
int main(int argc, char* argv[]) {
    // Parse command line arguments
    CommandLineArgs args = parse_arguments(argc, argv);
    
    // Show help or version if requested
    if (args.show_help) {
        print_usage(argv[0]);
        return 0;
    }
    
    if (args.show_version) {
        std::cout << "Namepicker / Namesorter v1.0\n";
        std::cout << "Built with C++17\n";
        return 0;
    }
    
    // Validate required arguments
    if (!args.input_file) {
        std::cerr << "❌ Error: Input file is required. Use -i or --input.\n";
        print_usage(argv[0]);
        return 1;
    }
    
    if (!args.num_groups) {
        std::cerr << "❌ Error: Number of groups is required. Use -n or --groups.\n";
        print_usage(argv[0]);
        return 1;
    }
    
    if (*args.num_groups > 1000) {
        std::cerr << "❌ Error: Maximum 1000 groups allowed.\n";
        return 1;
    }
    
    // Check if input file exists
    if (!fs::exists(*args.input_file)) {
        std::cerr << "❌ Error: Input file '" << *args.input_file << "' does not exist.\n";
        return 1;
    }
    
    std::cout << "🎯 Loading roster from: " << *args.input_file << "\n";
    
    // Load names from file
    std::vector<std::string> names = io_utils::load_names_from_file(*args.input_file);
    
    if (names.empty()) {
        std::cerr << "❌ Error: No valid names found in input file.\n";
        return 1;
    }
    
    std::cout << "✅ Loaded " << names.size() << " names.\n";
    
    // Check if we have enough people for the requested groups
    if (names.size() < static_cast<size_t>(*args.num_groups)) {
        std::cerr << "⚠️  Warning: More groups (" << *args.num_groups 
                  << ") than people (" << names.size() << "). Some groups will be empty.\n";
    }
    
    // Initialize random number generator
    std::mt19937_64 rng;
    
    if (args.seed.has_value()) {
        // Use provided seed for reproducibility
        rng.seed(args.seed.value());
        std::cout << "🔒 Using seed: " << args.seed.value() << " (reproducible)\n";
    } else {
        // Use time-based seed for randomness
        auto now = std::chrono::high_resolution_clock::now();
        auto seed_value = now.time_since_epoch().count();
        rng.seed(seed_value);
        std::cout << "🎲 Using random seed: " << seed_value << "\n";
    }
    
    // Load constraints if provided
    std::unique_ptr<Constraints> constraints;
    if (args.constraint_file.has_value()) {
        if (fs::exists(*args.constraint_file)) {
            std::cout << "📋 Loading constraints from: " << *args.constraint_file << "\n";
            constraints = std::make_unique<Constraints>(io_utils::load_constraints(*args.constraint_file));
        } else {
            std::cerr << "⚠️  Warning: Constraints file '" << *args.constraint_file 
                      << "' not found. Skipping constraints.\n";
        }
    }
    
    // Create name sorter and perform grouping
    std::cout << "\n🔄 Creating " << *args.num_groups << " balanced groups...\n";
    
    NameSorter sorter(*args.num_groups, std::move(rng));
    
    if (constraints) {
        sorter.set_constraints(std::move(constraints));
    }
    
    auto result = sorter.sort_names(names);
    
    if (!result) {
        std::cerr << "❌ Error: Failed to create balanced groups.\n";
        return 1;
    }
    
    // Determine output file
    std::string output_file = args.output_file.value_or("groups.txt");
    
    // Save results to file
    std::cout << "💾 Saving results to: " << output_file << "\n";
    
    if (!io_utils::save_groups_to_file(output_file, *result)) {
        std::cerr << "❌ Error: Failed to write output file.\n";
        return 1;
    }
    
    // Also print summary to console
    std::cout << "\n" << "═" << std::string(50, '═') << "\n";
    std::cout << "GROUPING SUMMARY\n";
    std::cout << "═" << std::string(50, '═') << "\n\n";
    
    int total_allocated = 0;
    for (size_t i = 0; i < result->groups.size(); ++i) {
        const auto& group = result->groups[i];
        std::cout << "Group " << (i + 1) << ": " << group.size() << " members\n";
        total_allocated += group.size();
        
        for (const auto& name : group) {
            std::cout << "  - " << name << "\n";
        }
    }
    
    std::cout << "\nTotal allocated: " << total_allocated << "/" << names.size() << " members\n";
    
    if (total_allocated != static_cast<int>(names.size())) {
        std::cerr << "⚠️  Warning: Not all names were assigned!\n";
    }
    
    std::cout << "\n✓ Done! Results saved to " << output_file << "\n";
    
    return 0;
}
