function SetupVsToolsPath {
    # from https://allen-mack.blogspot.com/2008/03/replace-visual-studio-command-prompt.html

    Push-Location 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build'

    cmd /c "vcvars32.bat&set" |
        ForEach-Object {
            if ($_ -match "=") {
                $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
            }
        }

    Pop-Location
}

function PrintToolsVersion {
    param ([string]$curBuildCombo)

    Write-Host ''
    Write-Host '#'
    Write-Host '# TOOLS VERSIONS'
    Write-Host '#'
    Write-Host ''

    switch ($curBuildCombo) {
        "windows-mingw" {
            Write-Host '# MinGW'
            C:\msys64\mingw64\bin\gcc.exe -v
            break
        }
        "windows-msvc" {
            Write-Host '# MSVC'
            which cl
            cl
            break
        }
        "windows-clang_cl" {
            Write-Host '# Clang-cl'
            which clang-cl
            clang-cl -v
            break
        }
        Default {
            Write-Error 'Unknown build combo used, could not find appropriate compiler.'
            exit 1
        }
    }
    
    Write-Host '# CMake'
    cmake --version

    Write-Host '# Ninja'
    ninja --version

    Write-Host '# Git'
    git --version

    Write-Host ''
    Write-Host '#'
    Write-Host '# END OF TOOLS VERSIONS'
    Write-Host '#'
    Write-Host ''
}

$curBuildCombo = $env:BUILD_COMBO

$isMingwBuild = $curBuildCombo -eq 'windows-mingw'
$isMsvcBuild = $curBuildCombo -eq 'windows-msvc'

Write-Host "Running setup script..."
Write-Host "Current setup build combo is: $curBuildCombo"

if ($isWindows) {
    # install scoop
    Invoke-WebRequest -useb get.scoop.sh | Invoke-Expression

    # install ninja through scoop
    scoop install ninja

    if ($isMingwBuild) {
        # put mingw tools in path
        $mingwAppendPath = ';C:\msys64\mingw32\bin'
        $env:Path += $mingwAppendPath
        [Environment]::SetEnvironmentVariable("Path", $env:Path + $mingwAppendPath, "Machine")
    }

    if ($isMsvcBuild) {     
        # put VS tools in path to print their version
        SetupVsToolsPath
    }
}
else {
    Write-Error 'An unknown OS is running this script, implement me.'
    exit 1
}

Write-Host ''
Write-Host '#'
Write-Host '# Environment path:'
Write-Host '#'
Write-Host ''
Write-Host $env:PATH

# print tools versions
PrintToolsVersion $curBuildCombo

# setup submodules
git submodule update --init --recursive
