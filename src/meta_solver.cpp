#include "meta_solver.h"
#include "oracle.h"

namespace banana {
namespace solver {

void MetaSolver::explain(std::vector<Oracle::Vertex> &order)
{
  assert(order.size() == 0);
  for (auto vertex : m_order)
  {
    order.push_back(vertex);
  }
}

} // namespace solver
} // namespace banana