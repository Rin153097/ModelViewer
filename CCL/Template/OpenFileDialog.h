#pragma once

#include <Windows.h>
#include <queue>
#include <atlstr.h>

typedef std::queue<CStringA> FilePathList;

class FilePathManager {
public:
    static FilePathList pathList;
    static CStringA     next;
    static CStringA     null;

    static void GetPathByDialog();
    static size_t GetSize() { return pathList.size(); }
    static const CStringA&    GetPath() {
        if (pathList.empty()) return null;
        next = pathList.front();
        pathList.pop();
        return next;
    }
};