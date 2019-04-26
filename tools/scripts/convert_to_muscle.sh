for file in *.bmp; do ./muscle "$file" "./MuscleOutput/${file/%ext/o
ut}"; done
