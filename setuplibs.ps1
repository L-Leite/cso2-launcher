# setup PolyHook 2
Set-Location .\libs\PolyHook_2_0

$extraCmakeCfg = 'if(MSVC)
 set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
 set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
 set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
 set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
endif()
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)'

# ugly hack, find a way to pass this through command line
$lastLine = (Get-Content -tail 1 .\CMakeLists.txt)
if ($lastLine -ne 'add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)') {
    Add-Content .\CMakeLists.txt $extraCmakeCfg
}

# build capstone for PH
.\BuildCapstone.bat
# now PH
cmake -G "Visual Studio 15 2017" -DBUILD_DLL=ON -DBUILD_STATIC=ON .\

# setup source sdk
Set-Location ..\source-sdk-cso2
.\createlauncher.bat

# get back to the project's root
Set-Location ..\..
