# Anno1800PythonAPI
Send python code to Anno 1800 using PyRun_SimpleString.

Usage: Just extract AnnoPythonAPITool.exe and AnnoPythonInject.dll anywhere you like and run AnnoPythonAPITool.exe.

How it Works:
The GUI application writes the desired code/command to script.lua (this can also be programmed as script.py). After that it injects the AnnoPythonInject.dll into Anno 1800. The injected dll looks for python35.dll and initializes the thread with PyGILState_Ensure. Then sends a command to run our script.lua file using PyRun_SimpleString and after that finalizes the thread with PyGILState_Release. When the dll is finished sending the command it fully unloads itself from Anno 1800.


![Screenshot 2022-12-26 114746](https://user-images.githubusercontent.com/50437199/209540439-02cff3d5-2e92-45e8-a0a0-5fb36fd01daf.png)
