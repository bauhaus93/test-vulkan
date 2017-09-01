#include "Queue.h"

namespace engine::vulkan {

Queue::Queue():
    queue { VK_NULL_HANDLE },
    index { -1 } {
}

Queue::Queue(VkQueue queue_, int index_):
    queue { queue_ },
    index { index_ } {
}

Queue::Queue(const Queue& other):
    queue { other.queue },
    index { other.index } {
}

}
