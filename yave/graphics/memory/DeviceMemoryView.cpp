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

#include "DeviceMemoryView.h"
#include "DeviceMemoryHeap.h"

#include <yave/device/Device.h>

namespace yave {

DeviceMemoryView::DeviceMemoryView(const DeviceMemory& mem) :
		DeviceLinked(mem.device()),
		_heap(mem.heap()),
		_memory(mem.vk_memory()),
		_offset(mem.vk_offset()) {
}

vk::MappedMemoryRange DeviceMemoryView::vk_mapped_range(usize size, usize offset) const {
	usize atom_size = device()->vk_limits().nonCoherentAtomSize;

	usize new_offset = _offset + offset;
	usize offset_adjust = new_offset % atom_size;

	usize new_size = size + offset_adjust;
	usize size_mod = size % atom_size;
	usize size_adjust = size_mod ? atom_size - size_mod : 0;

	return vk::MappedMemoryRange(_memory, new_offset - offset_adjust, new_size + size_adjust);
}

vk::DeviceMemory DeviceMemoryView::vk_memory() const {
	return _memory;
}

usize DeviceMemoryView::vk_offset() const {
	return _offset;
}

void* DeviceMemoryView::map() {
	return _heap->map(*this);
}

void DeviceMemoryView::unmap() {
	_heap->unmap(*this);
}

}
