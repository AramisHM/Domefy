for file in *.bmp; do convert "$file" -type truecolor "output/${file/%ext/o
ut}.bmp"; done

