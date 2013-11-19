//
// DirectoryIterator_WIN32U.cpp
//
// $Id: //poco/1.4/Foundation/src/DirectoryIterator_WIN32U.cpp#1 $
//
// Library: Foundation
// Package: Filesystem
// Module:  DirectoryIterator
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/DirectoryIterator_WINRT.h"
#include "Poco/File_WINRT.h"
#include "Poco/Path.h"
#include "Poco/UnicodeConverter.h"
#include <cstring>


namespace Poco {


DirectoryIteratorImpl::DirectoryIteratorImpl(const std::string& path): _fh(INVALID_HANDLE_VALUE), _rc(1)
{
	Path p(path);
	p.makeDirectory();
	std::string findPath = p.toString();
	findPath.append("*");
	std::wstring uFindPath;
	UnicodeConverter::toUTF16(findPath, uFindPath);

	_fh = FindFirstFileEx(uFindPath.c_str(), FindExInfoStandard, &_fd, FindExSearchNameMatch, nullptr, 0);
	
	if (_fh == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() != ERROR_NO_MORE_FILES)
			File::handleLastError(path);
	}
	else
	{
		UnicodeConverter::toUTF8(_fd.cFileName, _current);
		if (_current == "." || _current == "..")	
			next();
	}
}


DirectoryIteratorImpl::~DirectoryIteratorImpl()
{
	if (_fh != INVALID_HANDLE_VALUE)
		FindClose(_fh);
}


const std::string& DirectoryIteratorImpl::next()
{
	do
	{
		_current.clear();
		if (FindNextFileW(_fh, &_fd) != 0)
		{
			UnicodeConverter::toUTF8(_fd.cFileName, _current);
		}
	}
	while (_current == "." || _current == "..");
	return _current;
}


} // namespace Poco
