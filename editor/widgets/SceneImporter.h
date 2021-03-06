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
#ifndef EDITOR_WIDGETS_SCENEIMPORTER_H
#define EDITOR_WIDGETS_SCENEIMPORTER_H

#include "FileBrowser.h"

#include <editor/import/import.h>

#include <future>

namespace editor {

class SceneImporter final : public Widget, public ContextLinked {

	enum class State {
		Browsing,
		Settings,
		Importing,
		Done,
	};

	public:
		SceneImporter(ContextPtr ctx, const core::String& import_path = ".");

	private:
		bool can_destroy() const override;
		bool done_loading() const;

		void paint_ui(CmdBufferRecorder&recorder, const FrameToken&token) override;
		void paint_import_settings();

		void import(import::SceneData scene);

		State _state = State::Browsing;

		FileBrowser _browser;

		core::String _import_path;
		core::String _filename;

		import::SceneImportFlags _flags = import::SceneImportFlags::ImportAll;

		usize _forward_axis = 0;
		usize _up_axis = 4;

		float _scale = 1.0f;

		std::future<void> _import_future;
};

}

#endif // EDITOR_WIDGETS_SCENEIMPORTER_H
