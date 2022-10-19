#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Graph {
 public:
  using VertexId = int;
  using EdgeId = int;

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

  void add_vertex() {
    const auto vertex_id = gen_new_vertex_id();
    vertices_.insert(std::make_pair(vertex_id, Vertex(vertex_id)));
    connections_list_[vertex_id] = {};
  }

  void add_edge(VertexId from_vertex_id, VertexId to_vertex_id) {
    assert(has_vertex(from_vertex_id));
    assert(has_vertex(to_vertex_id));
    const auto edge_id = gen_new_edge_id();
    edges_.insert(
        std::make_pair(edge_id, Edge(edge_id, from_vertex_id, to_vertex_id)));
    if (from_vertex_id != to_vertex_id) {
      connections_list_[from_vertex_id].insert(edge_id);
    }

    connections_list_[to_vertex_id].insert(edge_id);
  }

  const std::unordered_map<VertexId, Vertex>& vertices() const {
    return vertices_;
  }
  const std::unordered_map<EdgeId, Edge>& edges() const { return edges_; }
  const std::unordered_set<EdgeId>& connected_edge_ids(
      VertexId vertex_id) const {
    assert(has_vertex(vertex_id));
    return connections_list_.at(vertex_id);
  }

  bool has_vertex(VertexId id) const {
    return vertices_.find(id) != vertices_.end();
  }

  bool has_edge(EdgeId id) const { return edges_.find(id) != edges_.end(); }

 private:
  EdgeId gen_new_edge_id() { return next_edge_id_++; }

  VertexId gen_new_vertex_id() { return next_vertex_id_++; }

  VertexId next_vertex_id_ = 0;
  EdgeId next_edge_id_ = 0;
  std::unordered_map<VertexId, Vertex> vertices_;
  std::unordered_map<EdgeId, Edge> edges_;
  std::unordered_map<VertexId, std::unordered_set<EdgeId>> connections_list_;
};

namespace printing {
namespace json {

std::string print_vertex(const Graph::Vertex& vertex, const Graph& graph) {
  const auto vertex_id = vertex.id();
  std::string vertex_json =
      "{\"id\":" + std::to_string(vertex_id) + ",\"edge_ids\":[";
  const auto& connected_edge_ids = graph.connected_edge_ids(vertex_id);

  for (const auto& edge_id : connected_edge_ids) {
    vertex_json += std::to_string(edge_id) + ",";
  }

  if (!connected_edge_ids.empty()) {
    vertex_json.pop_back();
  }

  vertex_json += "]}";

  return vertex_json;
}

std::string print_edge(const Graph::Edge& edge) {
  const auto edge_id = edge.id();
  return "{\"id\":" + std::to_string(edge_id) + ",\"vertex_ids\":[" +
         std::to_string(edge.from_vertex_id()) + "," +
         std::to_string(edge.to_vertex_id()) + "]}";
}

std::string print_graph(const Graph& graph) {
  std::string graph_json = "{\"vertices\":[";
  const auto& vertices = graph.vertices();

  if (!vertices.empty()) {
    for (const auto& [vertex_id, vertex] : vertices) {
      graph_json += print_vertex(vertex, graph) + ",";
    }

    graph_json.pop_back();
  }

  graph_json += "],\"edges\":[";

  const auto& edges = graph.edges();

  if (!edges.empty()) {
    for (const auto& [edge_id, edge] : edges) {
      graph_json += print_edge(edge) + ",";
    }

    graph_json.pop_back();
  }

  graph_json += "]}";

  return graph_json;
}
}  // namespace json
}  // namespace printing

Graph generate_graph(const int& kVerticesCount = 14) {
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

void write_to_file(const std::string& str, const std::string& filename) {
  std::ofstream out(filename, std::ios::trunc);
  out << str << std::endl;
}

int main() {
  const auto graph = generate_graph();
  const auto graph_json = printing::json::print_graph(graph);
  std::cout << graph_json << std::endl;
  write_to_file(graph_json, "graph.json");

  return 0;
}
