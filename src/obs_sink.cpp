#include "obs_sink.h"
#include "glsource.h"
#include <obs/graphics/graphics.h>

OBSSink::OBSSink(uint32_t fps) {
    this->fps = fps;
    next_frame = SINK_TIMESTAMP_ASAP;
    buffer = new RingBuffer(8);
}

OBSSink::~OBSSink() {
    ConnectVideoSource(nullptr);

    delete buffer;
};

void OBSSink::ReadVideoFrame(unsigned int width, unsigned int height, const uint8_t* data, int stride, AVPixelFormat format, int64_t timestamp) {

    if (buffer->full()) return;

    auto* new_data = new uint8_t[width * height * 4];
    for (int y = 0; y < height; ++y) {
        int destoffset = y * width * 4;
        int srcoffset = stride * y;
        for (int x = 0; x < width * 4; ++x) {
            if (x == 0 || x == width * 4 - 1) {
            }
            new_data[x + destoffset] = data[x + srcoffset];
        }
    }

    auto* fd = new FrameData(width, height, new_data);
    push_frame(fd);

    next_frame = timestamp + fps * 1000000000; // microseconds
}

int64_t OBSSink::GetNextVideoTimestamp() {
    return next_frame;
}

void OBSSink::push_frame(FrameData* data) {
    buffer->push(data);
}

FrameData* OBSSink::get_next_frame() {
    return buffer->pop();
}