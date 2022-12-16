#pragma once

namespace uni_course_cpp {

using VertexId = int;

class IVertex {
public:
  virtual ~IVertex() {};

  virtual VertexId id() const = 0;
};

}  // namespace uni_course_cpp