/*
 * Siren
 *
 * This code will generate a simple siren patch using
 * some of the pre-made patchwerk-wrapped soundpipe dsp
 * nodes found included in the patchwerk codebase.
 *
 * The patch is equivalent to the following runt code:
 *
 * == BEGIN RUNT CODE ==
 *
 * patchwerk nodes
 *
 * 0.1 1 sine 0 1 biscale bhold 0 cabset
 *
 * 0 cabget 100 600 scale 0.5 blsaw
 *
 * 0 cabget 100 2000 scale butlp
 *
 * bdup
 * bdup 0.97 10000 revsc bdrop 0.1 mul dcblock
 *
 * 0 cabget bunhold
 *
 * add
 *
 * "siren.wav" wavout bdrop
 *
 * sr 10 * _compute rep
 *
 * == END RUNT CODE ==
 *
 * It is advisable to understand how the runt code above
 * works before attempting to parse out the C code below.
 * That way, the runt code can be used as a sort of Rosetta
 * Stone. Comments in the C program will break up the
 * program by Runt statements. With any luck, a reader
 * should begin to understand connection between the
 * Patchwerk C library and Runt abstraction.
 *
 * Even with the DSP wrapper code, one can see from
 * this small program that the Patchwerk C API at this level
 * is quite repetive and redundant. In practice, it is best
 * to write abstractions on top of this and not mess with
 * these C operations directly. Otherwise, it is slow and
 * tedious work.
 *
 * When building up a patchwerk patch, the programmer must
 * be able to keep track of what is on the buffer stack at
 * all times. Missing a push or pop operation can cause the
 * entire patch to break. At the C level, these can be very
 * tedious to debug! It is highly recommended to express the
 * Patch using some sort of postfix notation like Sporth or
 * Runt. Presenting the patch in this way will naturally
 * align the stack operations.
 *
 */

#include <stdlib.h>
#include <soundpipe.h>
#include "patchwerk.h"

#include "dsp/sine.h"
#include "dsp/wavout.h"
#include "dsp/biscale.h"
#include "dsp/blsaw.h"
#include "dsp/scale.h"
#include "dsp/butlp.h"
#include "dsp/revsc.h"
#include "dsp/mul.h"
#include "dsp/dcblock.h"
#include "dsp/add.h"

#define NBUFS 8
#define STACKSIZE 10
#define SR 44100
#define BLKSIZE 64

static add_d * mk_add(pw_patch *patch,
                      sp_data *sp)
{
    pw_node *node;
    add_d *add;
    pw_patch_new_node(patch, &node);
    node_add(node, sp);
    pw_node_setup(node);
    add = pw_node_get_data(node);
    return add;
}

static dcblock_d * mk_dcblock(pw_patch *patch,
                              sp_data *sp)
{
    pw_node *node;
    dcblock_d *dcblock;
    pw_patch_new_node(patch, &node);
    node_dcblock(node, sp);
    pw_node_setup(node);
    dcblock = pw_node_get_data(node);
    return dcblock;
}

static mul_d * mk_mul(pw_patch *patch,
                      sp_data *sp)
{
    pw_node *node;
    mul_d *mul;
    pw_patch_new_node(patch, &node);
    node_mul(node, sp);
    pw_node_setup(node);
    mul = pw_node_get_data(node);
    return mul;
}

static revsc_d * mk_revsc(pw_patch *patch,
                          sp_data *sp)
{
    pw_node *node;
    revsc_d *revsc;
    pw_patch_new_node(patch, &node);
    node_revsc(node, sp);
    pw_node_setup(node);
    revsc = pw_node_get_data(node);
    return revsc;
}

static butlp_d * mk_butlp(pw_patch *patch,
                          sp_data *sp)
{
    pw_node *node;
    butlp_d *butlp;
    pw_patch_new_node(patch, &node);
    node_butlp(node, sp);
    pw_node_setup(node);
    butlp = pw_node_get_data(node);
    return butlp;
}

static blsaw_d * mk_blsaw(pw_patch *patch,
                          sp_data *sp)
{
    pw_node *node;
    blsaw_d *blsaw;
    pw_patch_new_node(patch, &node);
    node_blsaw(node, sp);
    pw_node_setup(node);
    blsaw = pw_node_get_data(node);
    return blsaw;
}

static scale_d * mk_scale(pw_patch *patch,
                          sp_data *sp)
{
    pw_node *node;
    scale_d *scale;
    pw_patch_new_node(patch, &node);
    node_scale(node, sp);
    pw_node_setup(node);
    scale = pw_node_get_data(node);
    return scale;
}

static biscale_d * mk_biscale(pw_patch *patch,
                              sp_data *sp)
{
    pw_node *node;
    biscale_d *biscale;
    pw_patch_new_node(patch, &node);
    node_biscale(node, sp);
    pw_node_setup(node);
    biscale = pw_node_get_data(node);
    return biscale;
}

static sine_d * mk_sine(pw_patch *patch,
                        sp_data *sp)
{
    pw_node *node;
    sine_d *sine;
    pw_patch_new_node(patch, &node);
    node_sine(node, sp);
    pw_node_setup(node);
    sine = pw_node_get_data(node);
    return sine;
}

