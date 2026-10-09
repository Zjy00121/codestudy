# C Language Basics

C 语言基础学习归档，用户已确认第十七章及整个基础学习阶段完成。

- [学习计划与完成状态](STUDY_PLAN.md)
- [章节笔记](notes/)
- [第十七章：经典抽象数据类型](17_abstract_data_types/README.md)

编号章节及内部相对路径沿用原结构。每道练习独立编译，多文件练习需显式编译对应源文件。

迁移后，在仓库根目录进入第二题的 PowerShell 命令为：

```powershell
Set-Location '.\c_language_basics\17_abstract_data_types\Q_2'
& 'C:/mingw64/bin/gcc.exe' -std=c11 -g -Wall -Wextra -Wpedantic -Wshadow -Wconversion main.c queue.c -o main.exe
if ($LASTEXITCODE -eq 0) { .\main.exe }
```

CLAUDE.md 为原样保留的历史文件，其中进度快照可能过期。当前进度只以 STUDY_PLAN.md 为准。
