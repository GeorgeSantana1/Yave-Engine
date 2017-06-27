/*******************************
Copyright (c) 2016-2017 Gr�goire Angerand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********************************/
#ifndef YAVE_SWAPCHAIN_FRAMETOKEN_H
#define YAVE_SWAPCHAIN_FRAMETOKEN_H

#include <yave/images/ImageView.h>
#include <yave/commands/CmdBufferRecorder.h>

namespace yave {

struct FrameToken {
	const u64 id;
	const u32 image_index;
	const u32 image_count;

	const ImageView<ImageUsage::ColorBit | ImageUsage::StorageBit> image_view;
	const vk::Semaphore image_aquired;
	const vk::Semaphore render_finished;


	bool operator==(const FrameToken& other) const {
		return id == other.id &&
			   image_view == other.image_view &&
			   image_index == other.image_index &&
			   image_count == other.image_count &&
			   image_aquired == other.image_aquired &&
			   render_finished == other.render_finished;
	}

	bool operator!=(const FrameToken& other) const {
		return !operator==(other);
	}
};



}

#endif // YAVE_SWAPCHAIN_FRAMETOKEN_H
