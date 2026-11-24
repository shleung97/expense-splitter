param(
    [string]$Compiler = 'C:\msys64\ucrt64\bin\g++.exe',
    [string]$OutputName = 'expense-splitter-gui.exe'
)
$ErrorActionPreference = 'Stop'
if (-not (Test-Path -LiteralPath $Compiler)) {
    throw "C++ compiler not found at $Compiler. Supply -Compiler with your MinGW g++.exe path."
}
$env:PATH = (Split-Path -Parent $Compiler) + ';' + $env:PATH
$output = Join-Path $PSScriptRoot 'build'
New-Item -ItemType Directory -Force -Path $output | Out-Null
$sources = @('gui\main.cpp', 'src\expense.cpp', 'src\input.cpp', 'src\storage.cpp', 'src\operations.cpp', 'src\report.cpp', 'src\budget.cpp', 'src\percentage.cpp') |
    ForEach-Object { Join-Path $PSScriptRoot $_ }
& $Compiler -std=c++11 -Wall -Wextra -I (Join-Path $PSScriptRoot 'src') $sources `
    -mwindows -static -lcomctl32 -lcomdlg32 -o (Join-Path $output $OutputName)
if ($LASTEXITCODE -ne 0) { throw 'GUI build failed.' }
Write-Output "Built $output\$OutputName"
