#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            words.push_back(word);
            word = "";
        } else {
            word += c;
        }
    }
    words.push_back(word);

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        for (const string& word : words) {
            ++word_to_document_freqs_[word][document_id];
        }
        for (const string& word : words) {
            if (word_to_document_freqs_[word][document_id] >= 1.0) {
                word_to_document_freqs_[word][document_id] /= words.size();
            }
        }
        ++document_count_;
    }

    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    vector<Document> FindTopDocuments(const string& query) const {
        auto matched_documents = FindAllDocuments(query);
        sort(
            matched_documents.begin(), 
            matched_documents.end(), 
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            }
        );

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }    

    void Print() const {
        for (const auto& [word, doc_fr] : word_to_document_freqs_) {
            cout << word << endl;
            for (auto [document_id, relevance] : doc_fr) {
                cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s << endl;
            }
        }
    }

private:
    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;
    int document_count_ = 0;

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (stop_words_.count(word) == 0) {
                words.push_back(word);
            }
        }
        return words;
    }

    struct Query {
        vector<string> plus_words;
        vector<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        const vector<string> query_words = SplitIntoWordsNoStop(text);
        Query query;
        for (const string& word : query_words) {
            if (word.empty()) {
                continue;
            }
            if ('-' == word[0]) {
                query.minus_words.push_back(word.substr(1));
            } else {
                query.plus_words.push_back(word);
            }
        }
        return query;
    }
    
    vector<Document> FindAllDocuments(const string& query_text) const {
        map<int, double> document_to_relevance;

        const Query query = ParseQuery(query_text);
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const auto& document_freqs = word_to_document_freqs_.at(word);
            for (const auto [document_id, term_frequancy] : document_freqs) {
                const double inverse_document_frequency = log(static_cast<double>(document_count_) / document_freqs.size());
                document_to_relevance[document_id] += (inverse_document_frequency * term_frequancy);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> found_documents;
        for (auto [document_id, relevance] : document_to_relevance) {
            found_documents.push_back({document_id, relevance});
        }
        return found_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer server;
    server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        server.AddDocument(document_id, ReadLine());
    }

    return server;
}

int main() {
    const SearchServer server = CreateSearchServer();

    const string query = ReadLine();
    for (auto [document_id, relevance] : server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s << endl;
    }
}