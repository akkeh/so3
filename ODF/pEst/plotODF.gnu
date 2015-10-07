set terminal postscript
set output '| ps2pdf - ODFplot.pdf'

set multiplot layout 2, 1

set title "Onsets:"
plot 'onsets.txt' with lines

set title "Waveform:"
plot 'waveform.txt'
unset multiplot
