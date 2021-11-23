#include <algorithm>
#include <numeric>
#include <execution>

#include "process_queries.h"

using namespace std;

vector<vector<Document>> ProcessQueries(const SearchServer& search_server, const vector<string>& queries) {
    vector<vector<Document>> result(queries.size());
    transform(
        execution::par,
        queries.begin(), queries.end(),
        result.begin(),
        [&search_server](const string& raw_query) {
            return search_server.FindTopDocuments(raw_query);
        }
    );
    return result;
}

vector<Document> ProcessQueriesJoined(const SearchServer& search_server, const vector<string>& queries) {
    vector<Document> documents = transform_reduce(
        execution::par,
        queries.begin(), queries.end(),
        vector<Document>(),
        [](vector<Document> acc, const vector<Document>& documents) {
            copy(documents.begin(), documents.end(), back_inserter<vector<Document>>(acc));
            return acc;
        },
        [&search_server](const string& raw_query) {
            return search_server.FindTopDocuments(raw_query);
        }
    );
    return documents;
}