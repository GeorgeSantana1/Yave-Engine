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
#ifndef YAVE_GRAPHICS_COMMANDS_CMDBUFFERRECORDER_H
#define YAVE_GRAPHICS_COMMANDS_CMDBUFFERRECORDER_H

#include <yave/yave.h>
#include <yave/graphics/barriers/Barrier.h>
#include <yave/graphics/framebuffer/Viewport.h>

#include "CmdBuffer.h"

namespace yave {

class DescriptorSetBase;
class Semaphore;
class ComputeProgram;
class Framebuffer;
class Material;
class MaterialTemplate;
class GraphicPipeline;
class RenderPass;


namespace detail {
using DescriptorSetList = core::Span<DescriptorSetBase>;
}

class PushConstant : NonCopyable {

	public:
		constexpr PushConstant() = default;

		template<typename T>
		constexpr PushConstant(const T& data) : _data(&data), _size(sizeof(T)) {
			static_assert(sizeof(T) % 4 == 0, "PushConstant's size must be a multiple of 4");
			static_assert(sizeof(T) <= 128, "PushConstant's size must be at most 128 bytes");
			static_assert(std::is_standard_layout_v<T>, "T is not standard layout");
		}

		template<typename T>
		constexpr PushConstant(core::Span<T> arr) : _data(arr.data()), _size(arr.size() * sizeof(T)) {
			static_assert(sizeof(T) % 4 == 0, "PushConstant's size must be a multiple of 4");
			static_assert(sizeof(T) <= 128, "PushConstant's size must be at most 128 bytes");
			static_assert(std::is_standard_layout_v<T>, "T is not standard layout");
		}

		PushConstant(PushConstant&&) = delete;
		PushConstant& operator=(PushConstant&&) = delete;

		const void* data() const {
			return _data;
		}

		usize size() const {
			return _size;
		}

		bool is_empty() const {
			return !_size;
		}

	private:
		const void* _data = nullptr;
		usize _size = 0;
};

struct CmdBufferRegion : NonMovable, public DeviceLinked {
	public:
		CmdBufferRegion() = default;

		~CmdBufferRegion();

	private:
		friend class CmdBufferRecorder;

		CmdBufferRegion(const CmdBufferRecorder& cmd_buffer, const char* name, const math::Vec4& color);

		VkCommandBuffer _buffer = {};
};

class RenderPassRecorder : NonMovable {
	public:
		using DescriptorSetList = detail::DescriptorSetList;

		~RenderPassRecorder();

		DevicePtr device() const;
		bool is_null() const;

		// specific
		void bind_material(const Material& material);
		void bind_material(const MaterialTemplate* material, DescriptorSetList descriptor_sets = {});
		void bind_pipeline(const GraphicPipeline& pipeline, DescriptorSetList descriptor_sets);

		void draw(const VkDrawIndexedIndirectCommand& indirect);
		void draw(const VkDrawIndirectCommand& indirect);

		void bind_buffers(const SubBuffer<BufferUsage::IndexBit>& indices, const SubBuffer<BufferUsage::AttributeBit>& per_vertex, core::Span<SubBuffer<BufferUsage::AttributeBit>> per_instance = {});
		void bind_index_buffer(const SubBuffer<BufferUsage::IndexBit>& indices);
		void bind_attrib_buffers(const SubBuffer<BufferUsage::AttributeBit>& per_vertex, core::Span<SubBuffer<BufferUsage::AttributeBit>> per_instance = {});

		// proxies from _cmd_buffer
		CmdBufferRegion region(const char* name, const math::Vec4& color = math::Vec4());
		VkCommandBuffer vk_cmd_buffer() const;

		// Statefull stuff
		const Viewport& viewport() const;
		void set_viewport(const Viewport& vp);
		void set_scissor(const math::Vec2i& offset, const math::Vec2ui& size);

	private:
		friend class CmdBufferRecorder;

		RenderPassRecorder(CmdBufferRecorder& cmd_buffer, const Viewport& viewport);

		CmdBufferRecorder& _cmd_buffer;
		Viewport _viewport;
};

class CmdBufferRecorder : public CmdBufferBase {

	using SrcCopyBuffer = SubBuffer<BufferUsage::TransferSrcBit, MemoryType::DontCare>;
	using DstCopyBuffer = SubBuffer<BufferUsage::TransferDstBit, MemoryType::DontCare>;
	using SrcCopyImage = ImageView<ImageUsage::TransferSrcBit>;
	using DstCopyImage = ImageView<ImageUsage::TransferDstBit>;

	public:
		using DescriptorSetList = detail::DescriptorSetList;


		template<CmdBufferUsage Usage>
		CmdBufferRecorder(CmdBuffer<Usage>&& buffer) : CmdBufferRecorder(std::move(buffer), Usage) {
		}

		CmdBufferRecorder(CmdBufferRecorder&&) = default;

		~CmdBufferRecorder();

		CmdBufferRegion region(const char* name, const math::Vec4& color = math::Vec4());

		RenderPassRecorder bind_framebuffer(const Framebuffer& framebuffer);

		void dispatch(const ComputeProgram& program, const math::Vec3ui& size, DescriptorSetList descriptor_sets, const PushConstant& push_constants = PushConstant());

		void dispatch_size(const ComputeProgram& program, const math::Vec3ui& size, DescriptorSetList descriptor_sets, const PushConstant& push_constants = PushConstant());
		void dispatch_size(const ComputeProgram& program, const math::Vec2ui& size, DescriptorSetList descriptor_sets, const PushConstant& push_constants = PushConstant());

		void barriers(core::Span<BufferBarrier> buffers, core::Span<ImageBarrier> images);
		void barriers(core::Span<BufferBarrier> buffers);
		void barriers(core::Span<ImageBarrier> images);

		Y_TODO(Const all this)
		void barriered_copy(const ImageBase& src,  const ImageBase& dst);
		void copy(const SrcCopyBuffer& src, const DstCopyBuffer& dst);
		void copy(const SrcCopyImage& src,  const DstCopyImage& dst);
		void blit(const SrcCopyImage& src,  const DstCopyImage& dst);


		// never use directly, needed for internal work
		void transition_image(ImageBase& image, VkImageLayout src, VkImageLayout dst);

	protected:
		CmdBufferRecorder() = default;
		CmdBufferRecorder(CmdBufferBase&& base, CmdBufferUsage usage);

	private:
		friend class RenderPassRecorder;

		void end_renderpass();
		void check_no_renderpass() const;

		// this could be in RenderPassRecorder, but putting it here makes erroring easier
		const RenderPass* _render_pass = nullptr;
};

}

#endif // YAVE_GRAPHICS_COMMANDS_CMDBUFFERRECORDER_H
