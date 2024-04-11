#include <iostream>
#include <cassert>
#include <limits>
#include <memory>
#include <vector>
#include <queue>
#include <stack>

using namespace std;

enum class Partity : uint8_t {
  UNKNOWN = 0, A, B
};

struct Graph {
  std::vector<Partity> node;
  std::vector<std::vector<unsigned>> edge_weight;
};

struct Result
{
  unsigned total;
  std::vector<Partity> total_weights;
};



//TODO
void unite(vector<Partity>& nodes, vector<vector<unsigned>>& edges) {

};


void bfs(const vector<Partity>& nodes, const vector<vector<unsigned>>& edges, vector<unsigned>& level, unsigned& source) {

  //nodes accessed
  fill(level.begin(), level.end(), 0);

  queue<unsigned> q;

  q.push(source);

  while (!q.empty()) {
    
    for (unsigned i = 0; i < nodes.size(); i++) {
      //skip edges between identical nodes or heading to sourceing node
      if (q.front() == i || source == i) continue;

      //mark and add next available node
      if (level[i] == 0 && edges[q.front()][i] > 0) {
        level[i] = level[q.front()]+1;
        q.push(i);
      }
    }

    q.pop();
  }

  cout << "level: ";
  for (auto it = level.begin(); it != level.end(); it++) {
    cout << *it << " ";
  }
  cout << endl;
}

void dfs(vector<Partity>& nodes, vector<vector<unsigned>>& edges, vector<unsigned>& level, unsigned& source, unsigned& sink, Result& res) {

  stack<unsigned> s;
  vector<unsigned> path;
  unsigned node;
  unsigned max_flow;

  s.push(source);

  while (!s.empty()) {
    node = s.top();
    path.push_back(node);


    cout << node << " " << sink << endl;
    usleep(50000);

    //found a path from source to end
    if (node == sink) {

      max_flow = numeric_limits<unsigned>::max();
    
      //iterate path to find max flow
      for (unsigned i = 0; i < path.size()-1; i++) {
        max_flow = min(max_flow, edges[path[i]][path[i+1]]);
        cout << edges[path[i]][path[i+1]] << endl;
      }

      cout << "max flow: " << max_flow << endl;
      res.total -= max_flow;


      //fill the edge flow to both directions
      for (unsigned i = 0; i < path.size()-1; i++) {
        edges[path[i]][path[i+1]] -= max_flow;
        edges[path[i+1]][path[i]] += max_flow; 
      }

      while (s.top() != source) s.pop();

      path.clear();

      continue;
    }

    //push the next accessible nodes
    for (unsigned i = 0; i < nodes.size(); i++) {
      if (node == i || source == i) continue;

      if (level[node]+1 == level[i] && edges[node][i] > 0) {
        s.push(i);
      }
    }

    //deadend
    if (node == s.top()) {
      cout << "Deadend" << endl;

      //update node level to zero so that it won't be used again
      level[node] = 0;
      s.pop();
      path.pop_back();
    }


  }

  return;
}

void dinitz(vector<Partity>& nodes, vector<vector<unsigned>>& edges, Result& res, unsigned& source, unsigned& sink) {
  vector<vector<unsigned>> flow = edges;
  vector<vector<unsigned>> resi = edges;
  vector<vector<unsigned>> capa = edges;


  vector<unsigned> level(nodes.size());


  while (true) {
    //mark when each accessible node was opened
    bfs(nodes, edges, level, source);

    //no more paths from source to sink
    if (level[sink] == 0) break;

    //find a path and modify flow network
    dfs(nodes, edges, level, source, sink, res);
  }

  for (unsigned i = 0; i < nodes.size(); i++) {
    if (level[i] == 0 && i != source) {
      cout << "B" << endl;
      res.total_weights.push_back(Partity::B);
    }
    else {
      cout << "A" << endl;
      res.total_weights.push_back(Partity::A);
    }
  }
}







