
SUBDIRS = figures 
FILENAME = MICCAI2014

all: $(FILENAME).pdf 

$(FILENAME).pdf: $(FILENAME).tex $(FILENAME).bbl $(FILENAME).aux $(SUBDIRS)
	pdflatex -shell-escape $< 
	grep Rerun $(FILENAME).log && pdflatex $< 
	grep Rerun $(FILENAME).log && pdflatex $< 
	touch $(FILENAME).bbl
	touch $@

$(FILENAME).bbl: $(FILENAME).aux references.bib splncs.bst
	bibtex $(FILENAME)

$(FILENAME).aux: $(FILENAME).tex $(SUBDIRS)
	pdflatex -shell-escape $<

subdirs: $(SUBDIRS)


$(SUBDIRS):
	$(MAKE) -C $@

cleanall: clean
	rm -f *.pdf *.acn *.glo *.ist *.toc *.spl *.dvi *.ps *.fff

clean:
	rm -f *.aux *.log *.out *.blg *.bbl
