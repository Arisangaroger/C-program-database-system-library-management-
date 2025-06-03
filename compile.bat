@echo off
echo Compiling Library Management System...

gcc -o library_system.exe console_ui.c file_ops.c -Wall

if %errorlevel% equ 0 (
    echo Compilation successful!
    echo Run library_system.exe to start the program
) else (
    echo Compilation failed!
)

pause 