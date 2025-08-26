@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion
cd /d "%~dp0"

//여기에 texconv.exe의 경로 설정
set TEXCONV="K:\Personal Files\Code\PSN_PROJECT\Client\Bin\Resources\_SUPERHOT\Textures\_Prototype\texconv.exe"

for /R %%F in (*.png *.jpg) do (
    set "FULL=%%~F"
    set "OUT=%%~dpF"
    REM 경로에서 마지막 백슬래시 제거 (texconv에 잘 전달되게)
    call set "OUT=!OUT:~0,-1!"

    set "REL=!FULL:%~dp0=!"
    
    echo ▶ 실행: %TEXCONV% -y -f BC3_UNORM -m 0 -o "!OUT!" "!REL!"
    %TEXCONV% -y -f BC3_UNORM -m 0 -o "!OUT!" "!REL!"
)

pause