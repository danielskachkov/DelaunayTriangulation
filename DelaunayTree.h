#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "GeometricalObjects.h"

class DelaunayTree {
 public:
  DelaunayTree(const Point &, const Point &, const Point &);
  DelaunayTree(const DelaunayTree &) = delete;
  DelaunayTree &operator=(const DelaunayTree &) = delete;
  void add_point(const Point &);
  friend class DelaunayTriangulation;

 private:
  struct Node {
    std::vector<Point> points;
    std::vector<std::weak_ptr<Node> > neighbours;
    std::vector<std::shared_ptr<Node> > children;
    mutable int ind = -2;

    Node(Point p1, Point p2, Point p3) : points({p1, p2, p3}) {}
  };

  std::shared_ptr<Node> head;

  void add_interior_point(const std::shared_ptr<Node> &, const Point &);
  void add_borderline_point(const Point &, int, const std::shared_ptr<Node> &);
  void flip(const std::shared_ptr<Node> &, int);
  static bool illegal(const Point &, const Point &, const Point &, const Point &);
  std::shared_ptr<Node> find_triangle(const Point &);
};
