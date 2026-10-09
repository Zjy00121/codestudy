# Python 环境与运行说明

## 已检查及配置

- 基础解释器：F:\python\python.exe，Python 3.14.2。
- 项目解释器：python_basics/.venv/Scripts/python.exe。
- 虚拟环境 pip：25.3，pip check 通过。
- 项目检查脚本验证实际解释器、环境隔离及 UTF-8 文件读写通过。
- 根 .vscode 配置默认解释器、Python 当前文件调试入口和 Python/Pylance/debugpy 扩展推荐；原 C 配置保留。
- .venv、字节码及缓存加入忽略规则；没有安装机器学习依赖，没有修改系统 PATH 或 PowerShell 执行策略。

## PowerShell 运行

在仓库根目录执行，不需要激活环境：

```powershell
Set-Location 'H:\study\MYCODE\codestudy'
& '.\python_basics\.venv\Scripts\python.exe' --version
& '.\python_basics\.venv\Scripts\python.exe' -m pip --version
& '.\python_basics\.venv\Scripts\python.exe' python_basics/1_environment/check_environment.py
```

运行自己的题目时把末尾脚本路径替换成对应 main.py。使用 `解释器 -m pip` 保证依赖装进当前环境。当前没有第三方依赖，不必安装 requirements。

## VSCode 扩展与解释器

初次检查时未检测到 Python 扩展，随后用户已确认插件安装完成。Code Runner 解释器配置修正后，用户已确认运行测试正常。F5 断点调试尚未单独确认。

通过“Python: Select Interpreter”选择项目 .venv/Scripts/python.exe。如果曾选择其他解释器，默认路径设置不会自动覆盖已保存选择。打开 Python 文件运行，F5 选择“Python: current file”。C 文件继续选择原 GDB 配置。

## 现有安装的限制

基础解释器调用会打印 `Failed to find real location of F:\python\python.exe`，但本次脚本、venv 与 pip 检查成功。未确定该警告来自安装布局还是受限环境，不在此声称已修复全局安装。

py 启动器报告没有已注册 Python，因此当前直接使用明确解释器，不依赖 py -3。python3 命令指向 WindowsApps 别名，本次不用于运行。

重建环境时，若临时目录权限受限，可以把本次进程 TEMP/TMP 指到项目内已有临时目录。虚拟环境不应提交或搬移；以后仓库移动时重新创建。

解释器 3.14.2 可用于基础学习，未来选择机器学习依赖时再验证对应版本兼容性，本次未检查第三方包兼容性。环境配置完成不代表第 1 阶段课程已完成。

## Code Runner 解释器修正

Code Runner 的 `python -u ...` 默认命令可能使用 PATH 上的全局 Python，而非 Python 扩展所选解释器。根 .vscode/settings.json 已单独设置 code-runner.executorMap.python，以 PowerShell 的 & 调用本项目虚拟环境。此绝对路径配置适用于当前工作区，仓库迁移位置后需同步更新。

修正后的终端命令已运行环境检查成功，随后用户已确认实际运行测试正常，原先误用全局解释器导致的 RuntimeError 已解决。如果 VSCode 继续输出旧的 python -u 命令，可执行 Developer: Reload Window 后重新运行。也可使用 Python 扩展的“Run Python File in Terminal”，并选择项目解释器。

## 用户验证记录

- 用户已确认插件安装完成。
- Code Runner 解释器修正后，用户反馈测试正常，确认环境检查脚本可正常运行。
- 此前实际执行的检查已验证虚拟环境隔离、UTF-8 文件读写和 pip；本次反馈补充用户实际运行确认。
- 尚未单独确认 F5 断点调试或机器学习依赖兼容性；基础解释器路径警告未宣称消除。
