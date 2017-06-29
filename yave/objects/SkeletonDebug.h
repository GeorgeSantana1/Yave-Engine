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
#ifndef YAVE_OBJECTS_SKELETONDEBUG_H
#define YAVE_OBJECTS_SKELETONDEBUG_H

#include <yave/assets/AssetPtr.h>
#include <yave/meshs/SkinnedMesh.h>
#include <yave/animations/SkeletonInstance.h>

#include "Transformable.h"
#include "Renderable.h"

namespace yave {

class SkeletonDebug : public Renderable {

	public:
		SkeletonDebug(const AssetPtr<SkinnedMesh>& mesh) : SkeletonDebug(mesh->triangle_buffer().device(), mesh) {
		}

		SkeletonDebug(SkeletonDebug&& other);
		SkeletonDebug& operator=(SkeletonDebug&& other) = delete;

		void render(const FrameToken&, CmdBufferRecorderBase& recorder, const SceneData& scene_data) const override;

	private:
		SkeletonDebug(DevicePtr dptr, const AssetPtr<SkinnedMesh>& mesh);

		AssetPtr<SkinnedMesh> _mesh;

		TypedUniformBuffer<math::Transform<>, MemoryFlags::CpuVisible> _bone_transforms;
		DescriptorSet _descriptor_set;

		mutable AssetPtr<Material> _material;

};

}

#endif // YAVE_OBJECTS_SKELETONDEBUG_H
