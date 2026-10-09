"""检查解释器、虚拟环境与基本运行；仅使用标准库。"""
import sys
from pathlib import Path
import tempfile

# 本脚本位于 1_environment；上一级就是 Python 学习目录。
project_dir = Path(__file__).resolve().parent.parent
expected_prefix = project_dir / ".venv"

# 使用实际路径比较，确认没有误用全局解释器。
if Path(sys.prefix).resolve() != expected_prefix.resolve():
    raise RuntimeError("Run this script with python_basics/.venv/Scripts/python.exe")

# 临时文件放在已限定的项目环境内；with 退出时自动关闭和清理。
temp_dir = expected_prefix / "tmp"
temp_dir.mkdir(exist_ok=True)
with tempfile.TemporaryDirectory(dir=temp_dir) as directory:
    sample = Path(directory) / "sample.txt"
    sample.write_text("Python environment OK", encoding="utf-8")
    if sample.read_text(encoding="utf-8") != "Python environment OK":
        raise RuntimeError("UTF-8 file round trip failed")

print(f"Python: {sys.version.split()[0]}")
print(f"Executable: {sys.executable}")
print("Virtual environment: OK")
print("UTF-8 file read/write: OK")
