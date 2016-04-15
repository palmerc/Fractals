/*
 XANDER'S MANDELBROT PROGRAM OF DOOM
 REVISION DATE:  10 January 2012

 Expected input file format:
	line 1:		exponent (degree of the Julia or Mandelbrot set to generate)
	line 2:		x center
	line 3:		y center
	line 4:		zoom level
	line 5:		aspect ratio (width:height)
	line 6:		horizontal size of the output file
	line 7:		maximum number of iterations
	line 8:		number of colors
	line 9:		name of the output file (include file extension)
	line 10:	name of the color file (include file extension)
	line 11:	fractal to generate
 m - Mandelbrot / multibrot set (z^d+c)
 j - Julia set (z^d + K)
 s - burning ship fractal
 u - Mandelbrot / multibrot inversion ((1/z)^d+1/c)
 c - Collatz fractal
 use capital letters for NIC coloring
	line 12+:	additional parameters
 Re(K) and Im(K) for j or J
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "bitmap_image.hpp" // BMP v3 Support



const int MAXCOLORS = 12800;
const double PI = 3.14159265;

const unsigned char BMP_DATA_START_OFFSET = 0x0a;

int fact(int);
int choose(int, int);
void z_pow(double [], int);
int mandelbrot(double [], int, int);
int julia(double [], double [], int, int);
int ship(double [], int, int);
int collatz(double [], int);
unsigned int initColors(unsigned char [][3], bitmap_image &);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    std::ofstream fout;
    std::ifstream fin;
    int escapeTime, d, xsize, ysize, maximumIterations;
    std::string input, parFile, imgName, colorFile;
    double xcen, ycen, aspect, zoom, xmin, xmax, ymin, ymax, step, z[2], K[2], ff=0, tmp, D;
    unsigned char colors[MAXCOLORS+1][3], fractal;
    bool invalid=true;
    time_t start, end;

    if (argc > 1) {
        parFile = argv[1];

        //	READ IN THE PARAMETER FILE
        std::cout << "Parameter file is: " << parFile  << std::endl;

        fin.open(("par_files/" + parFile + ".txt").c_str());
        if (fin.fail()) {
            std::cerr << "ERROR: The parameter file  'par_files/" << parFile << ".txt' could not be opened.\nAttempting to use 'parameters-default.txt'.\n";

            fin.open("par_files/parameters-default.txt");
            if(fin.fail()) {
                std::cerr << "ERROR: 'parameters-default.txt' could not be opened.\nAborting.\n";
                exit(1);
            }
        }

        fin >> d >> xcen >> ycen >> zoom >> aspect >> xsize >> maximumIterations >> imgName >> colorFile >> fractal;

        switch(fractal) {
            case 'j':
                fin >> K[0] >> K[1];
                break;
            case 'J':
                fin >> K[0] >> K[1];
                break;
        }
        fin.close();

        ycen = -ycen;
        xmin = xcen - zoom;
        xmax = xcen + zoom;
        ymin = ycen - (zoom * 1.0/aspect);
        ymax = ycen + (zoom * 1.0/aspect);

        step = (xmax-xmin) / xsize;
        ysize = (ymax - ymin) / step;
    } else {
        std::cerr << "ERROR: No parameter file was specified.\nAborting.\n";
        exit(0);
    }

    std::string outputBMPFilename = "output/" + imgName;
    bitmap_image outputImage(xsize, ysize);

    std::string colorBMPFilename = "color_files/" + colorFile;
    bitmap_image colorBitmap(colorBMPFilename);

    start = time(NULL);

    unsigned int numberOfColors = initColors(colors, colorBitmap);
    if(numberOfColors > MAXCOLORS) {
        numberOfColors = MAXCOLORS;
    }

    xmin += (step/2);
    ymin += (step/2);

    //	THE HEART OF THE PROGRAM
    //		for each pixel, we determine whether it is in the set or not
    //		then color it appropriately
    for(int x = 0; x < xsize; x++) {
        for(int y = 0; y < ysize; y++) {
            z[0] = xmin + (x * step);
            z[1] = ymin + (y * step);

            switch(fractal) {
                case 'j':
                case 'J':
                    escapeTime = julia(z, K, d, maximumIterations);
                    break;
                case 's':
                    escapeTime = ship(z, d, maximumIterations);
                    break;
                case 'u':
                case 'U':
                    D=(z[0]*z[0])+(z[1]*z[1]);
                    z[0] = z[0]/D;
                    z[1] = -z[1]/D;
                    escapeTime = mandelbrot(z, d, maximumIterations);
                    break;
                case 'c':
                case 'C':
                    escapeTime = collatz(z, maximumIterations);
                    break;
                case 'm':
                case 'M':
                default:
                    escapeTime = mandelbrot(z, d, maximumIterations);
                    break;
            }

            // NIC coloring, if specified by a capital letter
            if(fractal >= 'A' && fractal <= 'Z' && escapeTime != 0) {
                tmp = escapeTime + (log(log(81)) - (log(log(sqrt(z[0]*z[0]+z[1]*z[1]))))/log(d));
                tmp = tmp * ((double) numberOfColors / (double) maximumIterations);
                escapeTime = (int) tmp;
            }

            int color_index = 0;
            if (escapeTime > 0) {
                color_index = (escapeTime % (numberOfColors - 1)) + 1;
            }

            unsigned char red = colors[color_index][0];
            unsigned char green = colors[color_index][1];
            unsigned char blue = colors[color_index][2];
            outputImage.set_pixel(x, y, red, green, blue);
        }
    }

    outputImage.save_image(outputBMPFilename);

    end = time(NULL);

    printf("Render Time: %5d seconds\n", (int) (end-start));

    return 0;
}

/*	COMPUTES n! */
int fact(int n) {
    int res=1;

    for(int i=2; i<=n; ++i) {
        res *= i;
    }

    return res;
}

