/*******************************
Copyright (c) 2016-2019 Grégoire Angerand

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

#include "UiElement.h"

#include <editor/context/EditorContext.h>

namespace editor {

UiElement::UiElement(std::string_view title) {
	set_title(title);
}

void UiElement::set_id(u64 id) {
	_id = id;
	set_title(_title);
}

void UiElement::set_title(std::string_view title) {
	core::String new_title = fmt("%##%", title, _id);
	_title_with_id = std::move(new_title);
	_title = std::string_view(_title_with_id.begin(), title.size());
}

void UiElement::refresh() {
}

bool UiElement::is_visible() const {
	return _visible;
}

std::string_view UiElement::title() const {
	return _title;
}

void UiElement::show() {
	_visible = true;
}

void UiElement::close() {
	_visible = false;
}

}
