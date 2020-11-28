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

#center1 = 1
#center10 = 2
#center100 = 3
#set xtics ('1KB' center1, '10KB' center10, '100KB' center100)
set xtics nomirror
set logscale y 10

infileR = indir . "results_R.csv"
infileW = indir . "results_W.csv"
outfile = outdir . "results_log.ps"
set output outfile

set style data histograms
plot\
 	infileR using 2:xtic(1) lt rgb rgbs1[1] fs pattern 1 title "Base(read)",\
	infileR using 5 lt rgb rgbs1[4] fs pattern 1 title "SGX+NaCl(read)",\
	infileW using 2 lt rgb rgbs1[1] fs pattern 2 title "Base(write)",\
	infileW using 5 lt rgb rgbs1[4] fs pattern 2 title "SGX+NaCl(write)",\
	#"" using 4 lt rgb rgbs1[2] title "SGX",\
	#"" using 3 lt rgb rgbs1[3] title "NaCl",\

unset logscale y

#set xtics ('1KB' center1, '10KB' center10, '100KB' center100)
outfile = outdir . "results_per.ps"
set output outfile

set yrange [0:2.5]
set arrow from -1, 2 to 4, 2 nohead lc "red" dt 2 lw 3
set style data histograms
set title "File I/O performance ratio\n(SGX+NaCl)/Base"
plot\
	infileR using ($5/$2):xtic(1) lt rgb rgbs1[4] fs pattern 1 title "SGX+NaCl(read)",\
	infileW using ($5/$2) lt rgb rgbs1[4] fs pattern 2 title "SGX+NaCl(write)",\
	#infile using ($4/$2) lt rgb rgbs1[2] title "SGX",\
	infileR using ($3/$2) lt rgb rgbs1[3] title "NaCl",\
	infileW using ($3/$2) lt rgb rgbs1[3] title "NaCl",\
	