/*	COMPUTES C(n,k) */
int choose(int n, int k) {
    int res = 1;

    if(k>n/2) {		//  C(n,k) = C(n,n-k), so work with the smaller of the two
        k = n-k;
    }

    for(int i=n; i>=n-k+1; --i) {
        res *= i;
    }

    return res/fact(k);
}

/*	COMPUTES z^d WHERE z IS COMPLEX AND d IS AN INTEGER */
void z_pow(double z[2], int d) {
    double res[2], delta;

    res[0]=0; res[1]=0;		// the real and imaginary parts of the result

    for(int i=0; i<=d; ++i) {
        delta = choose(d,i)*pow(z[0],d-i)*pow(z[1],i);
        switch(i%4) {
            case 0:
                res[0] += delta;
                break;
            case 1:
                res[1] += delta;
                break;
            case 2:
                res[0] -= delta;
                break;
            case 3:
                res[1] -= delta;
                break;
        }
    }

    z[0] = res[0];
    z[1] = res[1];

    return;
}

/*	DETERMINES WHETHER A GIVEN POINT IN THE COMPLEX PLANE IS IN THE dTH DEGREE
	MULTIBROT SET, AND EITHER RETURNS THE NUMBER OF ITERATIONS UNTIL THE ESCAPE
	CRITERION IS MET OR numIterations IF THE ESCAPE CRITERION IS NOT MET IN
	FEWER THAN numIterations ITERATIONS. */
int mandelbrot(double z[2], int d, int numIterations) {
    double c[2];
    c[0] = z[0];
    c[1] = z[1];

    int iteration = 0;
    double result = 0;
    while (result <= 4 && iteration < numIterations) {
        result = (z[0] * z[0]) + (z[1] * z[1]);

        z_pow(z, d);
        z[0] += c[0];
        z[1] += c[1];
        iteration++;
    }

    if (result <= 4) {
        iteration = 0;
    }

    return iteration;
}

