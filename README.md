# Mandelbrot C++

I wanted just wanted some quick examples of a couple of fractals and found an [Fractal generator](http://yozh.org/mset_index/) on the web. Worked, but since I had trouble getting a compatible BMP file as input, I replaced his code with [single header BMP library](http://www.partow.net/programming/bitmap/). I'm also providing a basic set of configuration files to get started.

FYI - the [BMP file format](https://en.wikipedia.org/wiki/BMP_file_format) is simple, but also a bit crazy. Nothing I have on my Mac supports BMP file creation, but if you output a PNG you can convert it with [ImageMagick's mogrify](http://www.imagemagick.org/script/mogrify.php).

## For the sake of nostalgia

This whole foray into an (overly complicated) example app using Fractals makes me wish I could find the version I wrote in Turbo Pascal when I was a teenager. Unfortunately, even if I found it, I know it was on a 5 1/4" floppy so it might as well not exist. Incidentally, my first version output to the dot matrix printer and must have taken at least an hour to print on the IBM PC AT.

## Compile

    clang++ -O3 -o mandelbrot mandelbrot-source.cpp 
  
## Run

    ./mandelbrot parameters-default
