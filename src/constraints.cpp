#include "constraints.h"
#include <algorithm>

// Normalizes the pair so we always store ("Alice", "Bob") even if input was "Bob", "Alice"
std::pair<std::string, std::string> Constraints::normalize_pair(const std::string& a, const std::string& b) const {
    if (a < b) {
        return {a, b};
    }
    return {b, a};
}

void Constraints::add_separation(const std::string& name1, const std::string& name2) {
    if (name1 == name2) {
        // No need to separate someone from themselves
        return;
    }
    m_forbidden_pairs.insert(normalize_pair(name1, name2));
}

bool Constraints::must_be_separated(const std::string& name1, const std::string& name2) const {
    if (name1 == name2) return false;
    
    auto key = normalize_pair(name1, name2);
    return m_forbidden_pairs.find(key) != m_forbidden_pairs.end();
}
