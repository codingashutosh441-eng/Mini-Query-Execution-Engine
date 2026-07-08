import subprocess

result = subprocess.run(
    [r"D:\C++ env\mini_query_engine\ash4.exe"],
    capture_output=True,
    text=True
)

print(result)