static wavout_d * mk_wavout(pw_patch *patch,
                            sp_data *sp,
                            const char *filename)
{
    pw_node *node;
    wavout_d *wavout;

    pw_patch_new_node(patch, &node);
    node_wavout(sp, node, filename);
    pw_node_setup(node);
    wavout = pw_node_get_data(node);
    return wavout;
}

int main(int argc, char *argv[])
{
    sp_data *sp;
    pw_patch *patch;
    sine_d *sine;
    pw_stack *stack;
    wavout_d *wavout;
    unsigned int n;
    pw_buffer *buf;
    pw_cable *lfo;
    biscale_d *biscale;
    blsaw_d *blsaw;
    scale_d *scale;
    butlp_d *butlp;
    revsc_d *revsc;
    mul_d *mul;
    add_d *add;
    dcblock_d *dcblock;


    /* initialize + allocate */

    sp_create(&sp);

    patch = calloc(1, pw_patch_size());
    pw_patch_init(patch, BLKSIZE);
    pw_patch_alloc(patch, NBUFS, STACKSIZE);
    pw_patch_srate_set(patch, SR);
    sp->sr = pw_patch_srate_get(patch);
    pw_patch_data_set(patch, sp);
    stack = pw_patch_stack(patch);

    /* 0.1 1 sine */
    sine = mk_sine(patch, sp);
    pw_cable_set_value(sine->freq, 0.1);
    pw_cable_set_value(sine->amp, 1);


    /* 0 1 biscale */

    pw_stack_pop(stack, NULL);

    biscale = mk_biscale(patch, sp);

    pw_cable_connect(sine->out, biscale->in);
    pw_cable_set_value(biscale->min, 0);
    pw_cable_set_value(biscale->max, 1);

    /* bhold 0 cabset */

    pw_patch_bhold(patch, &buf);
    pw_stack_pop(stack, NULL);
    lfo = biscale->out;

    /* 0 cabget 100 600 scale */

    pw_stack_push_buffer(stack, buf);

    pw_stack_pop(stack, NULL);
    scale = mk_scale(patch, sp);
    pw_cable_connect(lfo, scale->in);
    pw_cable_set_value(scale->min, 100);
    pw_cable_set_value(scale->max, 600);

    /* 0.5 blsaw */

    pw_stack_pop(stack, NULL);

    blsaw = mk_blsaw(patch, sp);
    pw_cable_set_value(blsaw->amp, 0.5);
    pw_cable_connect(scale->out, blsaw->freq);

    /* 0 cabget 100 2000 scale butlp */

    pw_stack_push_buffer(stack, buf);
    pw_stack_pop(stack, NULL);

    scale = mk_scale(patch, sp);
    pw_cable_connect(lfo, scale->in);
    pw_cable_set_value(scale->min, 100);
    pw_cable_set_value(scale->max, 2000);


    /* butlp */

    pw_stack_pop(stack, NULL);
    pw_stack_pop(stack, NULL);
    butlp = mk_butlp(patch, sp);
    pw_cable_connect(scale->out, butlp->p_freq);
    pw_cable_connect(blsaw->out, butlp->in);

    /* bdup */

    pw_stack_dup(stack);

    /* bdup 0.97 10000 revsc */
    pw_stack_dup(stack);
    pw_stack_pop(stack, NULL);
    pw_stack_pop(stack, NULL);
    revsc = mk_revsc(patch, sp);
    pw_cable_connect(butlp->out, revsc->in[1]);
    pw_cable_connect(butlp->out, revsc->in[0]);
    pw_cable_set_constant(revsc->feedback, 0.97);
    pw_cable_set_constant(revsc->lpfreq, 10000);

    /* bdrop */

    pw_stack_pop(stack, NULL);

    /* 0.1 mul */

    pw_stack_pop(stack, NULL);
    mul = mk_mul(patch, sp);
    pw_cable_set_constant(mul->in1, 0.1);
    pw_cable_connect(revsc->out[0], mul->in2);

    /* dcblock */
    pw_stack_pop(stack, NULL);
    dcblock = mk_dcblock(patch, sp);
    pw_cable_connect(mul->out, dcblock->in);

    /* add */

    pw_stack_pop(stack, NULL);
    add = mk_add(patch, sp);
    pw_cable_connect(butlp->out, add->in1);
    pw_cable_connect(dcblock->out, add->in2);

    /* 0 cabget bunhold */

    pw_patch_bunhold(patch, buf);

    /* siren.wav wavout bdrop */

    pw_stack_pop(stack, NULL);
    wavout = mk_wavout(patch, sp, "siren.wav");
    pw_cable_connect(add->out, wavout->in);
    pw_stack_pop(stack, NULL);

    /* sr 10 * _compute rep */

    for (n = 0; n < 10 * SR; n++) {
        pw_patch_tick(patch);
    }

    /* cleanup */

    pw_patch_destroy(patch);
    pw_patch_free_nodes(patch);
    free(patch);
    sp_destroy(&sp);
    return 0;
}
