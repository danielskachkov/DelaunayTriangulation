#pragma once

#include "DelaunayTree.h"

class DelaunayTriangulation {
 public:
  explicit DelaunayTriangulation(DelaunayTree &&);
  std::vector<std::pair<int, int> > get_edges() const;
  bool is_correct(const std::vector<std::pair<int, int> > &points);

 private:
  std::vector<std::shared_ptr<DelaunayTree::Node> > nodes;
};
