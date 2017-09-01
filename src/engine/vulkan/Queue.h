#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace engine::vulkan {

class Queue {

public:

                    Queue();
                    Queue(VkQueue queue_, int index_);
                    Queue(const Queue& other);

    void            SetIndex(int index_) { index = index_; }
    void            SetQueue(VkQueue queue_) { queue = queue_; }
    int             GetIndex() const { return index; }

private:

    VkQueue         queue;
    int             index;

};

}
