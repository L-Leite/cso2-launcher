$hash = git log --pretty=format:'%h' -n 1
Write-Output "current git commit hash is $hash"
if (Test-Path launcher/header/githash.hpp)
{
	Remove-Item launcher/header/githash.hpp
}
Add-Content launcher/header/githash.hpp "#define GIT_COMMIT_HASH `"$hash`""
