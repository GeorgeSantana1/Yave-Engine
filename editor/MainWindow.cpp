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

#include "MainWindow.h"
#include "MainEventHandler.h"

#include <renderers/ImGuiRenderer.h>

#include <yave/renderers/SimpleEndOfPipe.h>
#include <yave/renderers/ToneMapper.h>
#include <yave/renderers/GBufferRenderer.h>
#include <yave/renderers/TiledDeferredRenderer.h>
#include <yave/renderers/FramebufferRenderer.h>

#include <widgets/PerformanceMetrics.h>
#include <widgets/EntityView.h>
#include <widgets/CameraDebug.h>
#include <widgets/Gizmo.h>

#include "scenes.h"

#include <y/core/SmallVector.h>

#include <imgui/imgui.h>

namespace editor {

MainWindow::MainWindow(DebugParams params) :
		Window({1280, 768}, "Yave", Window::Resizable),
		_instance(params),
		_device(_instance),
		_engine_view(&_device) {

	ImGui::CreateContext();

	auto gui = Node::Ptr<SecondaryRenderer>(new ImGuiRenderer(&_device));
	_ui_renderer = new SimpleEndOfPipe(gui);

	auto [scene, view] = create_scene(&_device);
	set_scene(std::move(scene), std::move(view));

	_widgets << new PerformanceMetrics();
	_widgets << new EntityView(_scene.as_ptr());
	_widgets << new CameraDebug(_scene_view.as_ptr());
	//_widgets << new Gizmo(_scene_view.as_ptr());

	set_event_handler(new MainEventHandler());
}

MainWindow::~MainWindow() {
	ImGui::DestroyContext();
}

void MainWindow::resized() {
	create_swapchain();
}

void MainWindow::set_scene(core::Unique<Scene>&& scene, core::Unique<SceneView>&& view) {
	_scene = std::move(scene);
	_scene_view = std::move(view);

	_engine_view.set_scene_view(_scene_view.as_ptr());
}

void MainWindow::create_swapchain() {
	// needed because the swapchain imediatly destroys it images
	_device.queue(vk::QueueFlagBits::eGraphics).wait();

	if(_swapchain) {
		_swapchain->reset();
	} else {
		_swapchain = new Swapchain(&_device, this);
	}
}

void MainWindow::exec() {
	show();

	while(update()) {

		FrameToken frame = _swapchain->next_frame();
		CmdBufferRecorder<> recorder(_device.create_cmd_buffer());

		render(recorder, frame);
		present(recorder, frame);
	}

	_device.queue(QueueFamily::Graphics).wait();
}

void MainWindow::render(CmdBufferRecorder<>& recorder, const FrameToken& token) {
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(size());

	// draw ui
	ImGui::NewFrame();
	{
		ImU32 flags = ImGuiWindowFlags_NoTitleBar |
					  ImGuiWindowFlags_NoResize |
					  ImGuiWindowFlags_NoScrollbar |
					  ImGuiWindowFlags_NoInputs |
					  ImGuiWindowFlags_NoSavedSettings |
					  ImGuiWindowFlags_NoFocusOnAppearing |
					  ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::Begin("Main window", nullptr, flags);
		ImGui::BeginDockspace();

		{
			_engine_view.render_ui(recorder, token);

			ImGui::SetNextDock(ImGuiDockSlot_Left);
			{
				for(auto& w : _widgets) {
					w->paint();
				}
			}

			ImGui::SetNextDock(ImGuiDockSlot_Bottom);
			ImGui::BeginDock("test 1");
			ImGui::EndDock();

			ImGui::BeginDock("test 2");
			ImGui::EndDock();

			ImGui::BeginDock("test 3");
			ImGui::EndDock();
		}

		ImGui::EndDockspace();
		ImGui::End();
	}
	ImGui::EndFrame();
	ImGui::Render();

	// render ui pipeline into cmd buffer
	{
		RenderingPipeline pipeline(_ui_renderer);
		pipeline.render(recorder, token);
	}

}

void MainWindow::present(CmdBufferRecorder<>& recorder, const FrameToken& token) {


	RecordedCmdBuffer<> cmd_buffer(std::move(recorder));

	vk::PipelineStageFlags pipe_stage_flags = vk::PipelineStageFlagBits::eBottomOfPipe;
	auto graphic_queue = _device.queue(QueueFamily::Graphics).vk_queue();
	auto vk_buffer = cmd_buffer.vk_cmd_buffer();

	graphic_queue.submit(vk::SubmitInfo()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&token.image_aquired)
			.setPWaitDstStageMask(&pipe_stage_flags)
			.setCommandBufferCount(1)
			.setPCommandBuffers(&vk_buffer)
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&token.render_finished),
		cmd_buffer.vk_fence());

	_swapchain->present(token, graphic_queue);
}

}
