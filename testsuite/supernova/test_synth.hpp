#include "../server/supernova/server/synth.hpp"

namespace nova
{
class test_synth:
    public abstract_synth
{
public:
    test_synth(int node_id, synth_prototype_ptr const & prototype):
        abstract_synth(node_id, prototype)
    {}

    void run(void)
    {}

    virtual void set(slot_index_t slot_id, sample val)
    {}

    virtual void set(slot_index_t slot_id, size_t count, sample * val)
    {}
};

}
