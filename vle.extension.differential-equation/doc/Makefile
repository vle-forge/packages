
.PHONY = figs clean

all: figs differential-equation.pdf
	
figs: figures	
	$(MAKE) -C figures figs
	
clean:
	rm differential-equation.pdf ; rm *.aux ; rm *.bbl ; rm *.blg ; rm *.log ; rm *.out ; rm *.toc

differential-equation.pdf: differential-equation.tex biblio.bib
	pdflatex differential-equation.tex
	bibtex differential-equation
	pdflatex differential-equation.tex
	pdflatex differential-equation.tex

