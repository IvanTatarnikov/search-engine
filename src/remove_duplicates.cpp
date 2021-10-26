#include <vector>

#include "remove_duplicates.h"

using namespace std;

void RemoveDuplicates(SearchServer& search_server) {
    set<int> duplicates;

    for (const int document_id : search_server) {
        if (duplicates.count(document_id)) {
            continue;
        }

        for (const int inner_document_id : search_server) {
            if (document_id >= inner_document_id || duplicates.count(inner_document_id)) {
                continue;
            }
            
            const auto& words = search_server.GetWordFrequencies(document_id);
            const auto& inner_words = search_server.GetWordFrequencies(inner_document_id);
            
            if (words.size() != inner_words.size()) {
                continue;
            }

            bool equal = true;
            for (const auto& [word, _] : words) {
                if (inner_words.count(word) == 0) {
                    equal = false;
                    break;
                }
            }
            if (equal) {
                duplicates.insert(inner_document_id);
            }
        }    
    }

    for (const int duplicate_document_id : duplicates) {
        cout << "Found duplicate document id "s << duplicate_document_id << endl;
        search_server.RemoveDocument(duplicate_document_id);
    }
}