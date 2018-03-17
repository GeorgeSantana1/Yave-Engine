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

#include "ToneMapper.h"
#include "TiledDeferredRenderer.h"

#include <y/io/File.h>

namespace yave {

static TextureView renderer_texture(const Renderer::Ptr<Renderer>& renderer) {
#warning assuming TiledDeferredRenderer !!!
	auto tiled = dynamic_cast<TiledDeferredRenderer*>(renderer.as_ptr());
	if(!tiled) {
		fatal("Unsupported renderer type.");
	}
	return tiled->lighting();
}

ToneMapper::ToneMapper(const Ptr<Renderer>& renderer) :
		SecondaryRenderer(renderer->device()),
		_renderer(renderer),
		_material(device(), MaterialData()
			.set_frag_data(SpirVData::from_file(io::File::open("tonemap.frag.spv").expected("Unable to load spirv file.")))
			.set_vert_data(SpirVData::from_file(io::File::open("screen.vert.spv").expected("Unable to load spirv file.")))
			.set_bindings({Binding(renderer_texture(renderer))})
			.set_depth_tested(false)
		) {
}

void ToneMapper::build_frame_graph(FrameGraphNode& frame_graph) {
	frame_graph.schedule(_renderer);
}

void ToneMapper::render(RenderPassRecorder& recorder, const FrameToken&) {
	auto region = recorder.region("ToneMapper::render");

	recorder.bind_material(_material);
	recorder.draw(vk::DrawIndirectCommand(6, 1));
}

}
