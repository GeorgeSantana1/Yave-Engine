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
#ifndef Y_IO_WRITER_H
#define Y_IO_WRITER_H

#include <y/utils.h>
#include <y/core/Vector.h>
#include <y/core/Result.h>

namespace y {
namespace io {

class Writer : NonCopyable {
	public:
		using Result = core::Result<void, usize>;

		virtual ~Writer();

		virtual Result write(const void* data, usize bytes) = 0;
		virtual void flush() = 0;


		template<typename T>
		Result write_one(T t) {
			static_assert(std::is_trivially_copyable_v<T>, "write_one only works on trivially copyable data");
			return write(&t, sizeof(t));
		}

	protected:
		Result make_result(usize written, usize expected) const {
			if(written == expected) {
				return core::Ok();
			}
			return core::Err(written);
		}
};

}
}

#endif // Y_IO_WRITER_H