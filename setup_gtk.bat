@echo off
echo Setting up GTK development environment...

REM Check if MSYS2 is installed
if not exist "C:\msys64" (
    echo MSYS2 not found. Please install MSYS2 from https://www.msys2.org/
    echo After installation, run this script again.
    pause
    exit /b 1
)

REM Add MSYS2 to PATH if not already there
set PATH=%PATH%;C:\msys64\mingw64\bin

REM Install required packages
echo Installing required packages...
pacman -S --noconfirm mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gcc

echo Setup complete!
echo You can now compile GTK applications using compile_gtk.bat
pause 