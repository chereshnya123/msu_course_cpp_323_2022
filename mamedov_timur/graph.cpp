#include "graph.hpp"

#include <iostream>

Graph::Vertex::Vertex(VertexId id) : id_(id) {}

Graph::VertexId Graph::Vertex::id() const {
  return id_;
}

Graph::Edge::Edge(EdgeId id,
                  VertexId first_vertex_id,
                  VertexId second_vertex_id,
                  Edge::Color color)
    : id_(id),
      first_vertex_id_(first_vertex_id),
      second_vertex_id_(second_vertex_id),
      color_(color) {}

Graph::EdgeId Graph::Edge::id() const {
  return id_;
}

Graph::VertexId Graph::Edge::get_first_vertex_id() const {
  return first_vertex_id_;
}
Graph::VertexId Graph::Edge::get_second_vertex_id() const {
  return second_vertex_id_;
}

Graph::Edge::Color Graph::Edge::color() const {
  return color_;
}

Graph::VertexId Graph::add_vertex() {
  const VertexId new_vertex_id = get_new_vertex_id();
  Vertex new_vertex(new_vertex_id);

  vertices_.insert({new_vertex_id, Vertex(new_vertex_id)});
  return new_vertex_id;
}

Graph::EdgeId Graph::add_edge(VertexId first_vertex_id,
                              VertexId second_vertex_id) {
  assert(has_vertex_id(first_vertex_id));
  assert(has_vertex_id(second_vertex_id));

  const EdgeId new_edge_id = get_new_edge_id();
  auto color = Graph::Edge::Color::Grey;

  if (first_vertex_id == second_vertex_id){
    color = Graph::Edge::Color::Green;
    connections_[first_vertex_id].push_back(new_edge_id);
    edges_.insert({new_edge_id, Edge(new_edge_id, first_vertex_id,
                                     second_vertex_id, color)});

    return new_edge_id;
  }

  auto first_vertex_depth = vertex_depth(first_vertex_id);
  auto second_vertex_depth = vertex_depth(second_vertex_id);

  if (second_vertex_depth == -1) {
    color = Graph::Edge::Color::Grey;
    if (first_vertex_depth == -1) {
      vertices_depth_.insert({0, std::vector<VertexId>{first_vertex_id}});
      vertices_depth_.insert({1, std::vector<VertexId>{second_vertex_id}});
    } else {
          if ((first_vertex_depth + 1) < vertices_depth_.size())
            vertices_depth_.at(first_vertex_depth + 1).emplace_back(second_vertex_id);
          else
            vertices_depth_.insert(
                {first_vertex_depth + 1, std::vector<VertexId>{second_vertex_id}});
        }
  } else {
    if (second_vertex_depth - first_vertex_depth == 1)
      color = Graph::Edge::Color::Yellow;
    else if (first_vertex_depth - second_vertex_depth == 2) color = Graph::Edge::Color::Red;
  }

  connections_[first_vertex_id].push_back(new_edge_id);
  connections_[second_vertex_id].emplace_back(new_edge_id);

  edges_.insert({new_edge_id, Edge(new_edge_id, first_vertex_id,
                                   second_vertex_id, color)});

  return new_edge_id;
};

const std::unordered_map<Graph::EdgeId, Graph::Edge>& Graph::get_edges() const {
  return edges_;
}
const std::unordered_map<Graph::VertexId, Graph::Vertex>& Graph::get_vertices()
    const {
  return vertices_;
}

const std::vector<Graph::EdgeId>& Graph::get_edges_of_vertex(
    Graph::VertexId vertex_id) const {
  return connections_.at(vertex_id);
}

const std::map<Graph::Depth, std::vector<Graph::VertexId>>& Graph::get_vertices_depth() const{
  return vertices_depth_;
}

bool Graph::has_vertex_id(VertexId vertex_id) const {
  return vertices_.find(vertex_id) != vertices_.end();
}

Graph::Depth Graph::vertex_depth(VertexId vertex_id) const{
  for (auto it = vertices_depth_.begin(); it != vertices_depth_.end(); ++it) {
    if (std::find(it->second.begin(), it->second.end(), vertex_id) !=
        it->second.end()) return it->first;
  }
  return -1;
}

