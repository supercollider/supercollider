#include "../server/supernova/server/synth.hpp"

namespace nova {
class test_synth : public abstract_synth {
public:
    test_synth(int node_id, synth_definition_ptr const& prototype): abstract_synth(node_id, prototype) {}

    void run() override {}

    float get(slot_index_t slot_id) const override { return 0.f; }

    bool getMappedSymbol(slot_index_t slot_id, char* str) const override { return false; }

    void set(slot_index_t slot_id, sample val) override {}

    void set_control_array(slot_index_t slot_id, size_t count, sample* val) override {}
};

}
