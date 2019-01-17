/*******************************
Copyright (c) 2016-2018 Gr�goire Angerand

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
#ifndef YAVE_FRAMEGRAPH_FRAMEGRAPHPASSBUILDER_H
#define YAVE_FRAMEGRAPH_FRAMEGRAPHPASSBUILDER_H

#include "FrameGraphResourceToken.h"
#include "FrameGraphPass.h"

namespace yave {

class FrameGraphPassBuilder {
	public:
		void add_texture_input(FrameGraphImageId res, PipelineStage stage);

		void add_depth_output(FrameGraphMutableImageId res);
		void add_color_output(FrameGraphMutableImageId res);

		void add_copy_src(FrameGraphImageId res);

		void add_storage_output(FrameGraphMutableImageId res, usize ds_index = 0, PipelineStage stage = PipelineStage::ComputeBit);
		void add_storage_output(FrameGraphMutableBufferId res, usize ds_index = 0, PipelineStage stage = PipelineStage::ComputeBit);

		void add_storage_input(FrameGraphBufferId res, usize ds_index = 0, PipelineStage stage = PipelineStage::AllShadersBit);
		void add_storage_input(FrameGraphImageId res, usize ds_index = 0, PipelineStage stage = PipelineStage::AllShadersBit);
		void add_uniform_input(FrameGraphBufferId res, usize ds_index = 0, PipelineStage stage = PipelineStage::AllShadersBit);
		void add_uniform_input(FrameGraphImageId res, usize ds_index = 0, PipelineStage stage = PipelineStage::AllShadersBit);

		void add_uniform_input(StorageView tex, usize ds_index = 0, PipelineStage stage = PipelineStage::AllShadersBit);
		void add_uniform_input(TextureView tex, usize ds_index = 0, PipelineStage stage = PipelineStage::AllShadersBit);
		void add_uniform_input(CubemapView tex, usize ds_index = 0, PipelineStage stage = PipelineStage::AllShadersBit);

		void add_attrib_input(FrameGraphBufferId res, PipelineStage stage = PipelineStage::VertexInputBit);
		void add_index_input(FrameGraphBufferId res, PipelineStage stage = PipelineStage::VertexInputBit);

		template<typename T>
		void map_update(FrameGraphMutableTypedBufferId<T> res) {
			set_cpu_visible(res);
		}

		void set_render_func(FrameGraphPass::render_func&& func);

		void add_descriptor_binding(Binding bind, usize ds_index = 0);

	private:
		friend class FrameGraph;

		FrameGraphPassBuilder(FrameGraphPass* pass);

		void add_to_pass(FrameGraphImageId res, ImageUsage usage, PipelineStage stage);
		void add_to_pass(FrameGraphBufferId res, BufferUsage usage, PipelineStage stage);

		void add_uniform(FrameGraphDescriptorBinding binding, usize ds_index);

		void set_cpu_visible(FrameGraphMutableBufferId res);


		FrameGraphPass* _pass = nullptr;
};

}

#endif // YAVE_FRAMEGRAPH_FRAMEGRAPHBUILDER_H


