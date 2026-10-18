#include "sc_sema/ir.hpp"
#include "sc_util/typed_graph.hpp"

namespace sc::util::typed_graph {

template struct sc::util::typed_graph::GraphImplementation<sc::ir::OptNodeIndex, sc::ir::NodeIndex,
                                                           sc::ir::NodeCollectionHelper>;
}


namespace sc::ir {


}