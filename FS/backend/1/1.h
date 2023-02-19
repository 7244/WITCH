#pragma once

#include _WITCH_PATH(MEM/MEM.h)

#include _WITCH_PATH(IO/IO.h)

#include <direct.h>
#ifndef S_ISDIR
	#define S_ISDIR(m) \
		(((m) & _S_IFDIR) == _S_IFDIR)
#endif

typedef IO_size_t FS_size_t;
typedef IO_ssize_t FS_ssize_t;
typedef IO_off_t FS_off_t;

/* will be used in temp file functions */
#include _WITCH_PATH(STR/ttcc.h)
#include _WITCH_PATH(VEC/VEC.h)

typedef struct{
	IO_dirfd_t dirfd;
}FS_dir_t;

sint32_t _FS_dir_creat(const void *path){
	sint32_t err = _mkdir((const char *)path);
	if(err < 0){
		return -errno;
	}
	return 0;
}

sint32_t FS_dir_open(const void *path, FS_dir_t *dir, uint32_t flag){
	uintptr_t npath = MEM_cstreu(path);
	if(npath >= PATH_MAX){
		return -ENAMETOOLONG;
	}

	if(flag & O_CREAT){
		uint8_t tname[PATH_MAX];
		uintptr_t ipath = 0;
		for(; ipath < npath; ipath++){
			if(((uint8_t *)path)[ipath] != '/' && ipath + 1 != npath){
				continue;
			}
			ipath++;
			MEM_copy(path, tname, ipath);
			tname[ipath] = 0;
			IO_stat_t s;
			sint32_t err = IO_stat(tname, &s);
			if(err){
				if(err != -ENOENT){
					return -1;
				}
			}
			if(err == 0){
				if(S_ISDIR(s.st_mode)){
					continue;
				}
				return -ENOTDIR;
			}
			err = _FS_dir_creat(tname);
			if(err){
				return err;
			}
		}
	}

	dir->dirfd.handle = CreateFile((LPCSTR)path, 0, 0, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	if(dir->dirfd.handle == INVALID_HANDLE_VALUE){
		return -1;
	}
	return 0;
}

sint32_t FS_dir_close(FS_dir_t *dir){
	return 0;
}

typedef struct{
	FS_dir_t *dir;
	const char *name;
	uint64_t size;
	#if defined(WOS_WINDOWS)
		WIN32_FIND_DATA _fdFile;
		HANDLE _hFind;
		uint8_t _fr;
	#endif
}FS_dir_traverse_t;
uint8_t _FS_dir_traverse(FS_dir_t *dir, FS_dir_traverse_t *arg){
	arg->dir = dir;
	uint8_t ppath[PATH_MAX + 1];
	if(IO_getpath(&arg->dir->dirfd, ppath)){
		return 1;
	}
	char spath[PATH_MAX + 1];
	sprintf(spath, "%s\\*.*", (const char *)ppath);
	if((arg->_hFind = FindFirstFile(spath, &arg->_fdFile)) == INVALID_HANDLE_VALUE){
		return 2;
	}
	arg->_fr = 0;
	return 0;
}
uint8_t FS_dir_traverse(FS_dir_traverse_t *arg){
	if(!arg->_fr){
		arg->_fr = 1;
		arg->name = arg->_fdFile.cFileName;
		arg->size = ((uint64_t)arg->_fdFile.nFileSizeHigh << 32) | arg->_fdFile.nFileSizeLow;
		return 1;
	}
	if(!FindNextFile(arg->_hFind, &arg->_fdFile)){
		FindClose(arg->_hFind);
		return 0;
	}
	arg->name = arg->_fdFile.cFileName;
	arg->size = ((uint64_t)arg->_fdFile.nFileSizeHigh << 32) | arg->_fdFile.nFileSizeLow;
	return 1;
}
#define FS_dir_traverse(dir_m) \
	for(uint8_t _mv = 1; _mv;) \
	for(FS_dir_traverse_t mv; _mv; _mv = 0) \
	if(!_FS_dir_traverse(dir_m, &mv)) \
	for(; FS_dir_traverse(&mv);)

enum{
	_FS_file_FileSystem_e,
	_FS_file_Temporarily_e
};

typedef struct{
	uint8_t Type;
	union{
		struct{
			IO_fd_t fd;
		}FileSystem;
		struct{
			IO_off_t Offset;
			VEC_t vector;
		}Temporarily;
	};
}FS_file_t;

void FS_file_getfd(FS_file_t *file, IO_fd_t *fd){
	switch(file->Type){
		case _FS_file_FileSystem_e:{
			*fd = file->FileSystem.fd;
			break;
		}
		case _FS_file_Temporarily_e:{
			PR_abort();
			break;
		}
	}
}

sint32_t FS_unlink(const void *path, uint32_t flag){
	if(_unlink((const char *)path) == -1){
		return -errno;
	}
	return 0;
}

sint32_t FS_unlinkn(const void *path, uintptr_t pathsize, uint32_t flag){
	uint8_t npath[PATH_MAX];
	if(pathsize >= (PATH_MAX - 1)){
		return -ENAMETOOLONG;
	}
	MEM_copy(path, npath, pathsize);
	npath[pathsize] = 0;
	return FS_unlink(npath, flag);
}

sint32_t FS_unlinkat(FS_dir_t *dir, const void *path, uint32_t flag){
	uint8_t ppath[PATH_MAX + 1];
	if(IO_getpath(&dir->dirfd, ppath)){
		return 1;
	}
	uint8_t spath[PATH_MAX + 1];
	sprintf((char *const)spath, "%s\\%s", (const char *const)ppath, path);
	return FS_unlink(spath, flag);
}

sint32_t FS_unlinkatn(FS_dir_t *dir, const void *path, uintptr_t pathsize, uint32_t flag){
	uint8_t npath[PATH_MAX];
	if(pathsize >= (PATH_MAX - 1)){
		return -ENAMETOOLONG;
	}
	MEM_copy(path, npath, pathsize);
	npath[pathsize] = 0;
	return FS_unlinkat(dir, npath, flag);
}

sint32_t FS_file_opentmp(FS_file_t *file){
	file->Type = _FS_file_Temporarily_e;
	file->Temporarily.Offset = 0;
	VEC_init(&file->Temporarily.vector, 1, A_resize);
	return 0;
}

bool _FS_file_rename_passpath(STR_ttcc_t *ttcc){
	return 1;
}
void _FS_file_GetPathDirectory(const void *src, uintptr_t src_length, void *dst){
	src_length--;
	while(src_length != (uintptr_t)-1){
		if(((uint8_t *)src)[src_length] == '/'){
			MEM_copy(src, dst, src_length + 1);
			((uint8_t *)dst)[src_length + 1] = 0;
			return;
		}
		src_length--;
	}
	((uint8_t *)dst)[0] = 0;
}
bool _FS_file_GetFileName(const void *src, uintptr_t src_length, void *dst){
	uintptr_t src_i = src_length;
	src_i--;
	while(src_i != (uintptr_t)-1){
		if(((uint8_t *)src)[src_i] == '/'){
			MEM_copy(&((uint8_t *)src)[src_i + 1], dst, src_length - src_i);
			((uint8_t *)dst)[src_length - src_i] = 0;
			return 0;
		}
		src_i--;
	}
	if(src_length){
		MEM_copy(src, dst, src_length);
		((uint8_t *)dst)[src_length] = 0;
		return 0;
	}
	return 1;
}
sint32_t FS_file_rename(FS_file_t *file, const void *path){
	switch(file->Type){
		case _FS_file_FileSystem_e:{
			PR_abort();
			return 0;
		}
		case _FS_file_Temporarily_e:{
			uintptr_t path_length = MEM_cstreu(path);
			uint8_t FileName[PATH_MAX];
			if(_FS_file_GetFileName(path, path_length, FileName)){
				return -1;
			}
			uint8_t PathDirectory[PATH_MAX];
			_FS_file_GetPathDirectory(path, path_length, PathDirectory);
			uint8_t tmppath[PATH_MAX];
			STR_ttcc_t ttcc;
			ttcc.f = _FS_file_rename_passpath;
			ttcc.c = 0;
			ttcc.p = sizeof(tmppath);
			ttcc.ptr = tmppath;
			if(STR_FSttcc(&ttcc, "%s%s.tmp%c", PathDirectory, FileName, 0)){
				return -ENAMETOOLONG;
			}
			IO_fd_t fd;
			sint32_t err = IO_open(tmppath, O_WRONLY | O_CREAT, &fd);
			if(err){
				PR_abort();
			}
			if(IO_write(
				&fd,
				file->Temporarily.vector.ptr,
				file->Temporarily.vector.Current
			) != file->Temporarily.vector.Current){
				PR_abort();
				if(IO_close(&fd) != 0){
					PR_abort();
				}
				return -1;
			}
			if(IO_close(&fd) != 0){
				PR_abort();
			}
			if(IO_rename(tmppath, path) != 0){
				PR_abort();
			}
			err = IO_open(path, O_WRONLY, &fd);
			if(err){
				PR_abort();
			}
			VEC_free(&file->Temporarily.vector);
			file->Type = _FS_file_FileSystem_e;
			file->FileSystem.fd = fd;
			return 0;
		}
	}
}

sint32_t FS_file_open(const void *path, FS_file_t *file, uint32_t flag){
	file->Type = _FS_file_FileSystem_e;
	sint32_t err = IO_open(path, flag, &file->FileSystem.fd);
	if(err){
		return err;
	}
	return 0;
}

sint32_t FS_file_openn(const void *path, uintptr_t pathsize, FS_file_t *file, uint32_t flag){
	uint8_t npath[PATH_MAX];
	if(pathsize >= (PATH_MAX - 1)){
		return -ENAMETOOLONG;
	}
	MEM_copy(path, npath, pathsize);
	npath[pathsize] = 0;
	return FS_file_open(npath, file, flag);
}

sint32_t FS_file_openat(FS_dir_t *dir, const void *path, FS_file_t *file, uint32_t flag){
	file->Type = _FS_file_FileSystem_e;
	sint32_t err = IO_openat(&dir->dirfd, path, flag, &file->FileSystem.fd);
	if(err){
		return err;
	}
	return 0;
}

sint32_t FS_file_openatn(FS_dir_t *dir, const void *path, uintptr_t pathsize, FS_file_t *file, uint32_t flag){
	uint8_t npath[PATH_MAX];
	if(pathsize >= (PATH_MAX - 1)){
		return -ENAMETOOLONG;
	}
	MEM_copy(path, npath, pathsize);
	npath[pathsize] = 0;
	return FS_file_openat(dir, npath, file, flag);
}

enum{
	FS_file_seek_Begin = SEEK_SET,
	FS_file_seek_Current = SEEK_CUR,
	FS_file_seek_End = SEEK_END
};

void FS_file_seek(FS_file_t *file, FS_off_t offset, uint32_t flag){
	switch(file->Type){
		case _FS_file_FileSystem_e:{
			PR_abort();
			return;
		}
		case _FS_file_Temporarily_e:{
			switch(flag){
				case FS_file_seek_Begin:{
					file->Temporarily.Offset = offset;
					return;
				}
				case FS_file_seek_Current:{
					file->Temporarily.Offset += offset;
					return;
				}
				case FS_file_seek_End:{
					file->Temporarily.Offset = file->Temporarily.vector.Current + offset;
					return;
				}
			}
			PR_abort();
		}
	}
}

FS_ssize_t FS_file_read(FS_file_t *file, void *data, FS_size_t size){
	switch(file->Type){
		case _FS_file_FileSystem_e:{
			return IO_read(&file->FileSystem.fd, data, size);
		}
		case _FS_file_Temporarily_e:{
			PR_abort();
			return 0;
		}
	}
}

FS_ssize_t FS_file_write(FS_file_t *file, const void *data, FS_size_t size){
	switch(file->Type){
		case _FS_file_FileSystem_e:{
			return IO_write(&file->FileSystem.fd, data, size);
		}
		case _FS_file_Temporarily_e:{
			FS_off_t of = file->Temporarily.Offset + size - file->Temporarily.vector.Current;
			if(of > 0){
				VEC_handle0(&file->Temporarily.vector, size);
			}
			MEM_copy(data, &file->Temporarily.vector.ptr[file->Temporarily.Offset], size);
			file->Temporarily.Offset += size;
			return size;
		}
	}
}

sint32_t FS_file_close(FS_file_t *file){
	switch(file->Type){
		case _FS_file_FileSystem_e:{
			return IO_close(&file->FileSystem.fd);
		}
		case _FS_file_Temporarily_e:{
			PR_abort();
			return 0;
		}
	}
}
