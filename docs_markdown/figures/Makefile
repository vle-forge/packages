
.PHONY : clean

SVGS_WITHOUT_EXT = $(patsubst %.svg,%,$(wildcard *.svg))
SVGS_WITH_PNG_EXT = $(patsubst %.svg,%.png,$(wildcard *.svg))

all: allpng

allpng: $(SVGS_WITH_PNG_EXT)

%.png: %.svg
	inkscape -z $< --export-png=$@  --export-background="rgb(255, 255, 255)" --export-background-opacity=1.0

clean: 
	rm -rf *.png

upload:
	scp -P 6999 *.png trepos@www.vle-project.org:/cross/pub/2.0/docs

