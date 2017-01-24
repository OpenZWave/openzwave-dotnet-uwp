SET PackageName=dotMorten.OpenZWave
RMDIR package /S /Q
XCOPY ..\open-zwave\config package\config\ /S /Y
XCOPY OpenZWave.nuspec package\ /Y

REM UAP...
XCOPY ..\src\Output\OpenZWaveUWP\x86\Release\OpenZWave.dll package\runtimes\win10-x86\native\ /Y
XCOPY ..\src\Output\OpenZWaveUWP\x64\Release\OpenZWave.dll package\runtimes\win10-x64\native\ /Y
XCOPY ..\src\Output\OpenZWaveUWP\ARM\Release\OpenZWave.dll package\runtimes\win10-arm\native\ /Y
XCOPY ..\src\Output\OpenZWaveUWP\x86\Release\OpenZWave.winmd package\lib\uap10.0\ /Y
XCOPY ..\src\Output\OpenZWaveUWP\x86\Release\OpenZWave.pri package\lib\uap10.0\ /Y
XCOPY ..\src\Output\OpenZWaveUWP\x86\Release\OpenZWave.xml package\lib\uap10.0\ /Y
MD package\build\uap10.0\
COPY OpenZWave.uap10.0.targets package\build\uap10.0\%PackageName%.targets /Y
REM .NET4.5.2...
XCOPY ..\src\Output\OpenZWaveDotNet\x86\Release\OpenZWaveDotNet.dll package\lib\net452\ /Y
XCOPY ..\src\Output\OpenZWaveDotNet\x86\Release\OpenZWaveDotNet.xml package\lib\net452\ /Y
MD package\build\net452\
COPY OpenZWave.net452.targets package\build\net452\%PackageName%.targets /Y

nuget pack package\OpenZWave.nuspec
PAUSE