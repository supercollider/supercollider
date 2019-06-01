#include "../server/supernova/server/synth.hpp"

namespace nova {
class test_synth : public abstract_synth {
public:
    test_synth(int node_id, synth_definition_ptr const& prototype): abstract_synth(node_id, prototype) {}

    void run(void) {}

    virtual float get(slot_index_t slot_id) const { return 0.f; }

    virtual bool getMappedSymbol(slot_index_t slot_id, char* str) const { return false; }

    virtual void set(slot_index_t slot_id, sample val) {}

    virtual void set_control_array(slot_index_t slot_id, size_t count, sample* val) {}
};

}
