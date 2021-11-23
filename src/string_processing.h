#pragma once 

#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <unordered_set>

std::vector<std::string> SplitIntoWords(const std::string_view text);
std::vector<std::string_view> SplitIntoWordsView(const std::string_view text);

template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings);


template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string> non_empty_strings;
    for (const auto& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str.data());
        }
    }
    return non_empty_strings;
}
