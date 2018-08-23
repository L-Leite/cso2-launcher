# setup PolyHook 2
Set-Location ./libs/PolyHook_2_0

# ugly hack, find a way to pass this through command line
$lastLine = (Get-Content -tail 1 ./CMakeLists.txt)
if ($lastLine -ne "add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)")
{
	Add-Content .\CMakeLists.txt "`nadd_definitions(-D_ITERATOR_DEBUG_LEVEL=0)"
}

Set-Location ./capstone
mkdir build32
mkdir build64
Set-Location ./build32
cmake -G "Visual Studio 15 2017" ../
Set-Location ../../
cmake -G "Visual Studio 15 2017" -DBUILD_DLL=ON -DBUILD_STATIC=OFF ./

# setup source sdk
Set-Location ..\source-sdk-cso2
.\createlauncher.bat
