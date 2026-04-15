param (
    [Parameter(Mandatory=$true)][string]$t,
    [Parameter(Mandatory=$false)][string]$a,
    [switch]$c=$false,
    [switch]$r=$false
)

if ( $c )
{
    echo "Performing clean build..."
    rm -R -Force build/$t
}

$mode="Debug"

if ( $r )
{
    $mode="Release"
}

cmake --preset=$t -DTARGET_APP="$a" -DCMAKE_BUILD_TYPE="$mode"
pushd build/$t
cmake --build .
popd