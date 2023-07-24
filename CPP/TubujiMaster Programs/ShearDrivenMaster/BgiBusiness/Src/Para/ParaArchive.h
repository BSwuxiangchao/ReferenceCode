/*
* @Abrief Archive的实现类
* @note 对Archive的接口实现
* @version 1.0
* @author mosongjin 2021-07-27
*/

#ifndef _PARA_ARCHIVE_H_
#define _PARA_ARCHIVE_H_

#include "Archive.h"
#include "BgiLogger.h"
#include "ParaType.h"
#include <string.h>
#include <memory.h>

template<class T> class DefaultArchive : public Archive
{
public:
    VarDesc Load(const unsigned char* mem_ptr, int total_len, const char* token) 
    {
        BGI_ASSERT(mem_ptr != NULL);

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
        memcpy(&mem_token_len, (mem_ptr + sizeof(T)), sizeof(mem_token_len));
        const char* mem_token = (const char*)(mem_ptr + sizeof(T) + sizeof(int));

        //判断token是否相等
        bool token_equal = (total_len >= (int)(sizeof(T) + token_size));
        if (token_equal && (token_size > 0))
            token_equal = (token_len == mem_token_len) && (strncmp(mem_token, token, token_len) == 0);

        VarDesc var_desc;
        if (!token_equal)
        {
            var_desc.MemSize = 0;
            var_desc.MemPtr = NULL;
            var_desc.VarType = &typeid(T);
            var_desc.VarPtr = NULL;
        }
        else
        {
            var_desc.MemSize = sizeof(T) + token_size;
            var_desc.MemPtr = mem_ptr;
            var_desc.VarType = &typeid(T);

            T* var_ptr = new T;
            memcpy(var_ptr, mem_ptr, sizeof(T));
            var_desc.VarPtr = var_ptr;
        }
        return var_desc;
    }

    VarDesc Save(const void* var_ptr, const std::type_info& info, const char* token) {
        BGI_ASSERT(var_ptr != NULL);
        BGI_ASSERT(info == typeid(T));

        //token的size和len
        int token_len = 0;
        int token_size = 0;
        if ((token != NULL) && (strlen(token) != 0))
        {
            token_len = strlen(token);
            token_size = sizeof(int) + token_len;
        }

        VarDesc var_desc;
        var_desc.VarPtr = var_ptr;
        var_desc.VarType = &info;
        var_desc.MemSize = sizeof(T) + token_size;
        unsigned char* mem_ptr = new unsigned char[var_desc.MemSize];

        //copy参数的内容
        memcpy(mem_ptr, var_ptr, sizeof(T));

        //copy token的内容
        if (token_size > 0)
        {
            BGI_ASSERT(token_len > 0);
            memcpy(mem_ptr + sizeof(T), &token_len, sizeof(token_len));
            char* mem_token = (char*)(mem_ptr + sizeof(T) + sizeof(token_len));
            memcpy(mem_token, token, token_len);
        }

        var_desc.MemPtr = mem_ptr;
        return var_desc;
    }

    void FreeVarPtr(const void* var_ptr) {
        BGI_ASSERT(var_ptr != NULL);
        delete (const T*)var_ptr;
    }
};

class NullArchive : public Archive
{
public:
    VarDesc Load(const unsigned char* mem_ptr, int total_len, const char* token);

    VarDesc Save(const void* var_ptr, const std::type_info& info, const char* token);

    void FreeVarPtr(const void* var_ptr);
};

