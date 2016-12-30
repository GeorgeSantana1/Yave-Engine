/*******************************
Copyright (c) 2016-2017 Grégoire Angerand

This code is licensed under the MIT License (MIT).

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
#ifndef Y_IO_FILE_H
#define Y_IO_FILE_H

#include <y/core/String.h>

#include "Reader.h"
#include "Writer.h"

namespace y {
namespace io {

class File : public Reader, public Writer {

	Y_TODO(use exceptions for file)

	public:
		File();
		virtual ~File();

		File(File&& other);
		File& operator=(File&& other);

		static File create(const core::String& name);
		static File open(const core::String& name);

		usize size() const;
		usize remaining() const;

		virtual bool at_end() const override;

		virtual usize read(void* data, usize bytes) override;
		virtual usize read_all(core::Vector<u8>& data) override;

		virtual usize write(const void* data, usize bytes) override;
		virtual void flush() override;

	private:
		File(FILE* f);
		void swap(File& other);

		FILE* _file;
};

Y_ASSERT_SANE(File);

}
}

#endif // Y_IO_FILE_H
