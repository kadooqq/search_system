#pragma once

#include "synchronized.h"

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <utility>
#include <map>
#include <unordered_map>
#include <string>
#include <future>

using namespace std;

class InvertedIndex {
public:
  void Add(const string& document);
  const vector<pair<size_t, size_t>>& Lookup(string_view word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

  size_t GetNumOfDocs() { return docs.size(); }

private:
  unordered_map<string, vector<pair<size_t, size_t>>> index;
  vector<pair<size_t, size_t>> empty;
  vector<string> docs;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);
  void AddQueriesStreamSingleThread(istream& query_input, ostream& search_results_output);

private:
    Synchronized<InvertedIndex> index;
    vector<future<void>> queries_futures;
};
