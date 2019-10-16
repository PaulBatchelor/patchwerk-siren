OBJ=patchwerk/patchwerk.o

OBJ += $(addprefix soundpipe/modules/, \
	base.o\
	ftbl.o\
	osc.o\
	randmt.o\
	scale.o\
	biscale.o\
	wavout.o\
	blsaw.o\
	butlp.o\
	dcblock.o\
	revsc.o\
)

OBJ += $(addprefix soundpipe/lib/, \
	dr_wav/dr_wav.o\
)

OBJ += $(addprefix patchwerk/dsp/, \
	sine.o\
	add.o\
	mul.o\
	scale.o\
	biscale.o\
	wavout.o\
	blsaw.o\
	butlp.o\
	dcblock.o\
	revsc.o\
)

OBJ += siren.o

CFLAGS+=-Ipatchwerk -DNO_LIBSNDFILE

CFLAGS+=-Isoundpipe/lib/dr_wav
CFLAGS+=-Isoundpipe/lib/faust

default: siren

soundpipe/h/soundpipe.h:
	cd soundpipe; make h/soundpipe.h

patchwerk/patchwerk.c:
	cd patchwerk; make patchwerk.c

siren: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

clean:
	cd patchwerk; make clean
	cd soundpipe; make clean
	$(RM) $(OBJ)
	$(RM) siren
