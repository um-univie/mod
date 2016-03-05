# settings
JOBNAME = document
# set 'withfmt' to something to use a format file


PYFILE = ${JOBNAME}-mod.py
TEXFILE = ${JOBNAME}-mod.tex
MOD = mod -f ${PYFILE}
PDFLATEX = pdflatex -interaction=nonstopmode -halt-on-error
BIB = bibtex ${JOBNAME}
ifdef withfmt
FMTFILE = ${JOBNAME}.fmt
RUN = $(PDFLATEX) --synctex=1 -fmt ${FMTFILE} ${JOBNAME}.tex
else
FMTFILE = 
RUN = $(PDFLATEX) --synctex=1 ${JOBNAME}.tex
endif


.PHONY: single
single: realSingle ${TEXFILE}

.PHONY: realSingle
realSingle: ${FMTFILE}
	$(RUN)
	$(BIB)

.PHONY: complete
complete: clean single
	$(RUN) && $(RUN)

.PHONY: final
final: complete
	$(RUN) && $(RUN) && $(RUN) && $(RUN) && $(RUN) && $(RUN)

${TEXFILE}: ${PYFILE}.cp
	$(MOD) || rm -f ${PYFILE}.cp ${TEXFILE}

${PYFILE}.cp: FORCE
	test -e $@ || cp ${PYFILE} $@
	diff $@ ${PYFILE} > /dev/null || cp ${PYFILE} $@

.PHONY: FORCE
FORCE:

.PHONY: clean
clean: clean-aux
	rm -f ${JOBNAME}.pdf

.PHONY: clean-aux
clean-aux: clean-mod-aux
	rm -f *.log *.out *.bbl *.blg *.synctex.gz *.lof *.lot *.toc *.tdo *.fmt
	find . -iname "*.aux" | xargs rm -f

.PHONY: clean-mod-aux
clean-mod-aux:
	rm -f ${PYFILE} ${TEXFILE} ${PYFILE}.cp
