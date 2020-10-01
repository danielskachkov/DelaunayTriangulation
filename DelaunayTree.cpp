#include <stack>
#include "DelaunayTree.h"

#define next(i) (i + 1) % 3
#define prev(i) (i + 2) % 3

DelaunayTree::DelaunayTree(const Point &p1, const Point &p2, const Point &p3) {
  head = std::make_shared<Node>(p1, p2, p3);
}

void DelaunayTree::add_point(const Point &p) {
  auto node = find_triangle(p);

  //check if the point lies on edge
  int ind = -1;
  for (int i = 0; i < 3; ++i) {
    const Point &p1 = node->points[i];
    const Point &p2 = node->points[next(i)];
    if (side(p1, p2, p) == 0) {
      ind = i;
      break;
    }
  }

  if (ind == -1) {
    add_interior_point(node, p);
  } else {
    add_borderline_point(p, ind, node);
  }
}

void DelaunayTree::add_interior_point(const std::shared_ptr<Node> &node, const Point &p) {
  for (int i = 0; i < 3; ++i) {
    const Point &p1 = node->points[i];
    const Point &p2 = node->points[next(i)];
    auto new_node = std::make_shared<Node>(p1, p2, p);
    if (p1.index >= 0 || p2.index >= 0) {
      auto neighbour = node->neighbours[i].lock();
      new_node->neighbours.push_back(neighbour);
      for (int l = 0; l < 3; ++l) {
        if (neighbour->neighbours[l].lock() == node) {
          neighbour->neighbours[l] = new_node;
          break;
        }
      }
    } else {
      new_node->neighbours.push_back(std::weak_ptr<Node>());
    }
    node->children.push_back(std::move(new_node));
  }

  for (int i = 0; i < 3; ++i) {
    auto child = node->children[i];
    child->neighbours.push_back(node->children[next(i)]);
    child->neighbours.push_back(node->children[prev(i)]);
  }

  for (int i = 0; i < 3; ++i) {
    flip(node->children[i], 0);
  }
}

void DelaunayTree::add_borderline_point(const Point &p, int i, const std::shared_ptr<Node> &node) {
  auto neighbour = node->neighbours[i].lock();
  int j = 0;
  for (; neighbour->neighbours[j].lock() != node; ++j);

  auto new_node1 = std::make_shared<Node>(node->points[prev(i)], node->points[i], p);
  auto new_node2 = std::make_shared<Node>(node->points[next(i)], node->points[prev(i)], p);
  auto new_node3 = std::make_shared<Node>(neighbour->points[next(j)], neighbour->points[prev(j)], p);
  auto new_node4 = std::make_shared<Node>(neighbour->points[prev(j)], neighbour->points[j], p);

  auto neighbour1 = node->neighbours[prev(i)].lock();
  auto neighbour2 = node->neighbours[next(i)].lock();
  auto neighbour3 = neighbour->neighbours[next(j)].lock();
  auto neighbour4 = neighbour->neighbours[prev(j)].lock();

  new_node1->neighbours.push_back(neighbour1);
  new_node2->neighbours.push_back(neighbour2);
  new_node3->neighbours.push_back(neighbour3);
  new_node4->neighbours.push_back(neighbour4);

  if (neighbour1 != nullptr) {
    for (int r = 0; r < 3; ++r) {
      if (neighbour1->neighbours[r].lock() == node) {
        neighbour1->neighbours[r] = new_node1;
        break;
      }
    }
  }

  if (neighbour2 != nullptr) {
    for (int r = 0; r < 3; ++r) {
      if (neighbour2->neighbours[r].lock() == node) {
        neighbour2->neighbours[r] = new_node2;
        break;
      }
    }
  }

  if (neighbour3 != nullptr) {
    for (int r = 0; r < 3; ++r) {
      if (neighbour3->neighbours[r].lock() == neighbour) {
        neighbour3->neighbours[r] = new_node3;
        break;
      }
    }
  }

  if (neighbour4 != nullptr) {
    for (int r = 0; r < 3; ++r) {
      if (neighbour4->neighbours[r].lock() == neighbour) {
        neighbour4->neighbours[r] = new_node4;
        break;
      }
    }
  }

  new_node1->neighbours.push_back(new_node3);
  new_node1->neighbours.push_back(new_node2);
  new_node2->neighbours.push_back(new_node1);
  new_node2->neighbours.push_back(new_node4);
  new_node3->neighbours.push_back(new_node4);
  new_node3->neighbours.push_back(new_node1);
  new_node4->neighbours.push_back(new_node2);
  new_node4->neighbours.push_back(new_node3);

  node->children.push_back(new_node1);
  node->children.push_back(new_node2);
  neighbour->children.push_back(new_node3);
  neighbour->children.push_back(new_node4);

  flip(new_node1, 0);
  flip(new_node2, 0);
  flip(new_node3, 0);
  flip(new_node4, 0);
}

