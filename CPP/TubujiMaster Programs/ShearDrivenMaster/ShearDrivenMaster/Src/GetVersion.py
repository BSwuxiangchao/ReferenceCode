# -*- coding: utf-8 -*-
from git import Repo
import tkinter
import tkinter.messagebox
import os

Cur_Dir=os.getcwd()
Git_Dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(Cur_Dir)))))
print('Git_Dir: '+Git_Dir)
# 有未提交更改时提示


repo = Repo(Git_Dir)
verstr = repo.git.log('-1','--pretty=format:%cd hash:%h')
if repo.is_dirty():
    a=tkinter.messagebox.askokcancel('提示', '有未提交修改, 是否先手动更新上传获取最新版本信息?')
    if a:
        print("更新")
        raise
    verstr += '[M]'
    print("稍后请及时更新上传")
 
versionPath = Cur_Dir+'/GitVer.h'
print('versionPath: '+versionPath)
fi = open(versionPath, 'w', encoding='utf-8')
fstr = '#ifndef _GITVER_H\n#define _GITVER_H\n#define GIT_VER "' + verstr + '"\n#endif'
fi.write(fstr)
fi.close()
 
print('成功更新git信息到GitVer.h文件')