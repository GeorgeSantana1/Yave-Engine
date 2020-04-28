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

#include "Sampler.h"

#include <yave/device/Device.h>

namespace yave {

static vk::SamplerAddressMode vk_address_mode(Sampler::Type type) {
	switch(type) {
		case Sampler::Repeat:
			return vk::SamplerAddressMode::eRepeat;

		case Sampler::Clamp:
			return vk::SamplerAddressMode::eClampToEdge;

		default:
			y_fatal("Unknown sampler type");
	}
}

static vk::Sampler create_sampler(DevicePtr dptr, Sampler::Type type) {
	const vk::SamplerAddressMode address_mode = vk_address_mode(type);

	return dptr->vk_device().createSampler(vk::SamplerCreateInfo()
			.setMagFilter(vk::Filter::eLinear)
			.setMinFilter(vk::Filter::eLinear)
			.setAddressModeU(address_mode)
			.setAddressModeV(address_mode)
			.setAddressModeW(address_mode)
			.setMipmapMode(vk::SamplerMipmapMode::eLinear)
			.setMinLod(0.0f)
			.setMaxLod(1000.0f)
			.setMaxAnisotropy(1.0f)
			.setCompareEnable(false)
		);
}

Sampler::Sampler(DevicePtr dptr, Type type) : DeviceLinked(dptr), _sampler(create_sampler(dptr, type)) {
}

Sampler::~Sampler() {
	destroy(_sampler);
}

vk::Sampler Sampler::vk_sampler() const {
	return _sampler;
}


}
