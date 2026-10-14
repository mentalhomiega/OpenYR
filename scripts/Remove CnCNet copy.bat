@echo off
rem Deletes an OpenYR CnCNet copy made by "Set up CnCNet.bat". It asks before deleting.
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0cncnet_tester_setup.ps1" -Remove
echo.
pause
