#include "search.h"

bool SearchEngine::IsWordsEqualWithNoRegisterMatters(const std::string_view& a,
                                                     const std::string_view& b) const {

    if (a.size() != b.size()) {
        return false;
    }
    for (size_t index = 0; index < a.size(); ++index) {
        char x = a[index], y = b[index];
        if (x > y) {
            std::swap(x, y);
        }
        if (x != y && y - x != DIFFERENCE_BETWEEN_LOW_AND_BIG_LETTERS) {
            return false;
        }
    }
    return true;
}

long double SearchEngine::ComputeTermFrequency(const std::string_view& query_word,
                                               const std::string_view& document) const {
    int number_of_query_word_in_document = 0;

    int number_of_words_in_document = 0;

    size_t begin_of_new_word = 0;
    while (begin_of_new_word < document.size()) {
        if (!(std::isalpha(document[begin_of_new_word]))) {
            ++begin_of_new_word;
            continue;
        }

        size_t end_of_new_word = begin_of_new_word;
        while (end_of_new_word < document.size() && std::isalpha(document[end_of_new_word])) {
            ++end_of_new_word;
        }

        std::string_view new_word(document.begin() + begin_of_new_word,
                                  end_of_new_word - begin_of_new_word);

        if (IsWordsEqualWithNoRegisterMatters(new_word, query_word)) {
            ++number_of_query_word_in_document;
        }

        ++number_of_words_in_document;

        begin_of_new_word = end_of_new_word + 1;
    }
    if (number_of_words_in_document == 0) {
        return 1;
    }
    return static_cast<long double>(number_of_query_word_in_document) / number_of_words_in_document;
}

long double SearchEngine::ComputeInverseDocumentFrequency(
    const std::string_view& query_word) const {
    int number_of_documents_with_query_string = 0;

    for (const auto& document : strings_of_text) {
        bool is_document_has_query_string = false;

        size_t begin_of_new_word = 0;
        while (begin_of_new_word < document.size()) {
            if (!(std::isalpha(document[begin_of_new_word]))) {
                ++begin_of_new_word;
                continue;
            }

            size_t end_of_new_word = begin_of_new_word;
            while (end_of_new_word < document.size() && std::isalpha(document[end_of_new_word])) {
                ++end_of_new_word;
            }

            std::string_view new_word(document.begin() + begin_of_new_word,
                                      end_of_new_word - begin_of_new_word);

            if (IsWordsEqualWithNoRegisterMatters(new_word, query_word)) {
                is_document_has_query_string = true;
            }

            begin_of_new_word = end_of_new_word + 1;
        }

        if (is_document_has_query_string) {
            ++number_of_documents_with_query_string;
        }
    }

    if (number_of_documents_with_query_string == 0) {
        return 0;
    }

    return logl(static_cast<long double>(strings_of_text.size()) /
                number_of_documents_with_query_string);
}

long double SearchEngine::CompeteTFIDFForWordsFromQuery(
    const std::vector<std::string_view>& unique_words_from_query, const std::string_view& document,
    const std::vector<long double>& idfs_of_unique_words_of_query) const {
    long double tfidf = 0;

    for (size_t index = 0; index < unique_words_from_query.size(); ++index) {
        tfidf += ComputeTermFrequency(unique_words_from_query[index], document) *
                 idfs_of_unique_words_of_query[index];
    }

    return tfidf;
}

std::vector<std::string_view> SearchEngine::SplitQueryIntoWords(
    const std::string_view& query) const {
    std::vector<std::string_view> words_of_query;

    size_t begin_of_new_word = 0;
    while (begin_of_new_word < query.size()) {
        if (!isalpha(query[begin_of_new_word])) {
            ++begin_of_new_word;
            continue;
        }

        size_t end_of_new_word = begin_of_new_word;
        while (end_of_new_word < query.size() && std::isalpha(query[end_of_new_word])) {
            end_of_new_word++;
        }

        std::string_view new_word(query.begin() + begin_of_new_word,
                                  end_of_new_word - begin_of_new_word);
        words_of_query.push_back(new_word);

        begin_of_new_word = end_of_new_word + 1;
    }

    return words_of_query;
}

std::vector<std::string_view> SearchEngine::GetUniqueWords(
    const std::vector<std::string_view>& words) const {
    std::vector<std::string_view> unique_words;

    for (auto& word : words) {
        bool is_word_already_in_vector = false;

        for (auto& word_from_vector : unique_words) {
            if (IsWordsEqualWithNoRegisterMatters(word_from_vector, word)) {
                is_word_already_in_vector = true;
            }
        }

        if (!is_word_already_in_vector) {
            unique_words.push_back(word);
        }
    }

    return unique_words;
}

void SearchEngine::BuildIndex(std::string_view text) {
    strings_of_text.clear();

    size_t begin_of_new_string = 0;
    while (begin_of_new_string < text.size()) {
        if (text[begin_of_new_string] == '\n') {
            ++begin_of_new_string;
            continue;
        }

        size_t end_of_new_string = begin_of_new_string;
        while (end_of_new_string < text.size() && text[end_of_new_string] != '\n') {
            ++end_of_new_string;
        }

        std::string_view new_string(text.begin() + begin_of_new_string,
                                    end_of_new_string - begin_of_new_string);
        strings_of_text.push_back(new_string);

        begin_of_new_string = end_of_new_string + 1;
    }
}

std::vector<std::string_view> SearchEngine::Search(std::string_view query,
                                                   size_t results_count) const {
    std::vector<std::string_view> words_of_query = SplitQueryIntoWords(query);

    std::vector<std::string_view> unique_words_of_query = GetUniqueWords(words_of_query);

    std::vector<long double> idfs_of_unique_words_of_query(unique_words_of_query.size());

    for (size_t index = 0; index < unique_words_of_query.size(); ++index) {
        idfs_of_unique_words_of_query[index] =
            ComputeInverseDocumentFrequency(unique_words_of_query[index]);
    }

    std::vector<size_t> positions_of_documents_in_sorted_by_tfidf_vector(strings_of_text.size());

    std::iota(positions_of_documents_in_sorted_by_tfidf_vector.begin(),
              positions_of_documents_in_sorted_by_tfidf_vector.end(), 0);

    std::sort(positions_of_documents_in_sorted_by_tfidf_vector.begin(),
              positions_of_documents_in_sorted_by_tfidf_vector.end(), [&](int a, int b) {
                  long double tfidf_a = CompeteTFIDFForWordsFromQuery(
                      unique_words_of_query, strings_of_text[a], idfs_of_unique_words_of_query);

                  long double tfidf_b = CompeteTFIDFForWordsFromQuery(
                      unique_words_of_query, strings_of_text[b], idfs_of_unique_words_of_query);

                  if (std::abs(tfidf_a - tfidf_b) > EPS) {
                      return tfidf_a > tfidf_b;
                  }
                  return a < b;
              });

    std::vector<std::string_view> relevant_strings;

    for (size_t index = 0; index < std::min(results_count, strings_of_text.size()); ++index) {
        long double tfidf_for_next_sentence = CompeteTFIDFForWordsFromQuery(
            unique_words_of_query,
            strings_of_text[positions_of_documents_in_sorted_by_tfidf_vector[index]],
            idfs_of_unique_words_of_query);

        if (tfidf_for_next_sentence == 0) {
            continue;
        }

        relevant_strings.push_back(
            strings_of_text[positions_of_documents_in_sorted_by_tfidf_vector[index]]);
    }

    return relevant_strings;
}
