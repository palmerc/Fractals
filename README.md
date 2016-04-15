# Mandelbrot C++

I wanted some quick examples of a couple of fractals and found an [implementation on the web](http://yozh.org/mset_index/). Worked, but since I had trouble getting a compatible BMP file as input, I replaced his code with [a more complete implementation](http://www.partow.net/programming/bitmap/). I'm also providing a basic set of configuration files to get started.

## Compile

    clang++ -O3 -o mandelbrot mandelbrot-source.cpp 
  
## Run

    ./mandelbrot parameters-default
