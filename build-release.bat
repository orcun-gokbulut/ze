call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
cd Build
cmake -G "Visual Studio 9 2008" .. 
vcbuild ZinekEngine.sln "Release|Win32"
vcbuild Package.vcproj "Release|Win32"
pause