/*	DETERMINES WHETHER A GIVEN POINT IN THE COMPLEX PLANE IS IN THE dTH DEGREE
	JULIA SET, AND RETURNS THE NUMBER OF ITERATIONS UNTIL THE ESCAPE
	CRITERION IS MET OR numIterations IF THE ESCAPE CRITERION IS NOT MET IN
	FEWER THAN numIterations ITERATIONS. */
int julia(double z[2], double K[2], int d, int numIterations) {
    int iteration = 1;
    bool inSet = true;
    double c[2];

    c[0]=z[0];
    c[1]=z[1];

    if((z[0]*z[0])+(z[1]*z[1]) > 4) {
        inSet = false;
    }

    while(inSet && iteration < numIterations) {
        z_pow(z,d);
        z[0] += K[0];
        z[1] += K[1];
        ++iteration;

        if((z[0]*z[0])+(z[1]*z[1]) > 4) {
            inSet = false;
        }
    }

    if(inSet) {
        iteration = 0;
    }

    return iteration;
}

/*	DETERMINES WHETHER A GIVEN POINT IN THE COMPLEX PLANE IS IN THE dTH DEGREE
	BURNING SHIP FRACTAL, AND RETURNS THE NUMBER OF ITERATIONS UNTIL THE ESCAPE
	CRITERION IS MET OR numIterations IF THE ESCAPE CRITERION IS NOT MET IN
	FEWER THAN numIterations ITERATIONS. */
int ship(double z[2], int d, int numIterations) {
    int iteration = 1;
    bool inSet = true;
    double c[2];

    c[0]=z[0];
    c[1]=z[1];

    if((z[0]*z[0])+(z[1]*z[1]) > 4) {
        inSet = false;
    }

    while(inSet && iteration < numIterations) {
        z[0] = std::abs(z[0]);
        z[1] = std::abs(z[1]);
        z_pow(z,d);
        z[0] += c[0];
        z[1] += c[1];
        ++iteration;

        if((z[0]*z[0])+(z[1]*z[1]) > 4) {
            inSet = false;
        }
    }
    
    if(inSet) {
        iteration = 0;
    }
    
    return iteration;
}

/*	DETERMINES WHETHER A GIVEN POINT IN THE COMPLEX PLANE IS IN THE COLLATZ
	FRACTAL SET, AND RETURNS THE NUMBER OF ITERATIONS UNTIL THE ESCAPE
	CRITERION IS MET OR numIterations IF THE ESCAPE CRITERION IS NOT MET IN
	FEWER THAN numIterations ITERATIONS. */
int collatz(double z[2], int numIterations) {
    int iteration = 1;
    bool inSet = true;
    double cosz[2],tmp[2];
    
    if((z[0]*z[0])+(z[1]*z[1]) > 16384) {
        inSet = false;
    }
    
    while(inSet && iteration < numIterations) {
        cosz[0] = cos(PI*z[0])*cosh(PI*z[1]);
        cosz[1] = -sin(PI*z[0])*sinh(PI*z[1]);
        
        tmp[0] = (z[0]*cosz[0])-(z[1]*cosz[1]);
        tmp[1] = (z[0]*cosz[1])+(z[1]*cosz[0]);
        
        z[0] = (2+7*z[0]-2*cosz[0]-5*tmp[0])/4;
        z[1] = (7*z[1]-2*cosz[1]-5*tmp[1])/4;
        
        ++iteration;
        
        if((z[0]*z[0])+(z[1]*z[1]) > 16384) {
            inSet = false;
        }
    }
    
    if(inSet) {
        iteration = 0;
    }
    
    return iteration;
}

unsigned int initColors(unsigned char colors[][3], bitmap_image &colorBitmap) {
    const unsigned int numberOfColors = colorBitmap.width();
    for (std::size_t i = 0; i < numberOfColors; i++) {
        unsigned char red = 0;
        unsigned char green = 0;
        unsigned char blue = 0;
        colorBitmap.get_pixel(i, 0, red, green, blue);
        
        colors[i][0] = red;
        colors[i][1] = green;
        colors[i][2] = blue;
    }
    
    return numberOfColors;
}