template<class T> class LevelValueArchive : public Archive
{
public:
    LevelValueArchive(Archive* elem_archive)
        :m_ElementArchive(elem_archive)
    {
        BGI_ASSERT(m_ElementArchive != NULL);
    }
    ~LevelValueArchive() {
        delete m_ElementArchive;
    }

    VarDesc Load(const unsigned char* mem_ptr, int total_len, const char* token) {
        VarDesc var_desc;
        var_desc.MemPtr = mem_ptr;
        var_desc.MemSize = 0;
        var_desc.VarPtr = NULL;
        var_desc.VarType = &typeid(LevelValue<T>);

        //token的size和len
        int token_len = 0;
        int token_size = 0;
        if ((token != NULL) && (strlen(token) != 0))
        {
            token_len = strlen(token);
            token_size = sizeof(int) + token_len;
        }

        if (total_len < 2 * (int)sizeof(int) + token_size)
        {
            //total_len不够存储档位, 档位数以及token
            return var_desc;
        }

        int elem_idx = 0;

        //获取当前档位
        int current_level = 0;
        memcpy(&current_level, (mem_ptr + elem_idx), sizeof(current_level));
        elem_idx += sizeof(int);

        //获取档位数
        int level_num = 0;
        memcpy(&level_num, (mem_ptr + elem_idx), sizeof(level_num));
        elem_idx += sizeof(int);

        //获取各个档位的值
        bool is_suc = true;;
        std::vector<VarDesc> mem_list;
        for (int i = 0; i < level_num; i++)
        {
            VarDesc elem_var_desc = m_ElementArchive->Load((mem_ptr + elem_idx), total_len - elem_idx, NULL);
            if (elem_var_desc.VarPtr == NULL)
            {
                is_suc = false;
                break;
            }

            mem_list.push_back(elem_var_desc);
            elem_idx += elem_var_desc.MemSize;
        }
        BGI_ASSERT(elem_idx <= total_len);

        //构造LevalValue<T>
        int value_size = elem_idx;
        if (is_suc)
        {
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
                LevelValue<T>* var_ptr = new LevelValue<T>;
                var_ptr->Level = current_level;

                std::vector<VarDesc>::iterator it = mem_list.begin();
                std::vector<VarDesc>::iterator it_end = mem_list.end();
                for (; it != it_end; ++it)
                {
                    BGI_ASSERT(*it->VarType == typeid(T));
                    var_ptr->List.push_back(*(T*)it->VarPtr);
                }

                var_desc.MemSize = value_size + token_size;
                var_desc.VarPtr = var_ptr;
            }
        }

        //释放mem_list的空间
        std::vector<VarDesc>::iterator it = mem_list.begin();
        std::vector<VarDesc>::iterator it_end = mem_list.end();
        for (; it != it_end; ++it)
        {
            m_ElementArchive->FreeVarPtr(it->VarPtr);
            it->VarPtr = NULL;
        }

        return var_desc;
    }

    VarDesc Save(const void* var_ptr, const std::type_info& info, const char* token) {
        BGI_ASSERT(typeid(LevelValue<T>) == info);
        BGI_ASSERT(var_ptr != NULL);

        VarDesc var_desc;
        var_desc.MemPtr = NULL;
        var_desc.MemSize = 0;
        var_desc.VarPtr = var_ptr;
        var_desc.VarType = &info;

        const LevelValue<T>* var = (const LevelValue<T> *)var_ptr;
        std::vector<VarDesc> mem_list;

        //获取List各个元素的mem
        bool is_suc = true;
        int total_elem_len = 0;
        int list_size = var->List.size();
        for (int i = 0; i < list_size; i++)
        {
            VarDesc elem_var_desc = m_ElementArchive->Save(&(var->List[i]), typeid(T), NULL);
            if (elem_var_desc.MemPtr != NULL)
            {
                mem_list.push_back(elem_var_desc);
                total_elem_len += elem_var_desc.MemSize;
            }
            else
            {
                is_suc = false;
                break;
            }
        }

        if (is_suc)
        {
            //token的size和len
            int token_len = 0;
            int token_size = 0;
            if ((token != NULL) && (strlen(token) != 0))
            {
                token_len = strlen(token);
                token_size = sizeof(int) + token_len;
            }

            //需要增加用于保存当前Level和List长度, token的空间
            int total_len = total_elem_len + 2 * sizeof(int) + token_size;
            unsigned char* mem_ptr = new unsigned char[total_len];
            unsigned char* elem_ptr = (unsigned char*)mem_ptr;

            //存当前Level
            memcpy(elem_ptr, &var->Level, sizeof(var->Level));
            elem_ptr += sizeof(var->Level);

            //存档位数
            int level_num = var->List.size();
            memcpy(elem_ptr, &level_num, sizeof(level_num));
            elem_ptr += sizeof(level_num);

            //存List各个档位值
            std::vector<VarDesc>::const_iterator it = mem_list.begin();
            std::vector<VarDesc>::const_iterator it_end = mem_list.end();
            for (; it != it_end; ++it)
            {
                memcpy(elem_ptr, it->MemPtr, it->MemSize);
                elem_ptr += it->MemSize;
            }

            //copy token的内容
            if (token_size > 0)
            {
//                IM_ASSERT(token_len > 0);
                memcpy(elem_ptr, &token_len, sizeof(token_len));
                char* mem_token = (char*)(elem_ptr + sizeof(token_len));
                memcpy(mem_token, token, token_len);
            }

            //设置var_desc的值
            var_desc.MemPtr = mem_ptr;
            var_desc.MemSize = total_len;
        }

        //释放mem_list中的空间
        std::vector<VarDesc>::iterator mem_it = mem_list.begin();
        std::vector<VarDesc>::iterator mem_it_end = mem_list.end();
        for (; mem_it != mem_it_end; ++mem_it)
        {
            BGI_ASSERT(mem_it->MemPtr != NULL);
            delete[] mem_it->MemPtr;
            mem_it->MemPtr = NULL;
            mem_it->MemSize = 0;
        }

        return var_desc;
    }

    void FreeVarPtr(const void* var_ptr) {
        BGI_ASSERT(var_ptr != NULL);
        delete (const LevelValue<T> *)var_ptr;
    }

private:
    Archive* m_ElementArchive;
};



class StringArchive : public Archive
{
public:
    VarDesc Load(const unsigned char* mem_ptr, int total_len, const char* token);

    VarDesc Save(const void* var_ptr, const std::type_info& info, const char* token);

    void FreeVarPtr(const void* var_ptr);

};


#endif
