@echo off
echo Deploying NexPlayer for Windows...

set QT_PATH=C:\Qt\6.5.0\msvc2019_64
set BUILD_DIR=build\Release

:: Bundle Qt
echo Bundling Qt...
%QT_PATH%\bin\windeployqt.exe --qmldir qml %BUILD_DIR%\NexPlayer.exe

:: Copy FFmpeg DLLs
echo Bundling FFmpeg...
copy C:\ProgramData\chocolatey\lib\ffmpeg\tools\ffmpeg\bin\*.dll %BUILD_DIR%\

echo Windows deployment complete!