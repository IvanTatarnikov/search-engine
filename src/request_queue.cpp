#include "request_queue.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server)
    : search_server_(search_server) {
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    const auto documents_ = search_server_.FindTopDocuments(raw_query, status);
    UpdateRequests(documents_);
    return documents_;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    const auto documents_ = search_server_.FindTopDocuments(raw_query);
    UpdateRequests(documents_);
    return documents_;
}

int RequestQueue::GetNoResultRequests() const {
    return not_found_count_;
}

void RequestQueue::UpdateRequests(const vector<Document>&  documents) {
    requests_.push_back({documents});
    if (documents.empty()) {
        ++not_found_count_;
    }
    if (requests_.size() > sec_in_day_) {
        if (requests_.front().documents.empty()) {
            --not_found_count_;
        }
        requests_.pop_back();
    }   
}