@echo off
echo Compiling Library Management System (GUI Version)...

windres -i app.rc -o app.res
gcc -Wall -Wextra -g -DUNICODE -D_UNICODE -c gui_ui.c
gcc -Wall -Wextra -g -c file_ops.c
gcc -Wall -Wextra -g -mwindows -o library_system_gui.exe gui_ui.o file_ops.o app.res -lcomctl32 -lcomdlg32 -lgdi32 -luser32 -lshell32

if %errorlevel% equ 0 (
    echo Compilation successful!
    echo Run library_system_gui.exe to start the program
) else (
    echo Compilation failed!
)

pause 