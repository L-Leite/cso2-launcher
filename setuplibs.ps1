# setup PolyHook 2
Set-Location .\libs\PolyHook_2_0

$extraCmakeCfg = '
if(MSVC)
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

# setup capstone project for PolyHook
Set-Location .\capstone
New-Item -Type Directory -Path build32
Set-Location .\build32
cmake -G "Visual Studio 15 2017" -DCAPSTONE_ARM_SUPPORT=0 -DCAPSTONE_ARM64_SUPPORT=0 -DCAPSTONE_M680X_SUPPORT=0 -DCAPSTONE_M68K_SUPPORT=0 -DCAPSTONE_MIPS_SUPPORT=0 -DCAPSTONE_PPC_SUPPORT=0 -DCAPSTONE_SPARC_SUPPORT=0 -DCAPSTONE_SYSZ_SUPPORT=0 -DCAPSTONE_XCORE_SUPPORT=0 -DCAPSTONE_TMS320C64X_SUPPORT=0 -DCAPSTONE_M680X_SUPPORT=0 -DCAPSTONE_EVM_SUPPORT=0 ../

# now PolyHook
Set-Location ..\..
cmake -G "Visual Studio 15 2017" -DBUILD_DLL=ON -DBUILD_STATIC=ON -DFEATURE_INLINENTD=OFF .\

# setup source sdk
Set-Location ..\source-sdk-cso2
.\createlauncher.bat

# get back to the project's root
Set-Location ..\..