bool Graph::has_edge(VertexId first_vertex_id, VertexId second_vertex_id) const{
  if (connections_.find(first_vertex_id) == connections_.end())
    return false;
  if (connections_.find(second_vertex_id) == connections_.end())
    return false;

  const auto first_vertex_edges = get_edges_of_vertex(first_vertex_id);
  const auto second_vertex_edges = get_edges_of_vertex(second_vertex_id);
  return std::find_first_of(
             first_vertex_edges.begin(), first_vertex_edges.end(),
             second_vertex_edges.begin(),
             second_vertex_edges.end()) != first_vertex_edges.end();
}

Graph::VertexId Graph::get_new_vertex_id() {
  return last_vertex_id_++;
};

Graph::EdgeId Graph::get_new_edge_id() {
  return last_edge_id_++;
};

// GraphGenerator's interface

GraphGenerator::Params::Params(Graph::Depth depth, int new_vertices_count)
    : depth_(depth), new_vertices_count_(new_vertices_count) {}

Graph::Depth GraphGenerator::Params::depth() const {
  return depth_;
}

int GraphGenerator::Params::new_vertices_count() const {
  return new_vertices_count_;
}

GraphGenerator::GraphGenerator(Params&& params) : params_(std::move(params)) {}

Graph GraphGenerator::generate() const {
  auto graph = Graph();
  graph.add_vertex();
  generate_grey_edges(graph);
  generate_green_edges(graph);
  generate_yellow_edges(graph);
  return graph;
}

void GraphGenerator::generate_grey_edges(Graph& graph) const {
  const auto max_depth = params_.depth() - 1;
  const auto new_vertices_count = params_.new_vertices_count();

  double probability = 1.0;
  double step = 1.0 / max_depth;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::vector<Graph::VertexId> vertices = {graph.get_vertices().begin()->first};
  std::vector<Graph::VertexId> vertices_next;

  for (Graph::Depth temp_depth = 0; temp_depth < max_depth; ++temp_depth) {
    std::bernoulli_distribution d(probability);
    for (auto vertex = vertices.begin(); vertex != vertices.end(); ++vertex) {
      for (int j = 0; j < new_vertices_count; ++j) {
        if (d(gen)) {
          const auto new_vertex_id = graph.add_vertex();
          graph.add_edge(*vertex, new_vertex_id);
          vertices_next.emplace_back(new_vertex_id);
        }
      }
    }
    probability -= step;
    vertices = vertices_next;
    vertices_next.clear();
  }
}

void GraphGenerator::generate_green_edges(Graph& graph) const {
  std::random_device rd;
  std::mt19937 gen(rd());
  double probability = 0.1;
  std::bernoulli_distribution d(probability);
  const auto vertices = graph.get_vertices();
  for (auto vertex : vertices) {
    if (d(gen)) {
      graph.add_edge(vertex.first, vertex.first);
    }
  }
}

void GraphGenerator::generate_yellow_edges(Graph& graph) const {
  const auto max_depth = params_.depth() - 1;
  double probability = 1.0;
  double step = 1.0 / max_depth;
  std::random_device rd;
  std::mt19937 gen(rd());
  const auto vertices = graph.get_vertices_depth();

  auto it_temp = vertices.begin();
  auto it_next = it_temp;
  ++it_next;

  for (; it_next != vertices.end(); ++it_next, ++it_temp){
    std::bernoulli_distribution d(probability);
    std::uniform_int_distribution<> distrib(0, it_next->second.size() - 1);
    for (auto vertex = it_temp->second.begin(); vertex != it_temp->second.end(); ++vertex){
      if (d(gen)){
        int tries = 0;
        while (tries < it_next->second.size()){
          ++tries;
          auto new_vertex_id = it_next->second[distrib(gen)];
          if (!graph.has_edge(*vertex, new_vertex_id)) {
            graph.add_edge(*vertex, new_vertex_id);
            std::cout << *vertex << " " << new_vertex_id << std::endl;
            break;
          }
        }
      }
    }
    probability += step;
  }
}