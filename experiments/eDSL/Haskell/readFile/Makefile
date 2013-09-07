# Filename: Makefile (for readFile demo)
# Author: Fernando Freire
# Date created: 7/8/2013

BIN=readFile.out
# The directory to place intermediate GHC files
OUTDIR=clutter


all:
	@ghc -o readFile.out -outputdir $(OUTDIR) readFile.hs

clean:
	@rm -r $(OUTDIR) $(BIN)
