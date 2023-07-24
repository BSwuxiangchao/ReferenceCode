/*
* @Abrief 定义存储接口
* @note 接口将变量的内容转化为可保存在文件中的数据形式, 以及从数据中恢复变量的内容
* @version 1.0
* @author mosongjin 2021-07-27
*/

#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include <typeinfo>

class Archive
{
public:
    struct VarDesc
    {
        const void* VarPtr;
        const std::type_info* VarType;
        const unsigned char* MemPtr;
        int MemSize;
    };

    virtual ~Archive() {}
    virtual VarDesc Load(const unsigned char* mem_ptr, int total_len, const char* token) = 0;
    virtual VarDesc Save(const void* var_ptr, const std::type_info& info, const char* token) = 0;
    virtual void FreeVarPtr(const void* var_ptr) = 0;
};

#endif