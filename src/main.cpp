//
#include <iostream>
#include <filesystem>

#include "../utility/bpt/bpt.h"
#include "../utility/STLite/String.h"

using namespace std;
using namespace bubble;

void ClearFiles() {
  filesystem::remove("node_data");
  filesystem::remove("node_trash");
  filesystem::remove("leaf_data");
  filesystem::remove("leaf_trash");
}

int main() {
  ClearFiles();
  freopen("input.txt", "r", stdin);
  fstream fs("output.txt", ios::out);
  int n, i, value;
  char op[10];
  String<64> index;
  auto *tr = new BPlusTree<String<64>, int>("node_data", "node_trash", "leaf_data", "leaf_trash");
  cin >> n;
  for (i = 0; i < n; ++i) {
    if (i % (n / 100) == 0) {
      cout << i / (n / 100) << "%" << endl;
    }
    cin >> op;
    if (strcmp(op, "insert") == 0) {
      cin >> index >> value;
      tr->Insert(Pair<String<64>, int>(index, value));
    }
    else if (strcmp(op, "delete") == 0) {
      cin >> index >> value;
      tr->Erase(Pair<String<64>, int>(index, value));
    }
    else if (strcmp(op, "find") == 0) {
      fs << i + 2 << "|\t";
      cin >> index;
      Vector<int> find_ans(tr->Find(index));
      if (find_ans.empty()) {
        fs << "null\n";
      }
      else {
        for (auto &j: find_ans) {
          fs << j << " ";
        }
        fs << "\n";
      }
    }
  }
  delete tr;
  fs.close();
  return 0;
}