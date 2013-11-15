//
// LogFile_WIN32U.cpp
//
// $Id: //poco/1.4/Foundation/src/LogFile_WIN32U.cpp#1 $
//
// Library: Foundation
// Package: Logging
// Module:  LogFile
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
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


#include "Poco/LogFile_WIN32U.h"
#include "Poco/File.h"
#include "Poco/Exception.h"
#include "Poco/UnicodeConverter.h"


namespace Poco {


LogFileImpl::LogFileImpl(const std::string& path): _path(path), _hFile(INVALID_HANDLE_VALUE)
{
	File file(path);
	if (file.exists())
	{
		if (0 == sizeImpl())
			_creationDate = file.getLastModified();
		else
			_creationDate = file.created();
	}
}


LogFileImpl::~LogFileImpl()
{
	CloseHandle(_hFile);
}


void LogFileImpl::writeImpl(const std::string& text, bool flush)
{
	if (INVALID_HANDLE_VALUE == _hFile)	createFile();

	DWORD bytesWritten;
	BOOL res = WriteFile(_hFile, text.data(), (DWORD) text.size(), &bytesWritten, NULL);
	if (!res) throw WriteFileException(_path);
	res = WriteFile(_hFile, "\r\n", 2, &bytesWritten, NULL);
	if (!res) throw WriteFileException(_path);
	if (flush)
	{
		res = FlushFileBuffers(_hFile);
		if (!res) throw WriteFileException(_path);
	}
}


UInt64 LogFileImpl::sizeImpl() const
{
	if (INVALID_HANDLE_VALUE == _hFile)
	{
		File file(_path);
		if (file.exists()) return file.getSize();
		else return 0;
	}

	LARGE_INTEGER li;
	li.HighPart = 0;
	LARGE_INTEGER distance;
	memset(&distance, 0, sizeof(LARGE_INTEGER));

	li.LowPart  = SetFilePointerEx( _hFile, distance, &li, FILE_CURRENT);
	return li.QuadPart;
}


Timestamp LogFileImpl::creationDateImpl() const
{
	return _creationDate;
}


const std::string& LogFileImpl::pathImpl() const
{
	return _path;
}


void LogFileImpl::createFile()
{
	std::wstring upath;
	UnicodeConverter::toUTF16(_path, upath);
	
	CREATEFILE2_EXTENDED_PARAMETERS params; // TODO - Anything to put in here
	memset(&params, 0, sizeof(CREATEFILE2_EXTENDED_PARAMETERS));

	_hFile = CreateFile2(upath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS, &params);
	
	if (_hFile == INVALID_HANDLE_VALUE) throw OpenFileException(_path);
	LARGE_INTEGER distance;
	memset(&distance, 0, sizeof(LARGE_INTEGER));
	SetFilePointerEx(_hFile, distance, nullptr, FILE_END);
	// There seems to be a strange "optimization" in the Windows NTFS
	// filesystem that causes it to reuse directory entries of deleted
	// files. Example:
	// 1. create a file named "test.dat"
	//    note the file's creation date
	// 2. delete the file "test.dat"
	// 3. wait a few seconds
	// 4. create a file named "test.dat"
	//    the new file will have the same creation
	//    date as the old one.
	// We work around this bug by taking the file's
	// modification date as a reference when the
	// file is empty.
	if (sizeImpl() == 0)
		_creationDate = File(_path).getLastModified();
	else
		_creationDate = File(_path).created();
}


} // namespace Poco
