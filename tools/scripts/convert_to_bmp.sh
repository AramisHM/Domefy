for file in *.png; do convert "$file" "output/${file/%ext/o
ut}.bmp"; done

