# Filename: Makefile (for readFile demo)
# Author: Fernando Freire
# Date created: 7/8/2013

BIN=parseFile.out
# The directory to place intermediate GHC files
OUTDIR=clutter


all:
	@ghc -o parseFile.out -outputdir $(OUTDIR) parseFile.hs

clean:
	@rm -r $(OUTDIR) $(BIN)
