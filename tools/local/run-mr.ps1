<#
.SYNOPSIS
    Run Engine Simulator with an .mr engine file of your choice.

.DESCRIPTION
    The GUI always compiles <run>\assets\main.mr (it takes no command-line
    arguments) and reloads it when Enter is pressed. This script rewrites
    main.mr to import the chosen file and call its main() node, then starts
    the GUI from <run>\bin.

    Files outside <run>\assets are copied to <run>\assets\custom\ first, so
    Piranha can resolve them. Their own imports ("engine_sim.mr", "themes/...",
    ...) resolve through the normal es\ and assets\ search paths.

.EXAMPLE
    .\run-mr.ps1 engines\alco\alco_251d_diesel_turbo.mr
.EXAMPLE
    .\run-mr.ps1 D:\my_engines\v8.mr -NoLaunch      # GUI already open: press Enter
.EXAMPLE
    .\run-mr.ps1 -List
#>
param(
    [Parameter(Position = 0)] [string] $Path,
    [string] $Node = "main",
    [switch] $NoLaunch,
    [switch] $List,
    [string] $RunRoot = "C:\es\run"
)

$ErrorActionPreference = "Stop"
$assets = Join-Path $RunRoot "assets"
$bin = Join-Path $RunRoot "bin"
$mainMr = Join-Path $assets "main.mr"

if ($List) {
    Get-ChildItem (Join-Path $assets "engines") -Recurse -Filter *.mr | ForEach-Object {
        $hasMain = Select-String -Path $_.FullName -Pattern '^\s*(public\s+)?node\s+main\b' -Quiet
        $rel = $_.FullName.Substring($assets.Length + 1)
        "{0}{1}" -f $rel, $(if ($hasMain) { "" } else { "    (no main node: use -Node)" })
    }
    return
}

if (-not $Path) { throw "Give an .mr path, or -List to see the engines under $assets." }

# Resolve: as given, then relative to assets\, then assets\engines\.
$candidates = @($Path, (Join-Path $assets $Path), (Join-Path (Join-Path $assets "engines") $Path))
$file = $candidates | Where-Object { Test-Path $_ -PathType Leaf } | Select-Object -First 1
if (-not $file) { throw "Not found: $Path (also tried under $assets and $assets\engines)" }
$file = (Resolve-Path $file).Path

$assetsFull = (Resolve-Path $assets).Path
if (-not $file.StartsWith($assetsFull + "\", [StringComparison]::OrdinalIgnoreCase)) {
    $customDir = Join-Path $assets "custom"
    New-Item -ItemType Directory -Force $customDir | Out-Null
    $dest = Join-Path $customDir (Split-Path $file -Leaf)
    Copy-Item $file $dest -Force
    Write-Host "Copied to $dest"
    $file = $dest
}
$importPath = $file.Substring($assetsFull.Length + 1).Replace("\", "/")

$text = Get-Content $file -Raw
$definesNode = $text -match "(?m)^\s*(public\s+)?node\s+$([regex]::Escape($Node))\b"
$callsAtRoot = $text -match "(?m)^$([regex]::Escape($Node))\(\)\s*$"

if ($callsAtRoot) {
    $body = "import `"engine_sim.mr`"`nimport `"themes/default.mr`"`nimport `"$importPath`"`n`nuse_default_theme()`n"
}
elseif ($definesNode) {
    $body = "import `"engine_sim.mr`"`nimport `"themes/default.mr`"`nimport `"$importPath`"`n`nuse_default_theme()`n$Node()`n"
}
else {
    $nodes = [regex]::Matches($text, "(?m)^\s*public\s+node\s+(\w+)") | ForEach-Object { $_.Groups[1].Value }
    throw "$importPath has no node '$Node'. Public nodes: $($nodes -join ', '). Use -Node <name>."
}

if (Test-Path $mainMr) { Write-Host "Previous main.mr: $((Get-Content $mainMr) -join ' | ')" }
[System.IO.File]::WriteAllText($mainMr, $body, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "main.mr -> import `"$importPath`"$(if (-not $callsAtRoot) { "; $Node()" })"

if ($NoLaunch) {
    Write-Host "GUI already running? Press Enter in it to reload."
    return
}

$exe = Join-Path $bin "engine-sim-app.exe"
Start-Process -FilePath $exe -WorkingDirectory $bin
Write-Host "Started $exe"
Write-Host "Telemetry: $RunRoot\logs\telemetry_<engine>_<timestamp>.log (script errors: $bin\error_log.log)"
