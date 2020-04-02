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

#include "Device.h"
#include "PhysicalDevice.h"

#include <y/concurrent/concurrent.h>

#include <yave/graphics/commands/CmdBufferBase.h>

#include <y/utils/log.h>
#include <y/utils/format.h>

#include <mutex>

namespace yave {

static void check_features(const vk::PhysicalDeviceFeatures& features, const vk::PhysicalDeviceFeatures& required) {
	const auto feats = reinterpret_cast<const vk::Bool32*>(&features);
	const auto req = reinterpret_cast<const vk::Bool32*>(&required);
	for(usize i = 0; i != sizeof(features) / sizeof(vk::Bool32); ++i) {
		if(req[i] && !feats[i]) {
			y_fatal("Required Vulkan feature not supported");
		}
	}
}

static std::array<const char*, 1> extensions() {
	return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

static vk::Device create_device(
		const vk::PhysicalDevice physical,
		const core::Span<QueueFamily> queue_families,
		const DebugParams& debug) {

	y_profile();

	auto queue_create_infos = core::vector_with_capacity<vk::DeviceQueueCreateInfo>(queue_families.size());

	const auto prio_count = std::max_element(queue_families.begin(), queue_families.end(),
			[](const auto& a, const auto& b) { return a.count() < b.count(); })->count();
	const core::Vector<float> priorities(prio_count, 1.0f);
	std::transform(queue_families.begin(), queue_families.end(), std::back_inserter(queue_create_infos), [&](const auto& q) {
		return vk::DeviceQueueCreateInfo()
				.setQueueFamilyIndex(q.index())
				.setPQueuePriorities(priorities.data())
				.setQueueCount(q.count())
			;
		});

	auto required = vk::PhysicalDeviceFeatures();
	required.multiDrawIndirect = true;
	required.geometryShader = true;
	required.drawIndirectFirstInstance = true;
	required.fullDrawIndexUint32 = true;
	required.textureCompressionBC = true;
	required.shaderStorageImageExtendedFormats = true;
	required.shaderUniformBufferArrayDynamicIndexing = true;
	required.shaderSampledImageArrayDynamicIndexing = true;
	required.shaderStorageBufferArrayDynamicIndexing = true;
	required.shaderStorageImageArrayDynamicIndexing = true;
	required.fragmentStoresAndAtomics = true;

	if(debug.debug_features_enabled()) {
		required.robustBufferAccess = true;
	}

	check_features(physical.getFeatures(), required);

	auto exts = extensions();

	y_profile_zone("physical device");
	return physical.createDevice(vk::DeviceCreateInfo()
			.setEnabledExtensionCount(u32(exts.size()))
			.setPpEnabledExtensionNames(exts.data())
			.setEnabledLayerCount(u32(debug.device_layers().size()))
			.setPpEnabledLayerNames(debug.device_layers().begin())
			.setQueueCreateInfoCount(u32(queue_create_infos.size()))
			.setPQueueCreateInfos(queue_create_infos.begin())
			.setPEnabledFeatures(&required)
		);
}


Device::ScopedDevice::~ScopedDevice() {
	device.destroy();
}


static core::Vector<Queue> create_queues(DevicePtr dptr, core::Vector<QueueFamily> families) {
	core::Vector<Queue> queues;
	for(const auto& family : families) {
		for(auto& queue : family.queues(dptr)) {
			queues.push_back(std::move(queue));
		}
	}
	return queues;
}

Device::Device(Instance& instance) :
		_instance(instance),
		_physical(instance),
		_queue_families(QueueFamily::all(_physical)),
		_device{create_device(_physical.vk_physical_device(), _queue_families, _instance.debug_params())},
		_allocator(this),
		_lifetime_manager(this),
		_queues(create_queues(this, _queue_families)),
		_sampler(this),
		_descriptor_set_allocator(this),
		_resources(this) {
}

Device::~Device() {
	Y_TODO(Why do we need this?)
	{
		CmdBufferPool<CmdBufferUsage::Disposable> pool(this);
		CmdBufferRecorder rec = pool.create_buffer();
		graphic_queue().submit<SyncSubmit>(RecordedCmdBuffer(std::move(rec)));
	}

	wait_all_queues();
	_thread_devices.clear();
	_lifetime_manager.collect();
}

const PhysicalDevice& Device::physical_device() const {
	return _physical;
}

const Instance &Device::instance() const {
	return _instance;
}

DeviceMemoryAllocator& Device::allocator() const {
	return _allocator;
}

DescriptorSetAllocator& Device::descriptor_set_allocator() const {
	return _descriptor_set_allocator;
}

const QueueFamily& Device::queue_family(vk::QueueFlags flags) const {
	for(const auto& q : _queue_families) {
		if((q.flags() & flags) == flags) {
			return q;
		}
	}
	/*return*/ y_fatal("Unable to find queue.");
}

const Queue& Device::graphic_queue() const {
	return _queues.first();
}

Queue& Device::graphic_queue() {
	return _queues.first();
}

void Device::wait_all_queues() const {
	y_profile();
	vk_device().waitIdle();
}

ThreadDevicePtr Device::thread_device() const {
	static thread_local usize thread_id = concurrent::thread_id();
	static thread_local std::pair<DevicePtr, ThreadDevicePtr> thread_cache;

	auto& cache = thread_cache;
	if(cache.first != this) {
		const auto lock = y_profile_unique_lock(_lock);
		while(_thread_devices.size() <= thread_id) {
			_thread_devices.emplace_back();
		}
		auto& data = _thread_devices[thread_id];
		if(!data) {
			data = std::make_unique<ThreadLocalDevice>(this);
			if(_thread_devices.size() > 64) {
				log_msg("64 ThreadLocalDevice have been created.", Log::Warning);
			}
		}
		cache = {this, data.get()};
		return data.get();
	}
	return cache.second;
}

const DeviceResources& Device::device_resources() const {
	return _resources;
}

DeviceResources& Device::device_resources() {
	return _resources;
}

LifetimeManager& Device::lifetime_manager() const {
	return _lifetime_manager;
}

const vk::PhysicalDeviceLimits& Device::vk_limits() const {
	return _physical.vk_properties().limits;
}

vk::Device Device::vk_device() const {
	return _device.device;
}

vk::Sampler Device::vk_sampler() const {
	return _sampler.vk_sampler();
}

CmdBuffer<CmdBufferUsage::Disposable> Device::create_disposable_cmd_buffer() const {
	return thread_device()->create_disposable_cmd_buffer();
}

const DebugUtils* Device::debug_utils() const {
	return _instance.debug_utils();
}



}
