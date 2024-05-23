// generator
#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <cstdio>
#include <fstream>
#include <random>
#include <set>

using namespace std;

std::string GenerateRandomString(int len) {
  const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, characters.length() - 1);
  std::string randomString;
  for (int i = 0; i < len; ++i) {
    char randomChar = characters[dis(gen)];
    randomString += randomChar;
  }
  return randomString;
}

int GenerateRandomNumber(int n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, n - 1);
  return dis(gen);
}

int main() {
  fstream fs("input.txt", ios::out);
  vector<pair<string, int>> str_list;
  int n = 10000;
  fs << n << "\n";
  for (int i = 0; i < n; i++) {
    int op = GenerateRandomNumber(10);
    if (i < n * 0.5) {
      fs << "insert ";
      string str = GenerateRandomString(3);
      int Int = GenerateRandomNumber(100);
      int rnd = GenerateRandomNumber(5);
      if (!str_list.empty() && rnd < 2) {
        auto pair = str_list[GenerateRandomNumber(str_list.size())];
        str = pair.first;
        Int = pair.second;
      }
      else if (!str_list.empty() && rnd < 3) {
        auto pair = str_list[GenerateRandomNumber(str_list.size())];
        str = pair.first;
      }
      str_list.push_back(pair(str, Int));
      fs << str << " ";
      fs << Int << "\n";
    }
    else if (op <= 8) {
      fs << "delete ";
      if (!str_list.empty() && GenerateRandomNumber(5) < 4) {
        auto it = str_list.begin() + GenerateRandomNumber(str_list.size());
        fs << it->first << " " << it->second << "\n";
        str_list.erase(it);
      }
      else {
        fs << GenerateRandomString(3) << " " << GenerateRandomNumber(100) << "\n";
      }
    }
    else {
      fs << "find ";
      if (!str_list.empty() && GenerateRandomNumber(5) < 4) {
        auto pair = str_list[GenerateRandomNumber(str_list.size())];
        fs << pair.first << "\n";
      }
      else {
        fs << GenerateRandomString(3) << "\n";
      }
    }
  }
  fs.close();
  freopen("input.txt", "r", stdin);
  freopen("answer.txt", "w", stdout);
  auto Cmp = [](const pair<string, int> &lhs, const pair<string, int> &rhs) -> bool {
      if (lhs.first != rhs.first) {
        return lhs.first < rhs.first;
      }
      return lhs.second < rhs.second;
  };
  set<pair<string, int>, decltype(Cmp)> ms(Cmp);
  string op;
  cin >> n;
  for (int i = 0; i < n; i++) {
    cin >> op;
    string index;
    int value;
    if (op == "insert") {
      cin >> index >> value;
      ms.insert(make_pair(index, value));
    }
    else if (op == "delete") {
      cin >> index >> value;
      ms.erase(make_pair(index, value));
    }
    else {
      cout << i + 2 << "|\t";
      cin >> index;
      std::vector<int> ans;
      for (auto it = ms.begin(); it != ms.end(); ++it) {
        if (it->first == index) {
          ans.push_back(it->second);
        }
      }
      if (ans.empty()) {
        cout << "null\n";
      }
      else {
        for (int &an: ans) {
          cout << an << " ";
        }
        cout << "\n";
      }
    }
  }
  return 0;
}