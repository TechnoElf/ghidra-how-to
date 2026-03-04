.PHONY: all exercises presentation datasheet clean

all: exercises presentation datasheet

exercises:
	$(MAKE) -C exercises

presentation:
	typst compile presentation/main.typ presentation.pdf

datasheet:
	typst compile datasheet/main.typ datasheet.pdf

clean:
	$(MAKE) -C exercises clean
	rm presentation.pdf
	rm datasheet.pdf
