$hash = git log --pretty=format:'%h' -n 1
if (Test-Path src/githash.h)
{
	Remove-Item src/githash.h
}
Add-Content src/githash.h "#define GIT_COMMIT_HASH `"$hash`""
