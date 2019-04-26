for file in *.png; do convert "$file" -resize 800x410\! "./resizeOutput/${file/%ext/out}"; done
