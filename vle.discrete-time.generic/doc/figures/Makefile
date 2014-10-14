
.PHONY : clean

SVGS_WITHOUT_EXT = $(patsubst %.svg,%,$(wildcard *.svg))
SVGS_WITH_PDF_EXT = $(patsubst %.svg,%.pdf,$(wildcard *.svg))

figs: allpdf

allpdf: $(SVGS_WITH_PDF_EXT)

%.pdf: %.svg
	inkscape -z $< --export-pdf=$@
	


