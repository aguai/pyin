#!/bin/bash

set -u

testdir=$(dirname "$0")
rootdir="$testdir/../.."

if [ ! -d "$rootdir/testdata" ]; then
    echo "No testdata directory here, skipping regression test"
    exit 0
fi

if ! sonic-annotator -v >/dev/null ; then
    echo "No sonic-annotator available in PATH, skipping regression test"
    exit 0
fi

echo "Running regression test..."

for output in smoothedpitchtrack notes ; do
    for fixedlag in off on ; do

        flsuffix=""
        if [ "$fixedlag" = "on" ]; then
            flsuffix="-fixedlag"
        fi

        echo
        echo "Testing output $output with fixed-lag $fixedlag..."
    
        ( time ( VAMP_PATH="$rootdir" sonic-annotator \
	                  -t "$testdir/transform-$output$flsuffix.ttl" \
	                  -w csv --csv-stdout --csv-omit-filename \
	                  "$rootdir/testdata/bob_02.wav" \
	                  > "$testdir/obtained-$output$flsuffix.csv" \
	                  2> "$testdir/log-$output$flsuffix.txt" ) ) 2>&1 | \
            grep -i real | \
            sed 's/^real/Elapsed time/'

        if ! cmp -s "$testdir/expected-$output$flsuffix.csv" "$testdir/obtained-$output$flsuffix.csv" ; then
            echo "*** FAILED for output $output with fixed-lag $fixedlag, diff follows:"
            echo "    (expected on left, obtained on right)"
            sdiff -w78 "$testdir/expected-$output$flsuffix.csv" "$testdir/obtained-$output$flsuffix.csv"
        else
            echo "Succeeded for output $output with fixed-lag $fixedlag"
        fi
    done
done


