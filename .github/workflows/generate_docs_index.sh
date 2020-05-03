#!/bin/bash
set -euo pipefail

dirs() {
    pushd "$1" &> /dev/null || return
    for d in *; do
        [ -d "${d}" ] && echo "${d}"
    done
    popd &> /dev/null
}

language_links() {
    local dir="$1"
    local file="$2"
    for lang in $(dirs "${dir}" | sort); do
        echo "<a href=\"${dir}/${lang}/${file}\">${lang}</a>"
    done
}

title="Generated documentation for BibleTime"
cat << EOF
<!DOCTYPE html>
<html lang="en">
<head><title>${title}</title></head>
<body>
<h1>${title}</h1>
EOF

cd docs
for version in $(dirs '.' | sort -rV); do
    echo "<h2>BibleTime ${version#v}</h2>"
    echo '<p>Handbooks (HTML):'
    language_links "${version}/handbook/html" 'index.html'
    echo '<br>Handbooks (PDF):'
    language_links "${version}/handbook/pdf" 'handbook.pdf'
    echo '</p><p>Howtos (HTML):'
    language_links "${version}/howto/html" 'index.html'
    echo '<br>Howtos (PDF):'
    language_links "${version}/howto/pdf" 'howto.pdf'
    echo '</p>'
done

cat << EOF
</body>
</html>
EOF
