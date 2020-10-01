#include <iostream>
#include <utility>
#include <vector>
#include <random>
#include <unordered_set>
#include "GeometricalObjects.h"
#include "DelaunayTree.h"
#include "DelaunayTriangulation.h"

DelaunayTriangulation build_delaunay_triangulation(const std::vector<std::pair<int, int> > &coords,
                                                   bool shuffle = false) {
  std::vector<Point> points(coords.size());
  for (int i = 0; i < coords.size(); ++i) {
    points[i] = {coords[i].first, coords[i].second, i};
  }

  if (shuffle) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(points.begin(), points.end(), gen);
  }

  int M = -1;
  for (auto &point : points) {
    M = std::max(M, std::abs(point.x));
    M = std::max(M, std::abs(point.y));
  }
  ++M;
  Point p1 = {3 * M, 0, -1};
  Point p2 = {0, 3 * M, -2};
  Point p3 = {-3 * M, -3 * M, -3};
  DelaunayTree tree(p1, p2, p3);
  for (auto &point : points) {
    tree.add_point(point);
  }
  DelaunayTriangulation triangulation(std::move(tree));
  return triangulation;
}

std::vector<std::pair<int, int> > get_delaunay_triangulation_edges(const std::vector<std::pair<int, int> > &coords,
                                                                   bool shuffle = false) {
  DelaunayTriangulation triangulation = build_delaunay_triangulation(coords, shuffle);
  return triangulation.get_edges();
}

struct PairHash {
  int operator()(std::pair<int, int> p) const noexcept {
    return p.first;
  }
};

template<typename T>
bool feasible(const T &coords) {
  auto it = coords.cbegin();
  auto p1 = Point(*it);
  auto p2 = Point(*(++it));
  for (++it; it != coords.cend(); ++it) {
    if (side(p1, p2, Point(*it)) != 0) {
      return true;
    }
  }
  return false;
}

std::vector<std::pair<int, int> > generate_coords(int n, int bound) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(-bound, bound);
  std::unordered_set<std::pair<int, int>, PairHash> s;
  std::pair<int, int> pair;
  for (int i = 0; i < n - 1; ++i) {
    do {
      pair = {dist(gen), dist(gen)};
    } while (!s.insert(pair).second);
  }
  do {
    pair = {dist(gen), dist(gen)};
  } while (!s.insert(pair).second || !feasible(s));
  return std::vector<std::pair<int, int> >(s.begin(), s.end());
}

void test_output(const std::vector<std::pair<int, int> > &coords, const DelaunayTriangulation &triangulation) {
  std::cout << "Points:\n";
  for (auto &coord : coords) {
    std::cout << coord.first << ' ' << coord.second << std::endl;
  }
  auto edges = triangulation.get_edges();
  std::cout << "Edges:\n";
  for (auto &edge : edges) {
    std::cout << edge.first << ' ' << edge.second << std::endl;
  }
}

bool test(int points_num, int bound, bool output = false, bool error_output = true) {
  auto coords = generate_coords(points_num, bound);
  DelaunayTriangulation triangulation = build_delaunay_triangulation(coords);
  if (!triangulation.is_correct(coords)) {
    std::cout << "INCORRECT\n";
    if (error_output) {
      test_output(coords, triangulation);
    }
    return false;
  }
  if (output) {
    test_output(coords, triangulation);
  }
  return true;
}

int main() {
  int N, M, K;
  bool output;
  std::cout << "Number of points: ";
  std::cin >> N;
  std::cout << "Max coordinate value (should be not greater than 20 000): ";
  std::cin >> M;
  std::cout << "Number of tests: ";
  std::cin >> K;
  std::cout << "Would you like to output all tests (1 or 0)?\n";
  std::cin >> output;
  bool correct = true;
  for (int i = 0; i < K; ++i) {
    if (!test(N, M, output)) {
      correct = false;
      break;
    }
    if (output) {
      std::cout << "========\n";
    }
  }
  if (correct) {
    std::cout << "Everything worked fine\n";
  }
  return 0;
}