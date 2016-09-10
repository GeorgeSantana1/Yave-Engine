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
#ifndef Y_DEFINES_H
#define Y_DEFINES_H

namespace y {

struct Nothing;
Nothing fatal(const char *msg, const char *file = nullptr, int line = 0);
bool try_failed();

}


#define Y_TODO(msg)

#define Y_TRY(expr)	do { if(!(expr)) { return y::try_failed(); } } while(false)


#ifndef __PRETTY_FUNCTION__
#define __FUNC__ __PRETTY_FUNCTION__
#endif

/****************** OS DEFINES BELOW ******************/

#ifdef __WIN32
#define Y_OS_WIN
#endif

#ifdef __WIN32__
#define Y_OS_WIN
#endif

#ifdef WIN32
#define Y_OS_WIN
#endif

#ifdef _WINDOWS
#define Y_OS_WIN
#endif



#endif // Y_DEFINES_H
