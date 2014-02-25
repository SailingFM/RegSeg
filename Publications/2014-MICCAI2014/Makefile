
SUBDIRS = figures
FILENAME = MICCAI2014

all: $(FILENAME).pdf 

.PHONY: $(SUBDIRS)

$(FILENAME).pdf: $(FILENAME).tex $(FILENAME).bbl $(FILENAME).glo $(FILENAME).aux $(SUBDIRS)
	pdflatex -shell-escape $< 
	grep Rerun $(FILENAME).log && pdflatex $< 
	grep Rerun $(FILENAME).log && pdflatex $< 
	touch $(FILENAME).bbl
	touch $@

$(FILENAME).glo: $(FILENAME).aux
	makeglossaries $(FILENAME)

$(FILENAME).bbl: $(FILENAME).aux references.bib splncs.bst
	bibtex $(FILENAME)

$(FILENAME).aux: $(FILENAME).tex $(SUBDIRS)
	pdflatex -shell-escape $<

$(SUBDIRS):
	$(MAKE) -C $@

cleanall: clean
	rm -f *.pdf *.acn *.glo *.ist *.toc *.spl *.dvi *.ps *.fff *.acr *.alg *.glg *.gls

clean:
	rm -f *.aux *.log *.out *.blg *.bbl
