
# you should use the title of the article instead
ARTICLE_TITLE = Robust Automatic Threshold Selection

# spaces are not allowed
empty:=
space:= $(empty) $(empty)
PDF_NAME:= $(subst $(space),_,$(ARTICLE_TITLE))

NAME = Article

BIBS=$(wildcard *.bib)

PNGS=$(notdir $(wildcard ../images/*.png))
JPGS=$(notdir $(wildcard ../images/*.jpg))
DIAS=$(notdir $(wildcard ../images/*.dia))
FIGS=$(notdir $(wildcard ../images/*.fig))
EPSS=$(notdir $(wildcard ../images/*.eps))
TIFS=$(notdir $(wildcard ../images/*.tif))
PLOTS=$(notdir $(wildcard ../images/*.gnuplot))

OUTPUT_EPS=$(PNGS:png=eps) $(JPGS:jpg=eps) $(DIAS:dia=eps) $(FIGS:fig=eps) $(TIFS:tif=eps) $(PLOTS:gnuplot=eps) $(EPSS)

all: pdf


ps: ${NAME}.ps
pdf: ${PDF_NAME}.pdf
dvi: ${NAME}.dvi
bbl: ${NAME}.bbl
eps: $(OUTPUT_EPS)


%.eps: ../images/%.png
	convert $< $@

%.eps: ../images/%.jpg
	convert $< $@

%.eps: ../images/%.dia
	dia --nosplash -e $@ $<

%.eps: ../images/%.fig
	fig2dev -L eps $< $@

%.eps: ../images/%.gnuplot
	gnuplot $< > $@


${NAME}.ps: ${NAME}.dvi
	dvips -R0 -o ${NAME}.ps ${NAME}.dvi

%.eps: ../images/%.tif
	convert $< $@


${PDF_NAME}.pdf: ${NAME}.ps
	ps2pdf -dMaxSubsetPct=100 -dCompatibilityLevel=1.3 \
         -dSubsetFonts=true -dEmbedAllFonts=true \
         -dAutoFilterColorImages=false \
         -dColorImageFilter=/FlateEncode -dGrayImageFilter=/FlateEncode \
	 -dMonoImageFilter=/FlateEncode \
	 ${NAME}.ps ${PDF_NAME}.pdf


${NAME}.bbl: ${BIBS} ${NAME}.aux
	bibtex ${NAME}


${NAME}.dvi: ${NAME}.tex  ${BIBS}
	$(MAKE) eps
	latex -interaction=nonstopmode ${NAME}.tex
	bibtex ${NAME}
	latex -interaction=nonstopmode ${NAME}.tex
	latex -interaction=nonstopmode ${NAME}.tex


${NAME}.aux: ${NAME}.tex  ${BIBS}
	latex ${NAME}.tex


clean:
	rm -f ${NAME}.dvi ${NAME}.ps ${PDF_NAME}.pdf ${NAME}.aux ${NAME}.log ${NAME}.bbl ${NAME}.blg ${NAME}.toc ${NAME}.out  ${NAME}.brf $(DIAS:dia=eps) $(PNGS:png=eps) $(FIGS:fig=eps)

