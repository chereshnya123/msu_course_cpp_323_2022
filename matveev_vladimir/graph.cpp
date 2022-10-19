#include <cassert>
#include <vector>

constexpr int kVerticesCount = 14;

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

  void add_vertex() { vector_vertices_.emplace_back(generate_vertex_id()); }
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    assert(has_vertex(from_vertex_id));
    assert(has_vertex(to_vertex_id));
    assert(!has_edge(from_vertex_id, to_vertex_id));
    vector_edges_.emplace_back(generate_edge_id(), from_vertex_id,
                               to_vertex_id);
  }

 private:
  struct Vertex {
   public:
    explicit Vertex(VertexId id) : id_(id) {}
    VertexId id() const { return id_; }

   private:
    VertexId id_ = 0;
  };

  struct Edge {
   public:
    Edge(EdgeId id, VertexId from_vertex_id, VertexId to_vertex_id)
        : id_(id),
          from_vertex_id_(from_vertex_id),
          to_vertex_id_(to_vertex_id) {}
    EdgeId id() const { return id_; }
    VertexId from_vertex_id() const { return from_vertex_id_; }
    VertexId to_vertex_id() const { return to_vertex_id_; }

   private:
    EdgeId id_ = 0;
    VertexId from_vertex_id_ = 0;
    VertexId to_vertex_id_ = 0;
  };

  std::vector<Vertex> vector_vertices_;
  std::vector<Edge> vector_edges_;

  VertexId num_vertices_ = 0;
  EdgeId num_edges_ = 0;

  VertexId generate_vertex_id() { return num_vertices_++; }
  EdgeId generate_edge_id() { return num_edges_++; }

  bool has_vertex(VertexId id) const {
    for (const auto& vertex : vector_vertices_)
      if (vertex.id() == id)
        return true;
    return false;
  }
  bool has_edge(VertexId id_from, VertexId id_to) const {
    for (const auto& edge : vector_edges_) {
      if (edge.from_vertex_id() == id_from && edge.to_vertex_id() == id_to ||
          edge.from_vertex_id() == id_to && edge.to_vertex_id() == id_from)
        return true;
    }
    return false;
  }
};

int main() {
  auto graph = Graph();

  for (int i = 0; i < kVerticesCount; i++) {
    graph.add_vertex();
  }

  graph.add_edge(0, 1);
  graph.add_edge(0, 2);
  graph.add_edge(0, 3);
  graph.add_edge(1, 4);
  graph.add_edge(1, 5);
  graph.add_edge(1, 6);
  graph.add_edge(2, 7);
  graph.add_edge(2, 8);
  graph.add_edge(3, 9);
  graph.add_edge(4, 10);
  graph.add_edge(5, 10);
  graph.add_edge(6, 10);
  graph.add_edge(7, 11);
  graph.add_edge(8, 11);
  graph.add_edge(9, 12);
  graph.add_edge(10, 13);
  graph.add_edge(11, 13);
  graph.add_edge(12, 13);

  return 0;
}
