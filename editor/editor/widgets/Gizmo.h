/*******************************
Copyright (c) 2016-2018 Grégoire Angerand

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
#ifndef EDITOR_WIDGETS_GIZMO_H
#define EDITOR_WIDGETS_GIZMO_H

#include <editor/ui/Gadget.h>

#include <yave/scene/SceneView.h>

namespace editor {

class Gizmo : public Gadget {
	public:
		Gizmo(SceneView* scene_view = nullptr);

		void set_scene_view(SceneView* scene_view);

		void set_transformable(Transformable* tr);

	private:
		void paint_ui(CmdBufferRecorder<>&, const FrameToken&) override;

		SceneView* _scene_view = nullptr;
		Transformable* _transformable = nullptr;

		u32 _dragging_mask = 0;
};

}

#endif // EDITOR_WIDGETS_GIZMO_H
