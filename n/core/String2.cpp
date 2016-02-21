/*******************************
Copyright (C) 2013-2015 gregoire ANGERAND

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
#include "String2.h"
#include <cstring>

namespace n {
namespace core {

// --------------------------------------------------- LONG ---------------------------------------------------

String2::LongData::LongData() : data(0), length(0) {
}

String2::LongData::LongData(const LongData &l) : LongData(l.data, l.length) {
}

String2::LongData::LongData(LongData &&l) : data(l.data), length(l.length) {
	l.data = 0;
}

String2::LongData::LongData(const char *str, uint len) : data(allocLong(len)), length(len) {
	if(str) {
		memcpy(data, str, len);
	}
	data[len] = 0;
}


// --------------------------------------------------- SHORT ---------------------------------------------------

String2::ShortData::ShortData() : data{0}, length(0) {
}

String2::ShortData::ShortData(const ShortData &s) {
	memcpy(this, &s, sizeof(ShortData));
}

String2::ShortData::ShortData(const char *str, uint len) : length(len) {
	if(str) {
		memcpy(data, str, len);
	}
	data[len] = 0;
}

String2::ShortData &String2::ShortData::operator=(const ShortData &s) {
	memcpy(this, &s, sizeof(ShortData));
	return *this;
}


// --------------------------------------------------- ALLOC ---------------------------------------------------

char *String2::allocLong(uint len) {
	char *a = new char[len + 1];
	return a;
}

void String2::freeLong(LongData &d) {
	delete[] d.data;
}




// --------------------------------------------------- STRING ---------------------------------------------------

String2::String2() : s(ShortData()) {
}

String2::String2(const String2 &str) {
	if(str.isLong()) {
		new(&l) LongData(str.l);
	} else {
		s = str.s;
	}
}

String2::String2(String2 &&str) {
	if(str.isLong()) {
		new(&l) LongData(std::move(str.l));
	} else {
		s = str.s;
	}
}

String2::String2(const char *str) : String2(str, strlen(str)) {
}

String2::String2(const char *str, uint len) {
	if(len <= MaxShortSize) {
		s = ShortData(str, len);
	} else {
		new(&l) LongData(str, len);
	}
}

uint String2::size() const {
	return isLong() ? l.length : s.length;
}

bool String2::isLong() const {
	return l.length.isLong;
}

char *String2::data() {
	return isLong() ? l.data : s.data;
}

const char *String2::data() const {
	return isLong() ? l.data : s.data;
}

char &String2::operator[](uint i) {
	return data()[i];
}

char String2::operator[](uint i) const {
	return data()[i];
}

String2 &String2::operator=(const String2 &str) {
	if(isLong()) {
		freeLong(l);
	}
	if(str.isLong()) {
		new(&l) LongData(str.l);
	} else {
		s = str.s;
	}
	return *this;
}

void String2::swap(String2 &&str) {
	byte tmp[sizeof(ShortData)];
	unused(tmp);
	memcpy(tmp, &str.s, sizeof(ShortData));
	memcpy(&str.s, &s, sizeof(ShortData));
	memcpy(&s, tmp, sizeof(ShortData));
}

String2 &String2::operator=(String2 &&str) {
	swap(std::move(str));
	return *this;
}

String2 String2::operator+(const String2 &rhs) const {
	uint lhsize = size();
	uint rhsize = rhs.size();
	String2 str(0, lhsize + rhsize);
	char *d = str.data();
	memcpy(d, data(), lhsize);
	memcpy(d + lhsize, rhs.data(), rhsize);
	return str;
}

bool String2::operator==(const String2 &str) const {
	uint s = str.size();
	if(s != size()) {
		return false;
	}
	const char *lhs = data();
	const char *rhs = str.data();
	for(uint i = 0; i != s; i++) {
		if(lhs[i] != rhs[i]) {
			return false;
		}
	}
	return true;
}

bool String2::operator!=(const String2 &str) const {
	return !operator==(str);
}

bool String2::operator<(const String2 &s) const {
	uint lhsize = size();
	uint rhsize = s.size();
	uint min = std::min(lhsize, rhsize);
	const char *lhs = data();
	const char *rhs = s.data();
	for(uint i = 0; i != min; i++) {
		if(lhs[i] < rhs[i]) {
			return true;
		} else if(lhs[i] > rhs[i]) {
			return false;
		}
	}
	return lhsize < rhsize;
}

bool String2::operator>(const String2 &s) const {
	uint lhsize = size();
	uint rhsize = s.size();
	uint min = std::min(lhsize, rhsize);
	const char *lhs = data();
	const char *rhs = s.data();
	for(uint i = 0; i != min; i++) {
		if(lhs[i] > rhs[i]) {
			return true;
		} else if(lhs[i] < rhs[i]) {
			return false;
		}
	}
	return lhsize > rhsize;
}

}
}
