#include "DirScanner.h"

#include <dirent.h>

#include <cerrno>
#include <cstring>
#include <memory>

using std::unique_ptr;

// unique_ptr 删除器, 用于自动释放 DIR 指针
struct DirCloser {
    void operator()(DIR* dir) const {
        if (dir) {
            closedir(dir);
        }
    }
};

// 传入 路径, 将路径下的文件名 完整路径存入 vector
void DirScanner::traverse(string path) {
    unique_ptr<DIR, DirCloser> upDir(opendir(path.c_str()));
    if (!upDir.get()) {
        perror("opendir");
        return;
    }

    // 返回的 指针 指向 函数内部的静态变量, 不需要手动释放
    struct dirent* pDirent = nullptr;
    while ((pDirent = readdir(upDir.get())) != nullptr) {
        int cmp1 = strcmp(pDirent->d_name, ".");
        int cmp2 = strcmp(pDirent->d_name, "..");

        // 文件名 不为 . / ..
        if (cmp1 && cmp2) {
            _files.push_back(path + "/" + pDirent->d_name);
        }
    }

    if (errno != 0) {
        perror("readdir");
        return;
    }
}

// 返回 储存文件路径的 vector
vector<string>& DirScanner::getFiles() { return _files; }
