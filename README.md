# BioMeasureComparison
Bioinformatics measure comparison framework with several implemented
measure functions.

This software is under active development and you should expect it to
be buggy.  It might or might not work, and certainly is not guaranteed
useable for any purpose whatsoever.

This software is for the paper to be written and will have a citation
here.

## Prerequisites

The following are needed:

  * C++ at least standard C++11.  The code is tested with clang++
  4.0.1 and g++ 7.1.1.
  * Boost development libaries.  For my system, this is `boost_1_64-devel`.
  * Boost [edit distance](https://github.com/erikerlandson/edit_distance)
  This needs to be installed as a subdirectory that matches `INC` in the
  `Makefile`.  I use the default of `edit_distance`.
  * If you want to create a text version of this README, you need
  `pandoc`.

## Compiling

The `Makefile` should work on recent enough systems.  See comments in
the top of the file for choosing a compiler, etc.  If it does not work,
fix it and sent a pull request.

## Running

The program will checkpoint after every row of distance matrix
calculation.  You need to create the directory for the checkpointing
*before* running.  The default is `metrictest.checkpoint`.

### Sample data files to try

These data files are *not mine*, but they are in the `data/` directory
for experimentation and testing.

  * [`AF091148`](https://github.com/torognes/vsearch-data/blob/master/AF091148.fsa)
  A small (1408 sequences) FASTA file that works well for testing since
  it tends to be fast.
  * `1688_seqs_nophix` a set of sequences from the paper [Open-Source
  Sequence Clustering Methods Improve the State Of the
  Art](http://msystems.asm.org/content/1/1/e00003-15).
  To save you reading the paper, you can [download the FASTQ file
  here](https://www.dropbox.com/sh/atmpqqm9zxwf6qf/AAB9MPLGTh6ZWWBAV1HSWNoca?dl=0).
  You have to convert from FASTQ to FASTA format; [many conversion
  utilities exist](https://www.google.com/search?q=convert+FASTA+to+FASTQ).
  This dataset is claimed to come from the Bokulich et al. paper
  [Quality-filtering vastly improves diversity estimates from Illumina
  amplicon sequencing](http://dx.doi.org/10.1038/nmeth.2276).  I have
  done this for the version in the `data/` directory.

### Command-line options

The definitive option description is in `Options.h`.  This summary
might be out-of-date.

  * `--restart` restart from a checkpoint.  The default is to not
  restart.  If you are restarting, you must ensure that the
  `checkpointdir` is correct.  `checkpointdir` is the only command-line
  option that is valid when restarting.
  
  All other command-line options will come from the checkpoint; you
  cannot change them mid-run.
  * `--fasta=foo` Read [FASTA-format](https://en.wikipedia.org/wiki/FASTA_format) sequences from the file `foo`.  Required.
  * `--measure=foo` The distance measure to use (`foo` in this example).
  This must be a value known to createmetric.cpp.  Required.
  * `--submeasure=foo` Some distance measurements have submeasures.  For
  example, kmer distances can be calculated via Euclidean or cosine
  variants.  The measure function you use must understand the submeasure
  you supply.  Optional, depending on the distance measure you use.
  * `--measureopt=foo` Supply the option `foo` to the measure function.
  For example, kmers need to know the value for k.  Optional, depending
  on the distance measure you use.
  * `--distmatfname=foo` Write the resulting distance matrix to the file
  `foo`.  Required.  No default.
  * `--ncores=n` use _n_ threads.  The max (and default) value is the
  number of cores that the system has.  Optional.
* `--checkpointdir=foo` Write all checkpoint information to files in
the directory `foo`.
* `--printresult=true|false` Whether or not to print the resulting distance
matrix.  For a matrix of any size, it is impractical to print.  The
default is `false`.  If you set this to `true` then the result is
printed.

### Sample command lines

`./metrictest --measure=kmer --submeasure=cosine --measureopt=7 --fasta=data/AF091148.fasta --distmatfname=AF091148-7mercosine-distances`

`./metrictest --measure=kmer --submeasure=euclidean --measureopt=7 --fasta=data/AF091148.fasta --distmatfname=AF091148-7mereuclidean-distances`

`./metrictest --ncores=7 --measure=edit --fasta=data/AF091148.fasta --distmatfname=AF091148-edit-distances`

`./metrictest --measure=edit --measureopt=pam250ish --fasta=data/AF091148.short.fasta --distmatfname=AF091148-edit-distances`  

This last one uses edit weights from the file `pam250ish` (which must
exist before you run it).  That file is not included due to questions
about the biological meanings of the values in it.

### Edit cost

If you want to use the edit distance with weights, you need to create a
weight file.  This file contains 16 (4x4 in order of ACGT for both rows
and columns) long double values that are the cost of a mutation from
one base to the other.  Numbers are separated by white space; I put four
values per line.  The matrix *must* be symmetric.  A cost of 0 for the
diagonal is a good idea, but will never be used (why have a cost for
doing nothing?).

## Measure functions

The following functions currently exist:

  * Measure `edit` uses Levenshtein distance between sequences.  The
  default is unit cost per operation (insertion, deletion, substitution).
  You can provide a cost matrix in the file `foo` with the
  `--measureopt=foo` command-line option.
  * Measure `kmer` uses k-mers.  You must supply a value for _k_ by
  using `--measureopt=k`.  You must supply a `--submeasure=foo`
  where `foo` is either `euclidean` or `cosine`.

    * Euclidean is currently Eculidean squared, as described in [K-mer
    based distance estimation](http://resources.qiagenbioinformatics.com/manuals/phylogenymodule/current/K_mer_based_distance_estimation.html)

    * Cosine is described in [the Wikipedia page](https://en.wikipedia.org/wiki/Cosine_similarity)
    and (probaby?) used in Apostolico, A; Denas, O (March 2008). _[Fast
    algorithms for computing sequence distances by exhaustive substring
    composition.](https://doi.org/10.1186/1748-7188-3-13)_
