#include <stack>
#include "DelaunayTriangulation.h"

#define next(i) (i + 1) % 3
#define prev(i) (i + 2) % 3

DelaunayTriangulation::DelaunayTriangulation(DelaunayTree &&tree) {
  auto node = tree.head;
  for (; !node->children.empty(); node = node->children[0]);
  std::stack<std::shared_ptr<DelaunayTree::Node> > s;
  s.push(node);
  node->ind = -1;

  while (!s.empty()) {
    node = s.top();
    s.pop();
    bool interior = true;
    for (int i = 0; i < 3; ++i) {
      if (node->points[i].index < 0) {
        interior = false;
        break;
      }
    }
    if (interior) {
      node->ind = nodes.size();
      nodes.push_back(node);
    }
    for (auto &neighbour : node->neighbours) {
      if (!neighbour.expired() && neighbour.lock()->ind == -2) {
        s.push(neighbour.lock());
        neighbour.lock()->ind = -1;
      }
    }
  }

  tree.head = nullptr;
}

std::vector<std::pair<int, int> > DelaunayTriangulation::get_edges() const {
  std::vector<std::pair<int, int> > edges;
  std::stack<std::shared_ptr<DelaunayTree::Node> > s;
  std::vector<bool> visited(nodes.size(), false);

  for (const auto &node : nodes) {
    visited[node->ind] = true;

    for (int i = 0; i < 3; ++i) {
      if (node->neighbours[i].expired() || !visited[node->neighbours[i].lock()->ind]) {
        int p1 = node->points[i].index;
        int p2 = node->points[next(i)].index;
        edges.emplace_back(p1, p2);
      }
    }
  }

  return edges;
}

bool DelaunayTriangulation::is_correct(const std::vector<std::pair<int, int> > &points) {
  int N = points.size();
  auto edges = get_edges();

  for (int i = 0; i < N - 1; ++i) {
    for (int j = i + 1; j < N; ++j) {
      bool can_be_added = true;
      for (const auto &edge : edges) {
        int r = edge.first;
        int s = edge.second;
        if (intersect(static_cast<Point>(points[i]), static_cast<Point>(points[j]), static_cast <Point> (points[r]),
                      static_cast<Point>(points[s]))) {
          can_be_added = false;
          break;
        }
      }
      if (can_be_added) {
        std::cout << i << ' ' << j << std::endl;
        return false;
      }
    }
  }

  for (const auto &node : nodes) {
    for (int i = 0; i < 3; ++i) {
      if (!node->neighbours[i].expired()) {
        auto neighbour = node->neighbours[i].lock();
        int j = 0;
        for (; neighbour->neighbours[next(j)].lock() != node; ++j);
        if (is_point_in_circle(node->points[prev(i)], node->points[i], node->points[next(i)], neighbour->points[j])) {
          std::cout << node->points[prev(i)].index << ' ' << node->points[i].index << ' ' << node->points[next(i)].index
                    << ' ' << neighbour->points[j].index << std::endl;
          return false;
        }
      }
    }
  }
  return true;
}