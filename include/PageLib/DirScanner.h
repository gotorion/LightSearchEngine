#ifndef __DIRSCANNER_H__
#define __DIRSCANNER_H__

#include <string>
#include <vector>

using std::string;
using std::vector;

class DirScanner {
   public:
    // 传入 路径, 将路径下的文件名 的 完整路径存入 vector
    void traverse(string path);

    // 返回 储存文件路径的 vector
    vector<string>& getFiles();

   private:
    vector<string> _files;
};

#endif
