//
// Created by plaka on 05.04.2023.
//

#include "FileMgr.h"
#include "util/patch.h"

void CFileMgr::Initialise()
{
    CHook::CallFunction<void>(g_libGTASA + 0x003F0B74);
}

int32_t CFileMgr::ChangeDir(const char *path)
{
    CHook::CallFunction<int32_t>(g_libGTASA + 0x003F0BE4);
}

int32_t CFileMgr::SetDir(const char *path)
{
    CHook::CallFunction<int32_t>(g_libGTASA + 0x003F0C54);
}

size_t CFileMgr::LoadFile(const char *path, uint8_t *buf, size_t size, const char *mode)
{
    FILESTREAM f = CFileMgr::OpenFile(path, mode);
    if (f == nullptr)
        return -1;

    // MikuAuahDark: The original implementation actually ignore
    // "size" parameter and adds NUL terminator at the end of the
    // buffer, but that behavior causes buffer overflow
    size_t readed = fread(buf, 1, size, f);
    fclose(f);

    return readed;
}

FILESTREAM CFileMgr::OpenFile(const char *path, const char *mode)
{
    FILESTREAM fs{nullptr};
    //if (WindowsCharset == CP_UTF8) {
        fs = fopen(path, mode);
 //   } else {
        // MikuAuahDark: Let's improve it to allow opening non-ANSI names
//        // Convert to wide char
//        std::wstring pathWide = UTF8ToUnicode(path);
//        std::wstring modeWide = UTF8ToUnicode(mode);
//        _wfopen_s(&fs, pathWide.c_str(), modeWide.c_str());
//    }
    return fs;
}

FILESTREAM CFileMgr::OpenFileForWriting(const char *path)
{
    return CFileMgr::OpenFile(path, "wb");
}

FILESTREAM CFileMgr::OpenFileForAppending(const char *path)
{
    return CFileMgr::OpenFile(path, "a");
}


// 0x538950
size_t CFileMgr::Read(FILESTREAM file, void *buf, size_t size)
{
    return fread(buf, 1, size, file);
}

// 0x538970
size_t CFileMgr::Write(FILESTREAM file, const void *buf, size_t size)
{
    return fwrite(buf, 1, size, file);
}

// 0x538990
bool CFileMgr::Seek(FILESTREAM file, long offset, int32_t origin)
{
    // MikuAuahDark: Pretty sure it shouldn't be ret != 0
    return fseek(file, offset, origin) != 0;
}

// 0x5389b0
bool CFileMgr::ReadLine(FILESTREAM file, char *str, int32_t num)
{
    return fgets(str, num, file) != nullptr;
}

// 0x5389d0
int32_t CFileMgr::CloseFile(FILESTREAM file)
{
    return fclose(file);
}

int32_t CFileMgr::GetTotalSize(FILESTREAM file)
{
    int32_t currentPos, size;
    // MikuAuahDark: The actual implementation uses RwOsGetFileInterface
    // but for sake of portability, default stdio functions is used.
    /*
    RwFileFunctions *functions = RwOsGetFileInterface();

    currentPos = functions->rwftell(file);
    functions->rwfseek(file, 0, SEEK_END);
    size = functions->rwftell(file);
    functions->rwfseek(file, currentPos, SEEK_SET);

    return size;
    */
    currentPos = ftell(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, currentPos, SEEK_SET);

    return size;
}

// 0x538a20
int32_t CFileMgr::Tell(FILESTREAM file)
{
    /*
    RwFileFunctions *functions = RwOsGetFileInterface();
    return functions->rwftell(file);
    */
    return ftell(file);
}

// 0x538a50
bool CFileMgr::GetErrorReadWrite(FILESTREAM file)
{
    return (bool) ferror(file);
}

// notsa
void CFileMgr::SeekNextLine(FILESTREAM file) {
    while (!feof(file) && fgetc(file) != '\n');
}

int32_t CFileMgr::SetDirMyDocuments()
{
    //CFileMgr::fileDataArea = OSFDA_User_0;
}