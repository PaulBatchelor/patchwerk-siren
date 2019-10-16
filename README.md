# patchwerk-siren
A simple siren patch made in patchwerk, using local
versions of Soundpipe + Patchwerk. This repo serves as a
proof-of-concept for portable builds of soundpipe +
patchwerk.

## Building
To build, clone this repo and go into the directory.

Then run:

git submodule init
git submodule update
make

With any luck, there should be an executable called
"siren".  Running this program on the terminal should
generate an audio file called "siren.wav".

## What happens during the build

Before the siren example code can be compiled, a few things
must be done.

First, some code needs to be dynamically generated.

Soundpipe generates `soundpipe.h` using a makefile recipe
found in the Soundpipe makefile. It concatenates all the
header files into one header, along with a header guard.

Patchwerk code needs to be tangled into two files:
patchwerk.c and patchwerk.h. This is done using CWEB's
`ctangle` utility, a program that usually ships with a
standard texlive distribution. (for those who do not have
or wish to install texlive, ctangle can also be compiled
separately, but this is currently beyond the scope of this
project.

Finally, select bits of code from the soundpipe + patchwerk
projects are built. For patchwerk, this includes the
main patchwerk C and header files, as well as some of
the soundpipe-patchwerk nodes found in the DSP folder.
The corresponding soundpipe modules are built, alongside
any third party libraries internal to soundpipe.

For more information on the build process, see the
Makefile.
