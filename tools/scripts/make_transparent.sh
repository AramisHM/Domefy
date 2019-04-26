for file in *.png; do convert "$file" -transparent black "./transparentOutput/${file/%ext/out}"; done
