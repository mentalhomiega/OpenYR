@echo off
rem Makes a copy of your CnCNet Yuri's Revenge install that starts OpenYR. It asks before changing anything.
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0cncnet_tester_setup.ps1"
echo.
pause
