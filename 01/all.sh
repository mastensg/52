for i in *.timer
do
    BASE=$(basename $i .timer)
    make ${BASE}.pdf
done
