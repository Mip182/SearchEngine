#pragma once

#include <string_view>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <string>
#include <iostream>

const long double EPS = 1e-9;
const char DIFFERENCE_BETWEEN_LOW_AND_BIG_LETTERS = 'a' - 'A';

class SearchEngine {
public:
    std::vector<std::string_view> strings_of_text;

    bool IsWordsEqualWithNoRegisterMatters(const std::string_view& a,
                                           const std::string_view& b) const;

    long double ComputeTermFrequency(const std::string_view& query_word,
                                     const std::string_view& document) const;

    long double ComputeInverseDocumentFrequency(const std::string_view& query_word) const;

    long double CompeteTFIDFForWordsFromQuery(
        const std::vector<std::string_view>& unique_words_from_query,
        const std::string_view& document,
        const std::vector<long double>& idfs_of_unique_words_of_query) const;

    std::vector<std::string_view> SplitQueryIntoWords(const std::string_view& query) const;

    std::vector<std::string_view> GetUniqueWords(const std::vector<std::string_view>& words) const;

    void BuildIndex(std::string_view text);

    std::vector<std::string_view> Search(std::string_view query, size_t results_count) const;
};
