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
#ifndef EDITOR_MAINWINDOW_H
#define EDITOR_MAINWINDOW_H

#include <editor.h>

#include <yave/window/Window.h>

#include <yave/device/Instance.h>
#include <yave/device/Device.h>
#include <yave/swapchain/Swapchain.h>

#include <yave/renderers/renderers.h>

namespace editor {

class MainWindow : private Window {

	public:
		MainWindow(DebugParams params);

		void exec();

	private:
		void draw_ui();
		void render();

		void create_renderer();
		void create_swapchain();

		Instance _instance;
		Device _device;
		core::Unique<Swapchain> _swapchain;

		Renderer::Ptr<Renderer> _renderer;


};

}

#endif // EDITOR_MAINWINDOW_H
