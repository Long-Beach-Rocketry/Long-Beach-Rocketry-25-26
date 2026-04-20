$plugin = Join-Path $PWD "external/nanopb/generator/protoc-gen-nanopb.bat"
$venvBin = Join-Path $PWD ".venv/bin"

if (Test-Path $venvBin) {
    $env:PATH = "$venvBin;$env:PATH"
}

protoc --plugin=protoc-gen-nanopb="$plugin" --proto_path=common/core/protobuff --proto_path=external/nanopb/generator/proto --nanopb_out=common/core/protobuff/generated common/core/protobuff/protobuff.proto
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$generatedFiles = @(
    (Join-Path $PWD "common/core/protobuff/generated/protobuff.pb.c"),
    (Join-Path $PWD "common/core/protobuff/generated/protobuff.pb.h")
)

foreach ($generated in $generatedFiles) {
    if (-not (Test-Path $generated)) {
        Write-Error "Expected generated file missing: $generated"
        exit 1
    }
}

Write-Host "Generated common/core/protobuff/generated/protobuff.pb.c and common/core/protobuff/generated/protobuff.pb.h"
exit 0
    exit 1