void DelaunayTree::flip(const std::shared_ptr<Node> &node, int i) {
  //
  auto neighbour = node->neighbours[i].lock();
  if (neighbour == nullptr) {
    return;
  }
  int j = 0;
  for (; neighbour->neighbours[j].lock() != node; ++j);
  if (illegal(node->points[prev(i)], node->points[i], node->points[next(i)], neighbour->points[prev(j)])) {
    auto new_node1 = std::make_shared<Node>(node->points[prev(i)],
                                            node->points[i], neighbour->points[prev(j)]);
    auto new_node2 = std::make_shared<Node>(node->points[prev(i)],
                                            neighbour->points[prev(j)], neighbour->points[j]);

    node->children.push_back(new_node1);
    node->children.push_back(new_node2);
    neighbour->children.push_back(new_node1);
    neighbour->children.push_back(new_node2);

    new_node1->neighbours.push_back(node->neighbours[prev(i)]);
    new_node1->neighbours.push_back(neighbour->neighbours[next(j)]);
    new_node1->neighbours.push_back(new_node2);

    new_node2->neighbours.push_back(new_node1);
    new_node2->neighbours.push_back(neighbour->neighbours[prev(j)]);
    new_node2->neighbours.push_back(node->neighbours[next(i)]);

    auto neighbour1 = node->neighbours[prev(i)].lock();
    if (neighbour1 != nullptr) {
      for (int r = 0; r < 3; ++r) {
        if (neighbour1->neighbours[r].lock() == node) {
          neighbour1->neighbours[r] = new_node1;
        }
      }
    }

    neighbour1 = neighbour->neighbours[next(j)].lock();
    if (neighbour1 != nullptr) {
      for (int r = 0; r < 3; ++r) {
        if (neighbour1->neighbours[r].lock() == neighbour) {
          neighbour1->neighbours[r] = new_node1;
        }
      }
    }

    neighbour1 = neighbour->neighbours[prev(j)].lock();
    if (neighbour1 != nullptr) {
      for (int r = 0; r < 3; ++r) {
        if (neighbour1->neighbours[r].lock() == neighbour) {
          neighbour1->neighbours[r] = new_node2;
        }
      }
    }

    neighbour1 = node->neighbours[next(i)].lock();
    if (neighbour1 != nullptr) {
      for (int r = 0; r < 3; ++r) {
        if (neighbour1->neighbours[r].lock() == node) {
          neighbour1->neighbours[r] = new_node2;
        }
      }
    }

    flip(new_node1, 1);
    flip(new_node2, 1);
  }
}

bool DelaunayTree::illegal(const Point &p1, const Point &p2, const Point &p3, const Point &p) {
  if (p.index < 0 && p2.index >= 0 && p3.index >= 0) {
    return false;
  }
  if (p2.index < 0) {
    return side(p3, p1, p) == -1;
  }
  if (p3.index < 0) {
    return side(p1, p2, p) == -1;
  }
  return is_point_in_circle(p1, p2, p3, p);
}

std::shared_ptr<DelaunayTree::Node> DelaunayTree::find_triangle(const Point &p) {
  auto node = head;
  while (!node->children.empty()) {
    for (const auto &child : node->children) {
      if (is_point_in_polygon(p, child->points)) {
        node = child;
      }
    }
  }
  return node;
}