#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<int> bag(map<int, vector<int>>& bags, int i) {
  auto it = bags.begin();
  while (i) {
    it++;
    i--;
  }
  auto ret_val = it->second;
  bags.erase(it);
  return ret_val;
}

void join(vector<vector<int>>& graph, vector<int>& bag_l, vector<int>& bag_r) {
  for (int i = 0; i < bag_l.size(); i++) {
    for (int j = 0; j < bag_r.size(); j++) {
      graph[bag_l[i]].push_back(bag_r[j]);
      graph[bag_r[j]].push_back(bag_l[i]);
    }
  }
}

string print_vec(vector<int>& vec) {
  stringstream s;
  s << "{";
  for (auto& v : vec) {
    s << " " << v;
  }
  s << "}";
  return s.str();
}

int main() {
  iostream::sync_with_stdio(false);
  int N = 0, seed = 0, bag_num = 0, num_edges = 0;
  cin >> N >> seed;
  std::srand(seed);
  map<int, vector<int>> bags;
  vector<vector<int>> graph(N);
  for (int i = 0; i < N; i++) {
    bags[i].push_back(i);
    bag_num++;
  }

  while (bags.size() > 1) {
    int x = std::rand() % bags.size();
    vector<int> left = bag(bags, x);
    x = std::rand() % bags.size();
    vector<int> right = bag(bags, x);
    bag_num++;
    int op = std::rand() % 2;
    if (op) {
      // cerr << "joining " << print_vec(left) << " and " << print_vec(right)
      //      << endl;
      join(graph, left, right);
      // cerr << "joined" << endl;
      num_edges += left.size() * right.size();
    } else {
      // cerr << "union " << print_vec(left) << " and " << print_vec(right)
      //      << endl;
    }
    left.insert(left.end(), right.begin(), right.end());
    bags[bag_num] = left;
  }
  cout << "p tww " << N << " " << num_edges << endl;
  for (int i = 0; i < N; i++) {
    for (auto& v : graph[i]) {
      if (v > i) {
        cout << i+1 << " " << v+1 << endl;
      }
    }
  }
}
