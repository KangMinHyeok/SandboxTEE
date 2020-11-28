set terminal postscript enhanced color eps font "Helvetica, 14"
set size 1, 0.75
set dashtype 30 (2,6,2,6)
array rgbs1 = ["#85bcff", "#ff4500", "#ffc020", "#9300E2", "#00b200", "#b5001a", "#bd4c00"]
#array rgbs1 = ["#0695FF", "#e41a1c", "#AA00FF", "#3ADE1D", "#F2DF25", "#FFA200"]
array rgbs2 = ["#85bcff", "#ff4500", "#ffc020", "#9300E2", "#00b200", "#b5001a", "#bd4c00"]
#array rgbs2 = ["#21B400", "#E41AD4", "#05CFBD", "#DEA21D", "#DE1D56", "#8EDE1D"]

set style line 1 lc rgb "#85bcff" lw 3 # blue
set style line 2 lc rgb "#ff4500" lw 3 # burnt orange
set style line 3 lc rgb "#ffc020" lw 3 # goldenrod
set style line 4 lc rgb "#856ab0" lw 3 # purple
set style line 5 lc rgb "#00b200" lw 3 # green
set style line 6 lc rgb "#b5001a" lw 3 # red
set style line 7 lc rgb "#bd4c00" lw 3 # blue

set style line 1 lc rgb rgbs1[1] lw 3 # blue
set style line 2 lc rgb rgbs1[2] lw 3 # burnt orange
set style line 3 lc rgb rgbs1[3] lw 3 # goldenrod
set style line 4 lc rgb rgbs1[4] lw 3 # purple
set style line 5 lc rgb rgbs1[5] lw 3 # green
set style line 6 lc rgb rgbs1[6] lw 3 # red

set style line 11 lc rgb rgbs1[1] dashtype 2 lw 3 # blue
set style line 12 lc rgb rgbs1[2] dashtype 2 lw 3 # burnt orange
set style line 13 lc rgb rgbs1[3] dashtype 2 lw 3 # goldenrod
set style line 14 lc rgb rgbs1[4] dashtype 2 lw 3 # purple
set style line 15 lc rgb rgbs1[5] dashtype 2 lw 3 # green
set style line 16 lc rgb rgbs1[6] dashtype 2 lw 3 # red

set style line 21 lc rgb rgbs1[1] lw 3 # blue with opacity
set style line 22 lc rgb rgbs1[2] lw 3 # burnt orange with opacity
set style line 23 lc rgb rgbs1[3] lw 3 # goldenrod with opacity
set style line 24 lc rgb rgbs1[4] lw 3 # purple with opacity
set style line 25 lc rgb rgbs1[5] lw 3 # green with opacity
set style line 26 lc rgb rgbs1[6] lw 3 # red with opacity

set style line 31 lc rgb rgbs2[6] dashtype 30 lw 3 # blue
set style line 32 lc rgb rgbs2[5] dashtype 30 lw 3 # burnt orange
set style line 33 lc rgb rgbs2[4] dashtype 30 lw 3 # goldenrod
set style line 34 lc rgb rgbs2[3] dashtype 30 lw 3 # purple
set style line 35 lc rgb rgbs2[2] dashtype 30 lw 3 # green
set style line 36 lc rgb rgbs2[1] dashtype 30 lw 3 # red

set style line 41 lc rgb rgbs1[1] dashtype 30 lw 3 # blue
set style line 42 lc rgb rgbs1[2] dashtype 30 lw 3 # burnt orange
set style line 43 lc rgb rgbs1[3] dashtype 30 lw 3 # goldenrod
set style line 44 lc rgb rgbs1[4] dashtype 30 lw 3 # purple
set style line 45 lc rgb rgbs1[5] dashtype 30 lw 3 # green
set style line 46 lc rgb rgbs1[6] dashtype 30 lw 3 # red
