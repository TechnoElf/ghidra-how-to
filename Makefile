.PHONY: all exercises presentation clean

all: exercises presentation

exercises:
	$(MAKE) -C exercises

presentation:
	typst compile presentation/main.typ presentation.pdf

clean:
	$(MAKE) -C exercises clean
	rm presentation.pdf
