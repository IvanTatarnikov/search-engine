#pragma once 

#include <vector>
#include <string>
#include <deque>

#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;
private:
    struct QueryResult {
        std::vector<Document> documents;
    };
    std::deque<QueryResult> requests_;
    const static int sec_in_day_ = 1440;
    int not_found_count_ = 0;
    const SearchServer& search_server_;
    
    void UpdateRequests(const std::vector<Document>&  documents);
};

// сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    const auto documents_ = search_server_.FindTopDocuments(raw_query, document_predicate);
    UpdateRequests(documents_);
    return documents_;
}