Result split_graph(const Graph& Graph) {

  vector<Partity> nodes = Graph.node;
  vector<vector<unsigned>> edges = Graph.edge_weight;
  unsigned source, sink;
  Result res;

  unite(nodes, edges);

  //save total weight of edges
  res.total = 0;
  for (unsigned i = 0; i < nodes.size(); i++) {
    for (unsigned j = 0; j < nodes.size(); j++) {
      if (i == j) continue;

      res.total += edges[i][j];
    }
  }
  res.total /= 2;


  //set source sink nodes
  for (unsigned i = 0; i < nodes.size(); i++) {
    if (nodes[i] == Partity::A) source = i;
    if (nodes[i] == Partity::B) sink = i;
  }

  dinitz(nodes, edges, res, source, sink);

  cout << "Result: " << res.total << endl;

  return res;
}





using enum Partity;
const std::vector<std::pair<unsigned, Graph>> test_data = {
  /*
  { 92, Graph{ { {}, {}, B, {}, A, {}, }, {
    { 0, 8, 1, 9, 9, 6, },
    { 8, 0, 14, 11, 14, 11, },
    { 1, 14, 0, 9, 7, 3, },
    { 9, 11, 9, 0, 12, 7, },
    { 9, 14, 7, 12, 0, 5, },
    { 6, 11, 3, 7, 5, 0, },
  } } },
  { 122, Graph{ { B, B, A, {}, B, B, {}, }, {
    { 0, 3, 3, 5, 9, 4, 7, },
    { 3, 0, 5, 15, 9, 14, 14, },
    { 3, 5, 0, 11, 12, 2, 6, },
    { 5, 15, 11, 0, 6, 3, 13, },
    { 9, 9, 12, 6, 0, 7, 12, },
    { 4, 14, 2, 3, 7, 0, 1, },
    { 7, 14, 6, 13, 12, 1, 0, },
  } } },
  { 133, Graph{ { A, {}, {}, B, B, {}, {}, A, }, {
    { 0, 13, 7, 10, 9, 3, 15, 4, },
    { 13, 0, 5, 5, 11, 6, 5, 12, },
    { 7, 5, 0, 15, 5, 8, 15, 15, },
    { 10, 5, 15, 0, 9, 15, 6, 9, },
    { 9, 11, 5, 9, 0, 8, 5, 8, },
    { 3, 6, 8, 15, 8, 0, 1, 5, },
    { 15, 5, 15, 6, 5, 1, 0, 10, },
    { 4, 12, 15, 9, 8, 5, 10, 0, },
  } } },
  { 219, Graph{ { {}, {}, B, A, {}, {}, {}, {}, {}, }, {
    { 0, 7, 2, 7, 5, 9, 8, 10, 4, },
    { 7, 0, 3, 15, 4, 4, 14, 1, 8, },
    { 2, 3, 0, 6, 13, 4, 6, 2, 10, },
    { 7, 15, 6, 0, 11, 10, 3, 4, 6, },
    { 5, 4, 13, 11, 0, 1, 14, 14, 15, },
    { 9, 4, 4, 10, 1, 0, 11, 11, 5, },
    { 8, 14, 6, 3, 14, 11, 0, 5, 3, },
    { 10, 1, 2, 4, 14, 11, 5, 0, 10, },
    { 4, 8, 10, 6, 15, 5, 3, 10, 0, },
  } } },
  */
  { 311, Graph{ { A, {}, {}, {}, B, {}, {}, {}, {}, {}, }, {
    { 0, 4, 4, 10, 8, 8, 13, 2, 9, 2, },
    { 4, 0, 3, 5, 9, 3, 14, 15, 13, 12, },
    { 4, 3, 0, 7, 4, 14, 13, 9, 6, 5, },
    { 10, 5, 7, 0, 1, 10, 4, 10, 13, 12, },
    { 8, 9, 4, 1, 0, 12, 10, 4, 13, 2, },
    { 8, 3, 14, 10, 12, 0, 13, 8, 7, 13, },
    { 13, 14, 13, 4, 10, 13, 0, 2, 3, 15, },
    { 2, 15, 9, 10, 4, 8, 2, 0, 11, 3, },
    { 9, 13, 6, 13, 13, 7, 3, 11, 0, 13, },
    { 2, 12, 5, 12, 2, 13, 15, 3, 13, 0, },
  } } }
  /*
  ,
  { 327, Graph{ { {}, {}, {}, {}, B, {}, {}, {}, {}, A, B, }, {
    { 0, 3, 2, 8, 5, 15, 5, 1, 14, 8, 8, },
    { 3, 0, 8, 12, 9, 2, 8, 12, 11, 6, 6, },
    { 2, 8, 0, 3, 3, 1, 3, 6, 5, 6, 11, },
    { 8, 12, 3, 0, 8, 1, 8, 4, 10, 3, 2, },
    { 5, 9, 3, 8, 0, 8, 8, 15, 3, 13, 14, },
    { 15, 2, 1, 1, 8, 0, 8, 8, 6, 6, 10, },
    { 5, 8, 3, 8, 8, 8, 0, 10, 12, 15, 2, },
    { 1, 12, 6, 4, 15, 8, 10, 0, 14, 1, 9, },
    { 14, 11, 5, 10, 3, 6, 12, 14, 0, 9, 6, },
    { 8, 6, 6, 3, 13, 6, 15, 1, 9, 0, 9, },
    { 8, 6, 11, 2, 14, 10, 2, 9, 6, 9, 0, },
  } } },
  { 447, Graph{ { {}, {}, {}, {}, {}, {}, {}, {}, A, {}, B, {}, }, {
    { 0, 1, 12, 9, 9, 4, 11, 14, 8, 4, 9, 7, },
    { 1, 0, 5, 4, 9, 9, 5, 8, 9, 14, 12, 12, },
    { 12, 5, 0, 8, 12, 13, 14, 2, 1, 2, 4, 13, },
    { 9, 4, 8, 0, 6, 5, 1, 6, 5, 6, 15, 8, },
    { 9, 9, 12, 6, 0, 14, 11, 3, 9, 2, 13, 11, },
    { 4, 9, 13, 5, 14, 0, 11, 6, 6, 9, 8, 1, },
    { 11, 5, 14, 1, 11, 11, 0, 7, 13, 13, 9, 13, },
    { 14, 8, 2, 6, 3, 6, 7, 0, 4, 5, 3, 14, },
    { 8, 9, 1, 5, 9, 6, 13, 4, 0, 2, 10, 2, },
    { 4, 14, 2, 6, 2, 9, 13, 5, 2, 0, 1, 14, },
    { 9, 12, 4, 15, 13, 8, 9, 3, 10, 1, 0, 6, },
    { 7, 12, 13, 8, 11, 1, 13, 14, 2, 14, 6, 0, },
  } } },
  */
};



void verify_result(unsigned total, const Graph& Graph, const Result& res) {
  using enum Partity;

  unsigned total_2 = res.total;
  vector<Partity> dest = res.total_weights;

  assert(dest.size() == Graph.node.size());

  for (unsigned i = 0; i < dest.size(); i++) {
    assert(dest[i] == B || dest[i] == A);
    if (Graph.node[i] != Partity::UNKNOWN) assert(dest[i] == Graph.node[i]);
  }

  unsigned total_weights = 0;
  for (unsigned i = 0; i < dest.size(); i++)
    for (unsigned j = 0; j < i; j++)
      if (dest[i] == dest[j]) total_weights += Graph.edge_weight[i][j];

  assert(total == total_2);
  assert(total_weights == total_2);
}

int main() {

  for (auto it = test_data.begin(); it != test_data.end(); it++) {
    unsigned total = (*it).first;
    Graph graph = (*it).second;

    verify_result(total, graph, split_graph(graph));
  }

  return 0;
}
