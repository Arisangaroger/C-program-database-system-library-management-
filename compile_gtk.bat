@echo off
echo Compiling Library Management System (GTK Version)...

REM Set up environment
set PATH=%PATH%;C:\msys64\mingw64\bin

REM Get GTK flags using pkg-config
for /f "tokens=*" %%a in ('pkg-config --cflags gtk+-3.0') do set GTK_CFLAGS=%%a
for /f "tokens=*" %%a in ('pkg-config --libs gtk+-3.0') do set GTK_LIBS=%%a

echo GTK_CFLAGS: %GTK_CFLAGS%
echo GTK_LIBS: %GTK_LIBS%

REM Compile and link in one step
echo Compiling and linking...
gcc -o library_system_gtk.exe library_gtk.c file_ops.c -mwindows %GTK_CFLAGS% %GTK_LIBS%
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile and link
    exit /b %ERRORLEVEL%
)

echo Compilation successful!

REM Clean up object files
del *.o 