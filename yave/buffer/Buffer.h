/*******************************
Copyright (C) 2013-2016 gregoire ANGERAND

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************/
#ifndef YAVE_BUFFER_BUFFER_H
#define YAVE_BUFFER_BUFFER_H

#include "BufferUsage.h"
#include "BufferBase.h"

namespace yave {

template<BufferUsage Usage, MemoryFlags Flags = prefered_memory_flags<Usage>(), BufferTransfer Transfer = prefered_transfer<Flags>()>
class Buffer : public BufferBase {

	static_assert(Usage != BufferUsage::None || Transfer != BufferTransfer::None, "Buffers should not have Usage == BufferUsage::None");

	public:
		Buffer() = default;

		Buffer(DevicePtr dptr, usize byte_size) : BufferBase(dptr, byte_size, Usage, Flags, Transfer) {
		}

		Buffer(Buffer&& other) : BufferBase() {
			swap(other);
		}

		Buffer& operator=(Buffer&& other) {
			swap(other);
			return *this;
		}
};


}

#endif // YAVE_BUFFER_BUFFER_H