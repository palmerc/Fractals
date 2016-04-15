# Color Files

The BMP color files act as a color lookup table. We only look at the top row of the file and really it could be 1px high. Also keep in mind the example gradient is terrible, because the iteration count of the Mandelbrot set is heavily clustered around 0 (in the set), small values that are quickly determined to be not in the set, and high iteration counts around the fringe (where the action lies). So that nice gradient in the middle is basically useless.

## Convert PNG to correct BMP format using ImageMagick's mogrify

    mogrify -format bmp -define bmp:format=BMP3 input.png

