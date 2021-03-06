/*******************************
Copyright (c) 2016-2020 Grégoire Angerand

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
#ifndef YAVE_GRAPHICS_FRAMEBUFFER_RENDERPASS_H
#define YAVE_GRAPHICS_FRAMEBUFFER_RENDERPASS_H

#include <yave/yave.h>

#include <yave/device/DeviceLinked.h>
#include <yave/device/DeviceLinked.h>
#include <yave/graphics/images/Image.h>
#include <yave/graphics/images/ImageView.h>

#include <y/core/Vector.h>

namespace yave {

class RenderPass : NonCopyable, public DeviceLinked {
	public:
		enum class LoadOp : u32 {
			Clear,
			Load
		};

		struct ImageData {
			const ImageFormat format;
			const ImageUsage usage = ImageUsage::None;
			const LoadOp load_op = LoadOp::Clear;

			ImageData() = default;

			ImageData(ImageFormat fmt, ImageUsage us, LoadOp op) : format(fmt), usage(us), load_op(op) {
			}

			ImageData(const ImageBase& img, LoadOp op) : format(img.format()), usage(img.usage()), load_op(op) {
			}

			template<ImageUsage Usage>
			ImageData(const ImageView<Usage>& img, LoadOp op) : format(img.format()), usage(img.usage()), load_op(op) {
			}
		};

		class Layout {
			public:
				Layout() = default;
				Layout(ImageData depth, core::Span<ImageData> colors);

				u64 hash() const;
				bool is_depth_only() const;

				bool operator==(const Layout& other) const;

			private:
				ImageFormat _depth;
				core::SmallVector<ImageFormat, 7> _colors;
		};

		RenderPass() = default;
		RenderPass(RenderPass&&) = default;
		RenderPass& operator=(RenderPass&&) = default;

		RenderPass(DevicePtr dptr, ImageData depth, core::Span<ImageData> colors);
		RenderPass(DevicePtr dptr, core::Span<ImageData> colors);

		~RenderPass();

		bool is_depth_only() const;

		const Layout& layout() const;
		usize attachment_count() const;

		VkRenderPass vk_render_pass() const;

	private:
		usize _attachment_count = 0;
		SwapMove<VkRenderPass> _render_pass;

		Layout _layout;
};

}

#endif // YAVE_GRAPHICS_FRAMEBUFFER_RENDERPASS_H
