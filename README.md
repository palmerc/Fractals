# Mandelbrot C++

I wanted some quick examples of a couple of fractals and found an [Fractal generator](http://yozh.org/mset_index/) on the web. Worked, but since I had trouble getting a compatible BMP file as input, I replaced his code with [single header BMP library](http://www.partow.net/programming/bitmap/). I'm also providing a basic set of configuration files to get started.

## Compile

    clang++ -O3 -o mandelbrot mandelbrot-source.cpp 
  
## Run

    ./mandelbrot parameters-default
