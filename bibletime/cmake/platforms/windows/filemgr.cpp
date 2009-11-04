/******************************************************************************
 *  filemgr.cpp	- implementation of class FileMgr used for pooling file
 *  					handles
 *
 * $Id: filemgr.cpp 2245 2009-02-10 23:22:28Z scribe $
 *
 * Copyright 1998 CrossWire Bible Society (http://www.crosswire.org)
 *	CrossWire Bible Society
 *	P. O. Box 2528
 *	Tempe, AZ  85280-2528
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#include <filemgr.h>
#include <utilstr.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <swbuf.h>
#if !defined(__GNUC__) && !defined(_WIN32_WCE)
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif


#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef S_IRGRP
#define S_IRGRP 0
#endif

#ifndef S_IROTH
#define S_IROTH 0
#endif

// Fix for VC6
#ifndef S_IREAD
#ifdef _S_IREAD
#define S_IREAD _S_IREAD
#define S_IWRITE _S_IWRITE
#endif
#endif
// -----------


SWORD_NAMESPACE_START


int FileMgr::CREAT = O_CREAT;
int FileMgr::APPEND = O_APPEND;
int FileMgr::TRUNC = O_TRUNC;
int FileMgr::RDONLY = O_RDONLY;
int FileMgr::RDWR = O_RDWR;
int FileMgr::WRONLY = O_WRONLY;
int FileMgr::IREAD = S_IREAD;
int FileMgr::IWRITE = S_IWRITE;


// ---------------- statics -----------------
FileMgr *FileMgr::systemFileMgr = 0;

class __staticsystemFileMgr {
public:
	__staticsystemFileMgr() { }
	~__staticsystemFileMgr() { delete FileMgr::systemFileMgr; }
} _staticsystemFileMgr;


FileMgr *FileMgr::getSystemFileMgr() {
	if (!systemFileMgr)
		systemFileMgr = new FileMgr();

	return systemFileMgr;
}


void FileMgr::setSystemFileMgr(FileMgr *newFileMgr) {
	if (systemFileMgr)
		delete systemFileMgr;
	systemFileMgr = newFileMgr;
}

// --------------- end statics --------------


FileDesc::FileDesc(FileMgr *parent, const char *path, int mode, int perms, bool tryDowngrade) {
	this->parent = parent;
	this->path = 0;
	stdstr(&this->path, path);
	this->mode = mode;
	this->perms = perms;
	this->tryDowngrade = tryDowngrade;
	offset = 0;
	fd = -77;
}


FileDesc::~FileDesc() {
	if (fd > 0)
		close(fd);
		
	if (path)
		delete [] path;
}


int FileDesc::getFd() {
	if (fd == -77)
		fd = parent->sysOpen(this);
//	if ((fd < -1) && (fd != -77))  // kludge to hand ce
//		return 777;
	return fd;
}


long FileDesc::seek(long offset, int whence) {
	return lseek(getFd(), offset, whence);
}


long FileDesc::read(void *buf, long count) {
	int fd = getFd();
	if (fd < 0) {
		return 0;
	}
	return ::read(fd, buf, count);
}


long FileDesc::write(const void *buf, long count) {
	return ::write(getFd(), buf, count);
}


FileMgr::FileMgr(int maxFiles) {
	this->maxFiles = maxFiles;		// must be at least 2
	files = 0;
}


FileMgr::~FileMgr() {
	FileDesc *tmp;
	
	while(files) {
		tmp = files->next;
		delete files;
		files = tmp;
	}
}


FileDesc *FileMgr::open(const char *path, int mode, bool tryDowngrade) {
	return open(path, mode, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH, tryDowngrade);
}


FileDesc *FileMgr::open(const char *path, int mode, int perms, bool tryDowngrade) {
	FileDesc **tmp, *tmp2;
	
	for (tmp = &files; *tmp; tmp = &((*tmp)->next)) {
		if ((*tmp)->fd < 0)		// insert as first non-system_open file
			break;
	}

	tmp2 = new FileDesc(this, path, mode, perms, tryDowngrade);
	tmp2->next = *tmp;
	*tmp = tmp2;
	
	return tmp2;
}


void FileMgr::close(FileDesc *file) {
	FileDesc **loop;
	
	for (loop = &files; *loop; loop = &((*loop)->next)) {
		if (*loop == file) {
			*loop = (*loop)->next;
			delete file;
			break;
		}
	}
}


int FileMgr::sysOpen(FileDesc *file) {
	FileDesc **loop;
	int openCount = 1;		// because we are presently opening 1 file, and we need to be sure to close files to accomodate, if necessary
	
	for (loop = &files; *loop; loop = &((*loop)->next)) {

		if ((*loop)->fd > 0) {
			if (++openCount > maxFiles) {
				(*loop)->offset = lseek((*loop)->fd, 0, SEEK_CUR);
				::close((*loop)->fd);
				(*loop)->fd = -77;
			}
		}

		if (*loop == file) {
			if (*loop != files) {
				*loop = (*loop)->next;
				file->next = files;
				files = file;
			}
			if ((!access(file->path, 04)) || ((file->mode & O_CREAT) == O_CREAT)) {	// check for at least file exists / read access before we try to open
				char tries = (((file->mode & O_RDWR) == O_RDWR) && (file->tryDowngrade)) ? 2 : 1;  // try read/write if possible
				for (int i = 0; i < tries; i++) {
					if (i > 0) {
						file->mode = (file->mode & ~O_RDWR);	// remove write access
						file->mode = (file->mode | O_RDONLY);// add read access
					}
					file->fd = ::open(file->path, file->mode|O_BINARY, file->perms);

					if (file->fd >= 0)
						break;
				}

				if (file->fd >= 0)
					lseek(file->fd, file->offset, SEEK_SET);
			}
			else file->fd = -1;
			if (!*loop)
				break;
		}
	}
	return file->fd;
}


// to truncate a file at its current position
// leaving byte at current possition intact
// deleting everything afterward.
signed char FileMgr::trunc(FileDesc *file) {

	static const char *writeTest = "x";
	long size = file->seek(1, SEEK_CUR);
	if (size == 1) // was empty
		size = 0;
	char nibble [ 32767 ];
	bool writable = file->write(writeTest, 1);
	int bytes = 0;

	if (writable) {
		// get tmpfilename
		char *buf = new char [ strlen(file->path) + 10 ];
		int i;
		for (i = 0; i < 9999; i++) {
			sprintf(buf, "%stmp%.4d", file->path, i);
			if (!existsFile(buf))
				break;
		}
		if (i == 9999)
			return -2;

		int fd = ::open(buf, O_CREAT|O_RDWR, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
		if (fd < 0)
			return -3;
	
		file->seek(0, SEEK_SET);
		while (size > 0) {	 
			bytes = file->read(nibble, 32767);
			write(fd, nibble, (bytes < size)?bytes:size);
			size -= bytes;
		}
		// zero out the file
		::close(file->fd);
		file->fd = ::open(file->path, O_TRUNC, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
		::close(file->fd);
		file->fd = -77;	// force file open by filemgr
		// copy tmp file back (dumb, but must preserve file permissions)
		lseek(fd, 0, SEEK_SET);
		do {
			bytes = read(fd, nibble, 32767);
			file->write(nibble, bytes);
		} while (bytes == 32767);
		
		::close(fd);
		::close(file->fd);
		removeFile(buf);		// remove our tmp file
		file->fd = -77;	// causes file to be swapped out forcing open on next call to getFd()
	}
	else { // put offset back and return failure
		file->seek(-1, SEEK_CUR);
		return -1;
	}
	return 0;
}


signed char FileMgr::existsFile(const char *ipath, const char *ifileName)
{
	int len = strlen(ipath) + ((ifileName)?strlen(ifileName):0) + 3;
	char *ch;
	char *path = new char [ len ];
	strcpy(path, ipath);
	
	if ((path[strlen(path)-1] == '\\') || (path[strlen(path)-1] == '/'))
		path[strlen(path)-1] = 0;
	
	if (ifileName) {
		ch = path + strlen(path);
		sprintf(ch, "/%s", ifileName);
	}
	signed char retVal = !access(path, 04);
	delete [] path;
	return retVal;
}


signed char FileMgr::existsDir(const char *ipath, const char *idirName)
{
	char *ch;
	int len = strlen(ipath) + ((idirName)?strlen(idirName):0) + 1;
	if (idirName)
		len +=  strlen(idirName);
	char *path = new char [ len ];
	strcpy(path, ipath);
	
	if ((path[strlen(path)-1] == '\\') || (path[strlen(path)-1] == '/'))
		path[strlen(path)-1] = 0;
	
	if (idirName) {
		ch = path + strlen(path);
		sprintf(ch, "/%s", idirName);
	}
	signed char retVal = !access(path, 04);
	delete [] path;
	return retVal;
}


int FileMgr::createParent(const char *pName) {
	char *buf = new char [ strlen(pName) + 1 ];
	int retCode = 0;
	
	strcpy(buf, pName);
	int end = strlen(buf) - 1;
	while (end) {
		if ((buf[end] == '/') || (buf[end] == '\\'))
			break;
		end--;
	}
	buf[end] = 0;
	if (strlen(buf)>0) {
		if (access(buf, 02)) {  // not exists with write access?
			if ((retCode = mkdir(buf
#ifndef WIN32
					, 0755
#endif
					))) {
				createParent(buf);
				retCode = mkdir(buf
#ifndef WIN32
					, 0755
#endif
					);
			}
		}
	}
	else retCode = -1;
	delete [] buf;
	return retCode;
}
	

int FileMgr::openFileReadOnly(const char *fName) {
	int fd = ::open(fName, O_RDONLY|O_BINARY, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
	return fd;
}


int FileMgr::createPathAndFile(const char *fName) {
	int fd;
	
	fd = ::open(fName, O_CREAT|O_WRONLY|O_BINARY, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
	if (fd < 1) {
		createParent(fName);
		fd = ::open(fName, O_CREAT|O_WRONLY|O_BINARY, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
	}
	return fd;
}


int FileMgr::copyFile(const char *sourceFile, const char *targetFile) {
	int sfd, dfd, len;
	char buf[4096];

	if ((sfd = ::open(sourceFile, O_RDONLY|O_BINARY, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH)) < 1)
		return -1;
	if ((dfd = createPathAndFile(targetFile)) < 1)
		return -1;

	do {
		len = read(sfd, buf, 4096);
		write(dfd, buf, len);
	}
	while(len == 4096);	
	::close(dfd);
	::close(sfd);
	
	return 0;
}


int FileMgr::removeFile(const char *fName) {
	return ::remove(fName);
}

char FileMgr::getLine(FileDesc *fDesc, SWBuf &line) {
	int len;
	bool more = true;
	char chunk[255];

	line = "";

	// assert we have a valid file handle
	if (fDesc->getFd() < 1)
		return 0;

	while (more) {
		more = false;
		long index = fDesc->seek(0, SEEK_CUR);
		len = fDesc->read(chunk, 254);

		// assert we have a readable file (not a directory)
		if (len < 1)
			break;

		int start = 0;
		// clean up any preceding white space if we're at the beginning of line
		if (!line.length()) {
			for (;start < len; start++) {
				if ((chunk[start] != 13) && (chunk[start] != ' ') && (chunk[start] != '\t'))
					break;
			}
		}

		// find the end
		int end;
		for (end = start; ((end < (len-1)) && (chunk[end] != 10)); end++);
	
		if ((chunk[end] != 10) && (len == 254)) {
			more = true;
		}
		index += (end + 1);

		// reposition to next valid place to read
		fDesc->seek(index, SEEK_SET);

		// clean up any trailing junk on line if we're at the end
		if (!more) {
			for (; end > start; end--) {
				if ((chunk[end] != 10) && (chunk[end] != 13) && (chunk[end] != ' ') && (chunk[end] != '\t')) {
					if (chunk[end] == '\\') {
						more = true;
						end--;
					}
					break;
				}
			}
		}
		
		int size = (end - start) + 1;

		if (size > 0) {
			// line.appendFormatted("%.*s", size, chunk+start);
			line.append(chunk+start, size);
		}
	}
	return ((len > 0) || line.length());
}


char FileMgr::isDirectory(const char *path) {
	struct stat stats;
	if (stat(path, &stats))
		return 0;
	return ((stats.st_mode & S_IFDIR) == S_IFDIR);
}


int FileMgr::copyDir(const char *srcDir, const char *destDir) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(srcDir))) {
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
				SWBuf srcPath  = (SWBuf)srcDir  + (SWBuf)"/" + ent->d_name;
				SWBuf destPath = (SWBuf)destDir + (SWBuf)"/" + ent->d_name;
				if (!isDirectory(srcPath.c_str())) {
					copyFile(srcPath.c_str(), destPath.c_str());
				}
				else {
					copyDir(srcPath.c_str(), destPath.c_str());
				}
			}
		}
		closedir(dir);
	}
	return 0;
}


int FileMgr::removeDir(const char *targetDir) {
	DIR *dir = opendir(targetDir);
	struct dirent *ent;
	if (dir) {
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
				SWBuf targetPath = (SWBuf)targetDir + (SWBuf)"/" + ent->d_name;
				if (!isDirectory(targetPath.c_str())) {
					FileMgr::removeFile(targetPath.c_str());
				}
				else {
					removeDir(targetPath.c_str());
				}
			}
		}
		closedir(dir);
		removeFile(targetDir);
	}
	return 0;
}


void FileMgr::flush() {
	FileDesc **loop;
	
	for (loop = &files; *loop; loop = &((*loop)->next)) {
		if ((*loop)->fd > 0) {
			(*loop)->offset = lseek((*loop)->fd, 0, SEEK_CUR);
			::close((*loop)->fd);
			(*loop)->fd = -77;
		}
	}
}

long FileMgr::resourceConsumption() {
	long count = 0;
	FileDesc **loop;
	for (loop = &files; *loop; loop = &((*loop)->next)) {
		if ((*loop)->fd > 0) {
			count++;
		}
	}
	return count;
}


SWORD_NAMESPACE_END
