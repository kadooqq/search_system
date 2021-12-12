#include "search_server.h"
#include "iterator_range.h"
#include "parse.h"

#include <algorithm>
#include <sstream>
#include <mutex>

using namespace std;

vector<string_view> SplitIntoWords(string_view line) {
    vector<string_view> result;
    while (!line.empty()) {
        line = Strip(line);
        size_t pos = line.find(' ');
        result.push_back(line.substr(0, pos));
        line.remove_prefix(pos != line.npos ? pos + 1 : line.size());
    }

    return result;
}

SearchServer::SearchServer(istream &document_input) {
    UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream &document_input) {
    InvertedIndex new_index = move(async([&document_input] {
        InvertedIndex new_index;
        for (string current_document; getline(document_input, current_document);) {
            new_index.Add(move(current_document));
        }
        return new_index;
    }).get());

    swap(index.GetAccess().ref_to_value, new_index);
}

void SearchServer::AddQueriesStreamSingleThread(
        istream &query_input, ostream &search_results_output
) {
    for (string query; getline(query_input, query);) {
        const auto words = SplitIntoWords(query);
        vector<pair<size_t, size_t>> docid_count(index.GetAccess().ref_to_value.GetNumOfDocs());
        size_t doc_cnt = 0;
        size_t last_id = 0;
        {
            const auto &cur_index_state = index.GetAccess();

            for (const auto word: words) {
                for (const auto[docid, hits_cnt]: cur_index_state.ref_to_value.Lookup(word)) {
                    if (docid > last_id) last_id = docid;
                    if (docid_count[docid].second == 0) doc_cnt++;
                    docid_count[docid].first = docid;
                    docid_count[docid].second += hits_cnt;
                }
            }
        }

        partial_sort(
                begin(docid_count),
                begin(docid_count) + min(5, static_cast<int>(doc_cnt)),
                begin(docid_count) + last_id + 1,
                [](pair<size_t, size_t>& lhs, pair<size_t, size_t>& rhs) {
                    int64_t lhs_docid = lhs.first;
                    auto lhs_hit_count = lhs.second;
                    int64_t rhs_docid = rhs.first;
                    auto rhs_hit_count = rhs.second;
                    return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
                }
        );

        search_results_output << query << ':';
        for (auto& [docid, hitcount]: Head(docid_count, min(5, static_cast<int>(doc_cnt)))) {
            search_results_output << " {" << "docid: " << docid << ","
                                  << " hitcount: " << hitcount << '}';
        }
        search_results_output << endl;
    }
}

void SearchServer::AddQueriesStream(
        istream &query_input, ostream &search_results_output) {

    queries_futures.push_back(
            async([this](istream &query_input, ostream &search_results_output) {
                      return AddQueriesStreamSingleThread(query_input, search_results_output);
                  },
                  ref(query_input), ref(search_results_output)));

}

void InvertedIndex::Add(const string &document) {
    docs.push_back(document);

    const size_t docid = docs.size() - 1;
    for (const auto word: SplitIntoWords(document)) {
        auto &v = index[string(word)];
        if (v.empty() || v.back().first != docid) v.emplace_back(docid, 0);
        v.back().second++;
    }
}

const vector<pair<size_t, size_t>> &InvertedIndex::Lookup(const string_view word) const {
    if (auto it = index.find(string(word)); it != index.end()) {
        return it->second;
    } else {
        return empty;
    }
}

