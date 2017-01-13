RMDIR package /S /Q
XCOPY ..\open-zwave\config package\config\ /S /Y
XCOPY OpenZWave.nuspec package\ /Y
XCOPY ..\src\x86\Release\OpenZWaveUWP\OpenZWave.dll package\runtimes\win10-x86\native\ /Y
XCOPY ..\src\x64\Release\OpenZWaveUWP\OpenZWave.dll package\runtimes\win10-x64\native\ /Y
XCOPY ..\src\arm\Release\OpenZWaveUWP\OpenZWave.dll package\runtimes\win10-arm\native\ /Y
XCOPY ..\src\x86\Release\OpenZWaveUWP\OpenZWave.winmd package\lib\uap10.0\ /Y
XCOPY ..\src\x86\Release\OpenZWaveUWP\OpenZWave.pri package\lib\uap10.0\ /Y
XCOPY ..\src\x86\Release\OpenZWaveUWP\OpenZWave.xml package\lib\uap10.0\ /Y
XCOPY OpenZWave.targets package\build\net452\ /Y
XCOPY OpenZWave.targets package\build\uap10.0\ /Y

nuget pack package\OpenZWave.nuspec
PAUSE