#pragma once

#define int64(a) static_cast <int64_t> (a)
#define mul(a, b, c) int64(a)*int64(b)*int64(c)
#define det2(a, b) (mul(a.y, b.x, b.x) + mul(a.y, b.y, b.y) - mul(b.y, a.x, a.x) - mul(b.y, a.y, a.y))
#define det3(a, b, c) (int64(a.x)*det2(b, c) - int64(b.x)*det2(a, c) + int64(c.x)*det2(a, b))
#define det4(a, b, c, d) (-det3(b, c, d) + det3(a, c, d) - det3(a, b, d) + det3(a, b, c))

#include <iostream>

struct Point {
  int x, y;
  int index;

  Point() = default;
  explicit Point(const std::pair<int, int> &p) : x(p.first), y(p.second), index(0) {}
  Point(int x, int y, int index) : x(x), y(y), index(index) {}

  bool operator==(const Point &other) const {
    return this->x == other.x && this->y == other.y;
  }

  bool operator!=(const Point &other) const {
    return !(*this == other);
  }
};

inline std::istream &operator>>(std::istream &is, Point &p) {
  is >> p.x >> p.y;
  return is;
}

inline int side(const Point &p1, const Point &p2, const Point &p) {
  int64_t d1 = (int64(p.x) - int64(p1.x)) * (int64(p2.y) - int64(p1.y));
  int64_t d2 = (int64(p.y) - int64(p1.y)) * (int64(p2.x) - int64(p1.x));
  if (d1 > d2) {
    return 1;
  }
  if (d1 < d2) {
    return -1;
  }
  return 0;
}

inline bool intersect(const Point &p1, const Point &p2, const Point &s1, const Point &s2) {
  if ((p1 == s1 && p2 == s2) || (p1 == s2 && p2 == s1)) {
    return true;
  }

  int p1_side = side(s1, s2, p1);
  int p2_side = side(s1, s2, p2);
  int s1_side = side(p1, p2, s1);
  int s2_side = side(p1, p2, s2);

#define inside_coord(p, s, r, c) p.c >= std::min(s.c, r.c) && p.c <= std::max(s.c, r.c)
#define inside(p, s, r) inside_coord(p, s, r, x) && inside_coord(p, s, r, y) && p != s && p != r
  if (p1_side == 0 && inside(p1, s1, s2)) {
    return true;
  }
  if (p2_side == 0 && inside(p2, s1, s2)) {
    return true;
  }
  if (s1_side == 0 && inside(s1, p1, p2)) {
    return true;
  }
  if (s2_side == 0 && inside(s2, p1, p2)) {
    return true;
  }
  if (p1_side == 0 || p2_side == 0 || s1_side == 0 || s2_side == 0) {
    return false;
  }
  return (p1_side != p2_side && s1_side != s2_side);
}

inline bool is_point_in_polygon(const Point &p, const std::vector<Point> &vertices) {
  for (int i = 0; i < vertices.size(); ++i) {
    const Point &p1 = vertices[i];
    const Point &p2 = vertices[(i + 1) % vertices.size()];
    if (side(p1, p2, p) == 1) {
      return false;
    }
  }
  return true;
}

inline bool is_point_in_circle(const Point &p1, const Point &p2, const Point &p3, const Point &p) {
  return det4(p1, p2, p3, p) > 0;
}
