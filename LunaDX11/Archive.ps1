$ExcludeFolderNames = @("x64")

$ScriptPath = $MyInvocation.MyCommand.Path
$ScriptDir = Split-Path $ScriptPath
$ArchiveName = "Project"

function IsExcluded($filePath) {
    foreach ($ExcludeFolderName in $ExcludeFolderNames) {
        if ($filePath.StartsWith((Join-Path $ScriptDir $ExcludeFolderName))) {
            return $true
        }
    }
    return $false
}

$FilesToArchive = Get-ChildItem -Path $ScriptDir -Recurse -File | Where-Object {
    ($_.FullName -ne $ScriptPath) -and
    (-not (IsExcluded($_.FullName)) -and
    ($_.BaseName -ne $ArchiveName))
}

# 임시 폴더 생성
$TempFolder = Join-Path $env:TEMP ("ArchiveTemp_" + [guid]::NewGuid().ToString())
New-Item -Path $TempFolder -ItemType Directory | Out-Null

# 대상 파일 복사
foreach ($file in $FilesToArchive) {
    $relativePath = $file.FullName.Substring($ScriptDir.Length + 1)
    $destPath = Join-Path $TempFolder $relativePath
    New-Item -ItemType Directory -Path (Split-Path $destPath) -Force | Out-Null
    Copy-Item $file.FullName -Destination $destPath
}

# 압축
$ZipPath = Join-Path $ScriptDir $ArchiveName
Compress-Archive -Path "$TempFolder\*" -DestinationPath $ZipPath -Force

# 임시 폴더 정리
Remove-Item $TempFolder -Recurse -Force

Write-Output "압축 완료: $ZipPath"