function CreateDirectory {
    param( [string]$newDirectory)
    New-Item -ItemType Directory -Path $newDirectory
}

$curBuildCombo = $env:BUILD_COMBO
$curConfig = $env:CONFIGURATION

# only package on Release builds, but don't error out
if ($curConfig -ne 'Release') {
    Write-Host 'Non release build detected, exiting packaging script...'
    exit 0
}

$isMingwBuild = $curBuildCombo -eq 'windows-mingw'
$isMsvcBuild = $curBuildCombo -eq 'windows-msvc'

Write-Host "Running packaging script..."
Write-Host "Current setup build combo is: $curBuildCombo"

# create dir to store package files
CreateDirectory ./build/package

if ($isWindows) {
    if ($isMingwBuild) {
        # copy mingw libraries        
        Copy-Item "C:\msys64\mingw32\bin\libgcc_s_dw2-1.dll" -Destination ./build/package/
        Copy-Item "C:\msys64\mingw32\bin\libstdc++-6.dll" -Destination ./build/package/
        Copy-Item "C:\msys64\mingw32\bin\libwinpthread-1.dll" -Destination ./build/package/
    }

    # copy the launcher itself to the package dir
    Copy-Item ./build/launcher.exe -Destination ./build/package/
}
else {
    Write-Error 'An unknown OS is running this script, implement me.'
    exit 1
}

# copy license
Copy-Item ./COPYING -Destination ./build/package/

# copy README
Copy-Item ./README.md -Destination ./build/package/

# get app version
$versionStr = Get-Content -Path ./version.txt -TotalCount 1
Write-Host "cso2-launcher version: $versionStr"

Push-Location ./build

if ($isWindows) {
    if ($isMingwBuild) {       
        7z a -t7z -m0=lzma2 -mx=9 -mfb=64 -md=64m -ms=on "cso2_launcher-$versionStr-win64_mingw.7z" ./package/*
    }
    elseif ($isMsvcBuild) {       
        7z a -t7z -m0=lzma2 -mx=9 -mfb=64 -md=64m -ms=on "cso2_launcher-$versionStr-win64_msvc.7z" ./package/*
    }
}

Pop-Location
