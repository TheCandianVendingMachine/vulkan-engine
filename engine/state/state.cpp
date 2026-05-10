#include "engine/state/state.h"

#include "engine/meta_defines.h"

ENGINE_NS::StatePipeline::StatePipeline(State& state) : owner_(state) {
}

ENGINE_NS::StatePipeline::~StatePipeline() {
}

auto ENGINE_NS::StatePipeline::record(VkCommandBuffer buffer) -> void {
    this->owner_.state_update_mutex_.lock();
    this->record_(buffer);
    this->owner_.state_update_mutex_.unlock();
}
