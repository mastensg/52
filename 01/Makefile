%.tex: %.timer
	python timer2tex.py $*.timer

%.pdf: %.tex
	pdflatex $*.tex >/dev/null
	rm $*.aux
	rm $*.log
