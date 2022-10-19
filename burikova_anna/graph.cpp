#include <cassert>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

constexpr int kVerticesCount = 14;

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

  void add_vertex() {
    const VertexId new_vertex_id = generate_vertex_id();
    adjacency_list_[new_vertex_id] = {};
    vertices_.emplace(new_vertex_id, Vertex(new_vertex_id));
  }
  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    assert(has_vertex(from_vertex_id));
    assert(has_vertex(to_vertex_id));
    assert(!has_edge(from_vertex_id, to_vertex_id));
    const EdgeId new_edge_id = generate_edge_id();
    edges_.emplace(new_edge_id,
                   Edge(new_edge_id, from_vertex_id, to_vertex_id));
    adjacency_list_[from_vertex_id].emplace_back(new_edge_id);
    if (from_vertex_id != to_vertex_id) {
      adjacency_list_[to_vertex_id].emplace_back(new_edge_id);
    }
  }

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

  const std::unordered_map<VertexId, Vertex>& get_vertices() const {
    return vertices_;
  }

  const std::unordered_map<EdgeId, Edge>& get_edges() const { return edges_; }

  const std::vector<EdgeId>& get_connected_edge_ids(VertexId vertex_id) const {
    return adjacency_list_.at(vertex_id);
  }

 private:
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::vector<EdgeId>> adjacency_list_;

  VertexId num_vertices_ = 0;
  EdgeId num_edges_ = 0;

  VertexId generate_vertex_id() { return num_vertices_++; }
  EdgeId generate_edge_id() { return num_edges_++; }

  bool has_vertex(VertexId vertex_id) const {
    return vertices_.find(vertex_id) != vertices_.end();
  }
  bool has_edge(VertexId from_vertex_id, VertexId to_vertex_id) const {
    for (const auto& [edge_id, edge] : edges_) {
      if ((edge.from_vertex_id() == from_vertex_id &&
           edge.to_vertex_id() == to_vertex_id) ||
          (edge.from_vertex_id() == to_vertex_id &&
           edge.to_vertex_id() == from_vertex_id))
        return true;
    }
    return false;
  }
};

namespace printing {
namespace json {
std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  std::string edges_ids_string = "[";
  for (const Graph::EdgeId edge_id :
       graph.get_connected_edge_ids(vertex.id())) {
    edges_ids_string += std::to_string(edge_id) + ',';
  }
  edges_ids_string.erase(edges_ids_string.length() - 1,
                         edges_ids_string.length());
  return "\"id\":" + std::to_string(vertex.id()) +
         ",\"edge_ids\":" + edges_ids_string + "]";
}

std::string print_edge(const Graph::Edge& edge) {
  return "\"id\":" + std::to_string(edge.id()) + ",\"vertex_ids\":" + "[" +
         std::to_string(edge.from_vertex_id()) + "," +
         std::to_string(edge.to_vertex_id()) + "]";
}

std::string print_graph(const Graph& graph) {
  std::string graph_string = "{\n  \"vertices\": [\n";
  for (const auto& [vertex_id, vertex] : graph.get_vertices()) {
    graph_string += "    {" + print_vertex(vertex, graph) + "},\n";
  }
  graph_string.erase(graph_string.length() - 2, graph_string.length());
  graph_string += "\n  ],\n  \"edges\": [\n";
  for (const auto& [edge_id, edge] : graph.get_edges()) {
    graph_string += "    {" + print_edge(edge) + "},\n";
  }
  graph_string.erase(graph_string.length() - 2, graph_string.length());
  return graph_string + "\n  ]\n}\n";
}
}  // namespace json
}  // namespace printing

Graph generate_graph() {
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

  return graph;
}

void write_to_file(const std::string& content, const std::string& file_name) {
  std::ofstream new_file(file_name);

  new_file << content;
}

int main() {
  const auto graph = generate_graph();
  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;

  write_to_file(graph_json, "graph.json");

  return 0;
}
