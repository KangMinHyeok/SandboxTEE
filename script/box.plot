#load 'script/style.gnu'
set terminal postscript enhanced color eps font "Helvetica, 14"
set size 1, 0.75
set term postscript
set datafile separator ","
set key left top

indir = "./data/"
outdir = "./figures/" 

set grid

set xtics rotate by 45 right
set style fill solid #0.25 border -1
offset = 1.4

set style data boxplot
center1 = 10
center2 = 20
center4 = 30
center8 = 40
set xtics ('1 KB' center1, '2 KB' center2, '4 KB' center4, '8 KB' center8)
set style boxplot fraction 0.95

set boxwidth 2
set pointsize 1
set xtics nomirror

infile1 = indir . "1 KB_R.csv"
infile2 = indir . "2 KB_R.csv"
infile4 = indir . "4 KB_R.csv"
infile8 = indir . "8 KB_R.csv"
outfile = outdir . "R_box.ps"
set output outfile

set title "File Read Latency"
plot\
	infile1 using (center1 - offset):1 ls 3 title "Base",\
	'' using (center1 + offset):4 ls 1 title "SGX+NaCl",\
	infile2 using (center2 - offset):1 ls 3 title "",\
	infile2 using (center2 + offset):4 ls 1 title "",\
	infile4 using (center4 - offset):1 ls 3 title "",\
	infile4 using (center4 + offset):4 ls 1 title "",\
	infile8 using (center8 - offset):1 ls 3 title "",\
	infile8 using (center8 + offset):4 ls 1 title "",\

infile1 = indir . "1 KB_W.csv"
infile2 = indir . "2 KB_W.csv"
infile4 = indir . "4 KB_W.csv"
infile8 = indir . "8 KB_W.csv"
outfile = outdir . "W_box.ps"
set output outfile
set title "File Write Latency"

plot\
	infile1 using (center1 - offset):1 ls 3 title "Base",\
	'' using (center1 + offset):4 ls 1 title "SGX+NaCl",\
	infile2 using (center2 - offset):1 ls 3 title "",\
	infile2 using (center2 + offset):4 ls 1 title "",\
	infile4 using (center4 - offset):1 ls 3 title "",\
	infile4 using (center4 + offset):4 ls 1 title "",\
	infile8 using (center8 - offset):1 ls 3 title "",\
	infile8 using (center8 + offset):4 ls 1 title "",\

