# Writes "CnCNet setup.bat": cncnet_tester_setup.ps1 behind a batch header, so testers get one file
# they can double-click. Windows opens a .ps1 in Notepad on a double-click, which is why the header
# is needed. cmd runs the header and stops at "exit /b"; PowerShell reads the header as a comment.
#   powershell.exe -NoProfile -ExecutionPolicy Bypass -File make_cncnet_setup_bat.ps1 [-Output <path>]
param([string]$Output = (Join-Path $PSScriptRoot 'CnCNet setup.bat'))

$ErrorActionPreference = 'Stop'
$header = @(
	'<# : OpenYR CnCNet setup. Double-click to make or remove an OpenYR copy of a CnCNet install.',
	'@echo off',
	'set "OPENYR_SETUP=%~f0"',
	'powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& ([scriptblock]::Create([IO.File]::ReadAllText($env:OPENYR_SETUP))) -BuildPath (Split-Path $env:OPENYR_SETUP -Parent)"',
	'echo.',
	'pause',
	'exit /b',
	'#>'
)
$script = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'cncnet_tester_setup.ps1'))
$text = ($header -join "`r`n") + "`r`n" + ($script -replace "(?<!`r)`n", "`r`n")
[IO.File]::WriteAllText($Output, $text, (New-Object System.Text.UTF8Encoding $false))
Write-Host "Wrote $Output"
