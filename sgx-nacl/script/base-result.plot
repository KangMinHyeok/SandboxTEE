load 'script/style.gnu'

set term postscript
set datafile separator ","
set key left top

indir = "./data/"
outdir = "./figures/" 
set style fill solid 
set boxwidth 0.8


infile = indir . "1000.csv"
outfile = outdir . "1000.ps"
set output outfile

set style data histograms
plot infile using 2:xtic(1) lt rgb rgbs1[1] title "base",\
	"" using 3 lt rgb rgbs1[2] title "graphene",\
	"" using 4 lt rgb rgbs1[3] title "nacl",\
	"" using 5 lt rgb rgbs1[4] title "pave",\

infile = indir . "10000.csv"
outfile = outdir . "10000.ps"
set output outfile


set style data histograms
plot infile using 2:xtic(1) lt rgb rgbs1[1] title "base",\
	"" using 3 lt rgb rgbs1[2] title "graphene",\
	"" using 4 lt rgb rgbs1[3] title "nacl",\
	"" using 5 lt rgb rgbs1[4] title "pave",\

set logscale y 10


infile = indir . "1000.csv"
outfile = outdir . "1000_log.ps"
set output outfile

set style data histograms
plot infile using 2:xtic(1) lt rgb rgbs1[1] title "base",\
	"" using 3 lt rgb rgbs1[2] title "graphene",\
	"" using 4 lt rgb rgbs1[3] title "nacl",\
	"" using 5 lt rgb rgbs1[4] title "pave",\

infile = indir . "10000.csv"
outfile = outdir . "10000_log.ps"
set output outfile


set style data histograms
plot infile using 2:xtic(1) lt rgb rgbs1[1] title "base",\
	"" using 3 lt rgb rgbs1[2] title "graphene",\
	"" using 4 lt rgb rgbs1[3] title "nacl",\
	"" using 5 lt rgb rgbs1[4] title "pave",\

unset logscale y


infile = indir . "1000.csv"
outfile = outdir . "1000_per.ps"
set output outfile

set style data histograms
plot infile using ($3/$2):xtic(1) lt rgb rgbs1[2] title "graphene",\
	"" using ($4/$2) lt rgb rgbs1[3] title "nacl",\
	"" using ($5/$2) lt rgb rgbs1[4] title "pave",\

infile = indir . "10000.csv"
outfile = outdir . "10000_per.ps"
set output outfile

set style data histograms
plot infile using ($3/$2):xtic(1) lt rgb rgbs1[2] title "graphene",\
	"" using ($4/$2) lt rgb rgbs1[3] title "nacl",\
	"" using ($5/$2) lt rgb rgbs1[4] title "pave",\

