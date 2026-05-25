#include "c_api.h"

#include "GraphHelper.h"
#include "ModularityVertexPartition.h"
#include "Optimiser.h"

#include <igraph/igraph.h>

#include <exception>
#include <string>

namespace
{
  thread_local std::string g_last_error;

  int set_last_error(int code, const std::string& message)
  {
    g_last_error = message;
    return code;
  }
}

int libleidenalg_modularity_partition(
  int64_t vertex_count,
  int64_t edge_count,
  const int64_t* edges,
  int directed,
  uint64_t seed,
  int64_t* membership_out,
  int64_t membership_len,
  double* quality_out
)
{
  g_last_error.clear();

  if (vertex_count < 0 || edge_count < 0)
    return set_last_error(LIBLEIDENALG_ERR_INVALID_ARGUMENT, "vertex_count and edge_count must be non-negative.");

  if ((edge_count > 0) && edges == NULL)
    return set_last_error(LIBLEIDENALG_ERR_INVALID_ARGUMENT, "edges must be non-null when edge_count is greater than zero.");

  if (membership_out == NULL)
    return set_last_error(LIBLEIDENALG_ERR_INVALID_ARGUMENT, "membership_out must be non-null.");

  if (membership_len < vertex_count)
    return set_last_error(LIBLEIDENALG_ERR_INVALID_ARGUMENT, "membership_len must be at least vertex_count.");

  igraph_vector_int_t edge_vector;
  igraph_vector_int_init(&edge_vector, edge_count * 2);

  for (int64_t i = 0; i < edge_count * 2; i++)
    VECTOR(edge_vector)[i] = static_cast<igraph_integer_t>(edges[i]);

  igraph_t graph = {};
  const int igraph_result = igraph_create(
    &graph,
    &edge_vector,
    static_cast<igraph_integer_t>(vertex_count),
    directed ? 1 : 0
  );
  igraph_vector_int_destroy(&edge_vector);

  if (igraph_result != IGRAPH_SUCCESS)
    return set_last_error(LIBLEIDENALG_ERR_IGRAPH, "igraph_create failed.");

  try
  {
    {
      Graph wrapped_graph(&graph);
      ModularityVertexPartition partition(&wrapped_graph);
      Optimiser optimiser;
      optimiser.set_rng_seed(static_cast<size_t>(seed));
      optimiser.optimise_partition(&partition);

      if (quality_out != NULL)
        *quality_out = partition.quality();

      vector<size_t> const& membership = partition.membership();
      for (size_t i = 0; i < membership.size(); i++)
        membership_out[i] = static_cast<int64_t>(membership[i]);
    }
  }
  catch (std::exception const& ex)
  {
    igraph_destroy(&graph);
    return set_last_error(LIBLEIDENALG_ERR_INTERNAL, ex.what());
  }
  catch (...)
  {
    igraph_destroy(&graph);
    return set_last_error(LIBLEIDENALG_ERR_INTERNAL, "Unknown internal error.");
  }

  igraph_destroy(&graph);
  return LIBLEIDENALG_OK;
}

const char* libleidenalg_last_error(void)
{
  return g_last_error.c_str();
}
