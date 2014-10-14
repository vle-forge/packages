
.PHONY = figs clean

all: figs discrete-time.pdf
	
figs: figures	
	$(MAKE) -C figures figs
	
clean:
	rm discrete-time.pdf ; rm *.aux ; rm *.bbl ; rm *.blg ; rm *.log ; rm *.out ; rm *.toc

discrete-time.pdf: discrete-time.tex biblio.bib
	pdflatex discrete-time.tex
	#bibtex discrete-time
	pdflatex discrete-time.tex
	pdflatex discrete-time.tex

