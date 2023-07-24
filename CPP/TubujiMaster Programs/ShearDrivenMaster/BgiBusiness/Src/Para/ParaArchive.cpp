#include "ParaArchive.h"


Archive::VarDesc NullArchive::Load(const unsigned char* mem_ptr, int total_len, const char* token) {
    BGI_ASSERT(mem_ptr != NULL);

    VarDesc var_desc;
    var_desc.MemSize = 0;
    var_desc.MemPtr = NULL;
    var_desc.VarType = NULL;
    var_desc.VarPtr = NULL;
    return var_desc;
}

Archive::VarDesc NullArchive::Save(const void* var_ptr, const std::type_info& info, const char* token) {
    BGI_ASSERT(var_ptr != NULL);
    VarDesc var_desc;
    var_desc.VarPtr = var_ptr;
    var_desc.VarType = &info;
    var_desc.MemPtr = NULL;
    var_desc.MemSize = 0;
    return var_desc;
}

void NullArchive::FreeVarPtr(const void* var_ptr) {
    BGI_ASSERT(false);
}


Archive::VarDesc StringArchive::Load(const unsigned char* mem_ptr, int total_len, const char* token) {
    BGI_ASSERT(mem_ptr != NULL);

    VarDesc var_desc;
    var_desc.MemPtr = mem_ptr;
    var_desc.MemSize = 0;
    var_desc.VarPtr = NULL;
    var_desc.VarType = &typeid(std::string);

    //mem_ptr的长度能够保存长度信息
    if (total_len < (int)sizeof(int))
        return var_desc;

    //长度不能小于0
    int len = 0;
    memcpy(&len, mem_ptr, sizeof(len));
    if (len < 0)
        return var_desc;

    //获得长度
    int value_size = sizeof(int) + len;

    //token的size和len
    int token_len = 0;
    int token_size = 0;
    if ((token != NULL) && (strlen(token) != 0))
    {
        token_len = strlen(token);
        token_size = sizeof(int) + token_len;
    }

    //mem_ptr中token的长度和内容指针
    int mem_token_len = 0;
    memcpy(&mem_token_len, (mem_ptr + value_size), sizeof(mem_token_len));

    const char* mem_token = (const char*)(mem_ptr + value_size + sizeof(int));

    //判断token是否相等
    bool token_equal = (total_len >= (int)(value_size + token_size));
    if (token_equal && (token_size > 0))
        token_equal = (mem_token_len == token_len) && (strncmp(mem_token, token, token_len) == 0);

    if (token_equal)
    {
        //生成std::string
        const char* str_ptr = (const char*)(mem_ptr + sizeof(int));
        std::string* var_ptr = new std::string(str_ptr, len);

        var_desc.MemSize = value_size + token_size;
        var_desc.VarPtr = var_ptr;
    }

    return var_desc;
}

Archive::VarDesc StringArchive::Save(const void* var_ptr, const std::type_info& info, const char* token) {
    BGI_ASSERT(var_ptr != NULL);
    BGI_ASSERT(info == typeid(std::string));

    //token的size和len
    int token_len = 0;
    int token_size = 0;
    if ((token != NULL) && (strlen(token) != 0))
    {
        token_len = strlen(token);
        token_size = sizeof(int) + token_len;
    }

    const std::string* var = (const std::string*)var_ptr;
    int len = var->size();
    const char* buffer = var->c_str();

    BGI_ASSERT(len >= 0);
    BGI_ASSERT(buffer != NULL);

    int value_size = sizeof(int) + len;
    int total_len = value_size + token_size;
    unsigned char* mem_ptr = new unsigned char[total_len];

    //保存长度
    memcpy(mem_ptr, &len, sizeof(len));

    //保存数据
    memcpy(mem_ptr + sizeof(int), buffer, len);

    //copy token的内容
    if (token_size > 0)
    {
        BGI_ASSERT(token_len > 0);
        memcpy((mem_ptr + value_size), &token_len, sizeof(token_len));
        char* mem_token = (char*)(mem_ptr + value_size + sizeof(int));
        memcpy(mem_token, token, token_len);
    }

    VarDesc var_desc;
    var_desc.MemPtr = mem_ptr;
    var_desc.MemSize = total_len;
    var_desc.VarPtr = var_ptr;
    var_desc.VarType = &typeid(std::string);
    return var_desc;
}

void StringArchive::FreeVarPtr(const void* var_ptr) {
    BGI_ASSERT(var_ptr != NULL);
    delete (const std::string*)var_ptr;
}