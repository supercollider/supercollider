// Example for C++ Interface to Gnuplot

// requirements:
// * gnuplot has to be installed (http://www.gnuplot.info/download.html)
// * for Windows: set Path-Variable for Gnuplot path (e.g. C:/program files/gnuplot/bin)
//             or set Gnuplot path with: Gnuplot::set_GNUPlotPath(const std::string &path);


#include <iostream>
#include "gnuplot_i.hpp" //Gnuplot class handles POSIX-Pipe-communikation with Gnuplot

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
 #include <conio.h>   //for getch(), needed in wait_for_key()
 #include <windows.h> //for Sleep()
 void sleep(int i) { Sleep(i*1000); }
#endif


#define SLEEP_LGTH 2  // sleep time in seconds
#define NPOINTS    50 // length of array

using namespace std;

void wait_for_key(); // Programm halts until keypress



int main(int argc, char* argv[])
{
    // if path-variable for gnuplot is not set, do it with:
    // Gnuplot::set_GNUPlotPath("C:/program files/gnuplot/bin/");

    // set a special standart terminal for showonscreen (normally not needed),
    //   e.g. Mac users want to use x11 instead of aqua terminal:
    // Gnuplot::set_terminal_std("x11");

    cout << "*** example of gnuplot control through C++ ***" << endl << endl;

    //
    // Using the GnuplotException class
    //
    try
    {
        Gnuplot g1 = Gnuplot("lines");

        //
        // Slopes
        //
        cout << "*** plotting slopes" << endl;
        g1.set_title("Slopes");

        cout << "y = x" << endl;
        g1.plot_slope(1.0,0.0,"y=x");
        sleep(SLEEP_LGTH);

        cout << "y = 2*x" << endl;
        g1.plot_slope(2.0,0.0,"y=2x");
        sleep(SLEEP_LGTH);

        cout << "y = -x" << endl;
        g1.plot_slope(-1.0,0.0,"y=-x");
        sleep(SLEEP_LGTH);
        g1.unset_title();

        //
        // Equations
        //
        g1.reset_plot();
        cout << endl << endl << "*** various equations" << endl;

        cout << "y = sin(x)" << endl;
        g1.plot_equation("sin(x)","sine");
        sleep(SLEEP_LGTH);

        cout << "y = log(x)" << endl;
        g1.plot_equation("log(x)","logarithm");
        sleep(SLEEP_LGTH);

        cout << "y = sin(x) * cos(2*x)" << endl;
        g1.plot_equation("sin(x)*cos(2*x)","sine product");
        sleep(SLEEP_LGTH);

        //
        // Styles
        //
        g1.reset_plot();
        cout << endl << endl << "*** showing styles" << endl;

        cout << "sine in points" << endl;
        g1.set_pointsize(0.8).set_style("points");
        g1.plot_equation("sin(x)","points");
        sleep(SLEEP_LGTH);

        cout << "sine in impulses" << endl;
        g1.set_style("impulses");
        g1.plot_equation("sin(x)","impulses");
        sleep(SLEEP_LGTH);

        cout << "sine in steps" << endl;
        g1.set_style("steps");
        g1.plot_equation("sin(x)","steps");
        sleep(SLEEP_LGTH);

        //
        // Save to ps
        //
        g1.reset_all();
        cout << endl << endl << "*** save to ps " << endl;

        cout << "y = sin(x) saved to test_output.ps in working directory" << endl;
        g1.savetops("test_output");
        g1.set_style("lines").set_samples(300).set_xrange(0,5);
        g1.plot_equation("sin(12*x)*exp(-x)").plot_equation("exp(-x)");

        g1.showonscreen(); // window output


        //
        // User defined 1d, 2d and 3d point sets
        //
        vector<double> x, y, y2, dy, z;

        for (int i = 0; i < NPOINTS; i++)  // fill double arrays x, y, z
        {
            x.push_back((double)i);             // x[i] = i
            y.push_back((double)i * (double)i); // y[i] = i^2
            z.push_back( x[i]*y[i] );           // z[i] = x[i]*y[i] = i^3
            dy.push_back((double)i * (double)i / (double) 10); // dy[i] = i^2 / 10
        }
        y2.push_back(0.00); y2.push_back(0.78); y2.push_back(0.97); y2.push_back(0.43);
        y2.push_back(-0.44); y2.push_back(-0.98); y2.push_back(-0.77); y2.push_back(0.02);


        g1.reset_all();
        cout << endl << endl << "*** user-defined lists of doubles" << endl;
        g1.set_style("impulses").plot_x(y,"user-defined doubles");
        sleep(SLEEP_LGTH);

        g1.reset_plot();
        cout << endl << endl << "*** user-defined lists of points (x,y)" << endl;
        g1.set_grid();
        g1.set_style("points").plot_xy(x,y,"user-defined points 2d");
        sleep(SLEEP_LGTH);

        g1.reset_plot();
        cout << endl << endl << "*** user-defined lists of points (x,y,z)" << endl;
        g1.unset_grid();
        g1.plot_xyz(x,y,z,"user-defined points 3d");
        sleep(SLEEP_LGTH);

        g1.reset_plot();
        cout << endl << endl << "*** user-defined lists of points (x,y,dy)" << endl;
        g1.plot_xy_err(x,y,dy,"user-defined points 2d with errorbars");
        sleep(SLEEP_LGTH);


        //
        // Multiple output screens
        //
        cout << endl << endl;
        cout << "*** multiple output windows" << endl;

        g1.reset_plot();
        g1.set_style("lines");
        cout << "window 1: sin(x)" << endl;
        g1.set_grid().set_samples(600).set_xrange(0,300);
        g1.plot_equation("sin(x)+sin(x*1.1)");
        sleep(SLEEP_LGTH);

        g1.set_xautoscale().replot();
        sleep(SLEEP_LGTH);

        Gnuplot g2;
        cout << "window 2: user defined points" << endl;
        g2.plot_x(y2,"points");
        g2.set_smooth().plot_x(y2,"cspline");
        g2.set_smooth("bezier").plot_x(y2,"bezier");
        g2.unset_smooth();
        sleep(SLEEP_LGTH);

        Gnuplot g3("lines");
        cout << "window 3: log(x)/x" << endl;
        g3.set_grid();
        g3.plot_equation("log(x)/x","log(x)/x");
        sleep(SLEEP_LGTH);

        Gnuplot g4("lines");
        cout << "window 4: splot x*x+y*y" << endl;
        g4.set_zrange(0,100);
        g4.set_xlabel("x-axis").set_ylabel("y-axis").set_zlabel("z-axis");
        g4.plot_equation3d("x*x+y*y");
        sleep(SLEEP_LGTH);

        Gnuplot g5("lines");
        cout << "window 5: splot with hidden3d" << endl;
        g5.set_isosamples(25).set_hidden3d();
        g5.plot_equation3d("x*y*y");
        sleep(SLEEP_LGTH);

        Gnuplot g6("lines");
        cout << "window 6: splot with contour" << endl;
        g6.set_isosamples(60).set_contour();
        g6.unset_surface().plot_equation3d("sin(x)*sin(y)+4");
        sleep(SLEEP_LGTH);

        g6.set_surface().replot();
        sleep(SLEEP_LGTH);

        Gnuplot g7("lines");
        cout << "window 7: set_samples" << endl;
        g7.set_xrange(-30,20).set_samples(40);
        g7.plot_equation("besj0(x)*0.12e1").plot_equation("(x**besj0(x))-2.5");
        sleep(SLEEP_LGTH);

        g7.set_samples(400).replot();
        sleep(SLEEP_LGTH);

        Gnuplot g8("filledcurves");
        cout << "window 8: filledcurves" << endl;
        g8.set_legend("outside right top").set_xrange(-5,5);
        g8.plot_equation("x*x").plot_equation("-x*x+4");

        //
        // Plot an image
        //
        Gnuplot g9;
        cout << "window 9: plot_image" << endl;
        const int iWidth  = 255;
        const int iHeight = 255;
        g9.set_xrange(0,iWidth).set_yrange(0,iHeight).set_cbrange(0,255);
        g9.cmd("set palette gray");
        unsigned char ucPicBuf[iWidth*iHeight];
        // generate a greyscale image
        for(int iIndex = 0; iIndex < iHeight*iWidth; iIndex++)
        {
            ucPicBuf[iIndex] = iIndex%255;
        }
        g9.plot_image(ucPicBuf,iWidth,iHeight,"greyscale");
        sleep(SLEEP_LGTH);

        g9.set_pointsize(0.6).unset_legend().plot_slope(0.8,20);
        sleep(SLEEP_LGTH);

        //
        // manual control
        //
        Gnuplot g10;
        cout << "window 10: manual control" << endl;
        g10.cmd("set samples 400").cmd("plot abs(x)/2"); // either with cmd()
        g10 << "replot sqrt(x)" << "replot sqrt(-x)";    // or with <<

        wait_for_key();

    }
    catch (GnuplotException ge)
    {
        cout << ge.what() << endl;
    }


    cout << endl << "*** end of gnuplot example" << endl;

    return 0;
}



void wait_for_key ()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)  // every keypress registered, also arrow keys
    cout << endl << "Press any key to continue..." << endl;

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    _getch();
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    cout << endl << "Press ENTER to continue..." << endl;

    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
    std::cin.get();
#endif
    return;
}
