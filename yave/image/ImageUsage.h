/*******************************
Copyright (c) 2016-2017 Grégoire Angerand

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
#ifndef YAVE_IMAGE_IMAGEUSAGE_H
#define YAVE_IMAGE_IMAGEUSAGE_H

#include "ImageFormat.h"

namespace yave {

enum class ImageUsage {
    None = 0,

	TextureBit = uenum(vk::ImageUsageFlagBits::eSampled),
	DepthBit = uenum(vk::ImageUsageFlagBits::eDepthStencilAttachment),
	ColorBit = uenum(vk::ImageUsageFlagBits::eColorAttachment),
	StorageBit = uenum(vk::ImageUsageFlagBits::eStorage),

	SwapchainBit = detail::max(None, DepthBit, ColorBit, TextureBit, StorageBit) << 1,

	// Never use directly:
	Attachment = ColorBit | DepthBit,
	DepthTexture = TextureBit | DepthBit
};

constexpr ImageUsage operator|(ImageUsage l, ImageUsage r) {
	return ImageUsage(uenum(l) | uenum(r));
}

inline ImageUsage operator|(ImageUsage l, vk::ImageUsageFlags r) {
	return l | ImageUsage(uenum(r));
}

constexpr ImageUsage operator&(ImageUsage l, ImageUsage r)  {
	return ImageUsage(uenum(l) & uenum(r));
}

constexpr ImageUsage operator~(ImageUsage l) {
	return ImageUsage(~uenum(l));
}

constexpr bool is_attachment_usage(ImageUsage usage) {
	return (usage & ImageUsage::Attachment) != ImageUsage::None;
}

constexpr bool is_storage_usage(ImageUsage usage) {
	return (usage & ImageUsage::StorageBit) != ImageUsage::None;
}

constexpr bool is_texture_usage(ImageUsage usage) {
	return (usage & ImageUsage::TextureBit) != ImageUsage::None;
}

vk::ImageLayout vk_image_layout(ImageUsage usage);

}

#endif // YAVE_IMAGE_IMAGEUSAGE_H
