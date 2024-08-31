
if (-Not (Test-Path -Path "build")) {
    New-Item -Path "build" -ItemType Directory
}

Set-Location .\build

cmake ..

Set-Location ..

.\ms_build.ps1 .\build\dylib_for_rust.sln /p:Configuration=Release





# 获取源文件夹中的所有文件
$files = Get-ChildItem -Path ..\c_libs\Release -File

# 移动每个文件到目标文件夹
foreach ($file in $files) {
    Move-Item -Path $file.FullName -Destination ..\c_libs
}



