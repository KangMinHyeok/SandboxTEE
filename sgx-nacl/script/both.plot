load 'script/style.gnu'

set term postscript
set datafile separator ","
set key left top

indir = "./data/"
outdir = "./figures/" 
set style fill solid 
set boxwidth 0.8

set grid

set xtics rotate by 45 right

infile = indir . "results_both.csv"
set logscale y 10


outfile = outdir . "results_both_log.ps"
set output outfile

set style data histograms
plot infile using 2:xtic(1) lt rgb rgbs1[1] title "Base",\
	"" using 3 lt rgb rgbs1[3] title "NaCl",\
	"" using 5 lt rgb rgbs1[4] title "SGX+NaCl",\
	#"" using 4 lt rgb rgbs1[2] title "SGX",\

unset logscale y

outfile = outdir . "results_both_per.ps"
set output outfile

set arrow from -1, 2 to 10, 2 nohead lc "red" dt 2 lw 3
set style data histograms
plot infile using ($3/$2):xtic(1) lt rgb rgbs1[3] title "NaCl",\
	"" using ($5/$2) lt rgb rgbs1[4] title "SGX+NaCl",\
	#infile using ($4/$2):xtic(1) lt rgb rgbs1[2] title "SGX",\
