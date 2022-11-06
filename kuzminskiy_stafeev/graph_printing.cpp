#include "graph_printing.hpp"

namespace uni_course_cpp {

std::string printing::print_edge_color(const Graph::Edge::Color& color) {
  switch (color) {
    case Graph::Edge::Color::Grey:
      return "grey";
    case Graph::Edge::Color::Yellow:
      return "yellow";
    case Graph::Edge::Color::Red:
      return "red";
    case Graph::Edge::Color::Green:
      return "green";
    default:
      return "No color";
  }
}

std::string printing::print_graph(const Graph& graph) {
  const auto depth = graph.get_graph_depth();

  const auto vertices = graph.vertices().size();
  const auto edges = graph.edges().size();

  const std::vector<Graph::Edge::Color> edge_colors = {
      Graph::Edge::Color::Grey, Graph::Edge::Color::Green,
      Graph::Edge::Color::Yellow, Graph::Edge::Color::Red};

  std::string depths_s = "";

  for (int i = 1; i < depth + 1; i++) {
    depths_s += std::to_string(graph.vertices_of_depth(i).size());

    if (i != depth) {
      depths_s += ", ";
    }
  }

  std::string edges_s = "";
  for (int i = 0; i < edge_colors.size(); i++) {
    edges_s += print_edge_color(edge_colors[i]) + ": " +
               std::to_string(graph.edges_ids_of_color(edge_colors[i]).size());
    if (i != edge_colors.size() - 1) {
      edges_s += ", ";
    }
  }

  return "  depth: " + std::to_string(depth) + ",\n" +
         "  vertices: {amount: " + std::to_string(vertices) +
         ", distribution: " + "[" + depths_s + "]},\n" +
         "  edges: {amount: " + std::to_string(edges) +
         ", distribution: " + "{" + edges_s + "}}";
}

}  // namespace uni_course_cpp
