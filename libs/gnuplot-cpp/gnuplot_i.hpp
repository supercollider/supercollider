////////////////////////////////////////////////////////////////////////////////////////
//
// A C++ interface to gnuplot.
//
//
// The interface uses pipes and so won't run on a system that doesn't have POSIX pipe support
// Tested on Windows (MinGW and Visual C++) and Linux (GCC)
//
// Version history:
// 0. C interface
//    by N. Devillard (27/01/03)
// 1. C++ interface: direct translation from the C interface
//    by Rajarshi Guha (07/03/03)
// 2. corrections for Win32 compatibility
//    by V. Chyzhdzenka (20/05/03)
// 3. some member functions added, corrections for Win32 and Linux compatibility
//    by M. Burgis (10/03/08)
//
// Requirements:
// * gnuplot has to be installed (http://www.gnuplot.info/download.html)
// * for Windows: set Path-Variable for Gnuplot path (e.g. C:/program files/gnuplot/bin)
//             or set Gnuplot path with: Gnuplot::set_GNUPlotPath(const std::string &path);
//
////////////////////////////////////////////////////////////////////////////////////////


#ifndef _GNUPLOT_PIPES_H_
#define _GNUPLOT_PIPES_H_


#include <string>
#include <vector>
#include <fstream>    // for std::ifstream
#include <stdexcept>  // for std::runtime_error class in GnuplotException
#include <cstdio>     // for FILE (identifies a stream and contains a pointer to its buffer, its position indicator and all its state indicators)


//declare classes in global namespace


class GnuplotException : public std::runtime_error
{
    public:
        GnuplotException(const std::string &msg) : std::runtime_error(msg){}
};



class Gnuplot
{
    private:

    //----------------------------------------------------------------------------------
    // member data
        FILE                    *gnucmd;       // pointer to the stream that can be used to write to the pipe
        bool                     valid;        // validation of gnuplot session
        bool                     two_dim;      // true = 2d, false = 3d
        int                      nplots;       // number of plots in session
        std::string              pstyle;       // functions and data are displayed in a defined styles
        std::string              smooth;       // interpolate and approximate data in defined styles (e.g. spline)
        std::vector<std::string> tmpfile_list; // list of created tmpfiles

    //----------------------------------------------------------------------------------
    // static data
        static int               tmpfile_num;        // number of all tmpfiles (number of tmpfiles restricted)
        static std::string       m_sGNUPlotFileName; // name of executed GNUPlot file
        static std::string       m_sGNUPlotPath;     // gnuplot path
        static std::string       terminal_std;       // standart terminal, used by showonscreen

    //----------------------------------------------------------------------------------
    // member functions (auxiliary functions)
        void           init();                              // get_program_path(); and popen();
        std::string    create_tmpfile(std::ofstream &tmp);  // creates tmpfile and returns its name

    //----------------------------------------------------------------------------------
    //static functions
        static bool    get_program_path();                                   // gnuplot path found?
        static bool    file_exists(const std::string &filename, int mode=0); // checks if file exists


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
    public:

        // optional: set Gnuplot path manual
        //   for windows: path with slash '/' not backslash '\'
        static bool set_GNUPlotPath(const std::string &path);

        // optional: set standart terminal, used by showonscreen
        //   defaults: Windows - win, Linux - x11, Mac - aqua
        static void set_terminal_std(const std::string &type);

    //----------------------------------------------------------------------------------
    // constructors

        // set a style during construction
        Gnuplot(const std::string &style = "points");

        // plot a single std::vector at one go
        Gnuplot(const std::vector<double> &x,
                const std::string &title = "",
                const std::string &style = "points",
                const std::string &labelx = "x",
                const std::string &labely = "y");

        // plot pairs std::vector at one go
        Gnuplot(const std::vector<double> &x,
                const std::vector<double> &y,
                const std::string &title = "",
                const std::string &style = "points",
                const std::string &labelx = "x",
                const std::string &labely = "y");

        // plot triples std::vector at one go
        Gnuplot(const std::vector<double> &x,
                const std::vector<double> &y,
                const std::vector<double> &z,
                const std::string &title = "",
                const std::string &style = "points",
                const std::string &labelx = "x",
                const std::string &labely = "y",
                const std::string &labelz = "z");

        // destructor: needed to delete temporary files
        ~Gnuplot();


    //----------------------------------------------------------------------------------

        // send a command to gnuplot
        Gnuplot& cmd(const std::string &cmdstr);
        Gnuplot& operator<<(const std::string &cmdstr);


    //----------------------------------------------------------------------------------
    // show on screen or write to file

        // sets terminal type to terminal_std
        Gnuplot& showonscreen(); // window output is set by default (win/x11/aqua)

        // saves a gnuplot session to a postscript file, filename without extension
        Gnuplot& savetops(const std::string &filename = "gnuplot_output");


    //----------------------------------------------------------------------------------
    // set and unset

        // set line style (some of these styles require additional information):
        //  lines, points, linespoints, impulses, dots, steps, fsteps, histeps,
        //  boxes, histograms, filledcurves
        Gnuplot& set_style(const std::string &stylestr = "points");

        // interpolation and approximation of data, arguments:
        //  csplines, bezier, acsplines (for data values > 0), sbezier, unique, frequency
        // (works only with plot_x, plot_xy, plotfile_x, plotfile_xy
        // (if smooth is set, set_style has no effekt on data plotting)
        Gnuplot& set_smooth(const std::string &stylestr = "csplines");
        Gnuplot& unset_smooth(); // smooth is not set by default

        // scales the size of the points used in plots
        Gnuplot& set_pointsize(const double pointsize = 1.0);

        // turns grid on/off
        Gnuplot& set_grid();
        Gnuplot& unset_grid(); // grid is not set by default

        // set sampling rate of functions, or for interpolating data
        Gnuplot& set_samples(const int samples = 100);
        // set isoline density (grid) for plotting functions as surfaces (for 3d plots)
        Gnuplot& set_isosamples(const int isolines = 10);

        // enables/disables hidden line removal for surface plotting (for 3d plot)
        Gnuplot& set_hidden3d();
        Gnuplot& unset_hidden3d(); // hidden3d is not set by default

        // enables/disables contour drawing for surfaces (for 3d plot)
        //  base, surface, both
        Gnuplot& set_contour(const std::string &position = "base");
        Gnuplot& unset_contour(); // contour is not set by default

        // enables/disables the display of surfaces (for 3d plot)
        Gnuplot& set_surface(); // surface is set by default
        Gnuplot& unset_surface();

        // switches legend on/off
        // position: inside/outside, left/center/right, top/center/bottom, nobox/box
        Gnuplot& set_legend(const std::string &position = "default"); // legend is set by default
        Gnuplot& unset_legend();

        // sets and clears the title of a gnuplot session
        Gnuplot& set_title(const std::string &title = "");
        Gnuplot& unset_title(); // title is not set by default

        // set y and x axis labels
        Gnuplot& set_ylabel(const std::string &label = "x");
        Gnuplot& set_xlabel(const std::string &label = "y");
        Gnuplot& set_zlabel(const std::string &label = "z");

        // set axis - ranges
        Gnuplot& set_xrange(const double iFrom,
                            const double iTo);
        Gnuplot& set_yrange(const double iFrom,
                            const double iTo);
        Gnuplot& set_zrange(const double iFrom,
                            const double iTo);
        // autoscale axis (set by default)
        Gnuplot& set_xautoscale();
        Gnuplot& set_yautoscale();
        Gnuplot& set_zautoscale();

        // turns on/off log scaling for the specified axes (logscale is not set by default)
        Gnuplot& set_xlogscale(const double base = 10);
        Gnuplot& set_ylogscale(const double base = 10);
        Gnuplot& set_zlogscale(const double base = 10);
        Gnuplot& unset_xlogscale();
        Gnuplot& unset_ylogscale();
        Gnuplot& unset_zlogscale();

        // set palette range (autoscale by default)
        Gnuplot& set_cbrange(const double iFrom, const double iTo);


    //----------------------------------------------------------------------------------
    // plot

        // plot a single std::vector: x
        //   from file
        Gnuplot& plotfile_x(const std::string &filename,
                            const unsigned int column = 1,
                            const std::string &title = "");
        //   from std::vector
        template<typename X>
        Gnuplot& plot_x(const X& x, const std::string &title = "");


        // plot x,y pairs: x y
        //   from file
        Gnuplot& plotfile_xy(const std::string &filename,
                             const unsigned int column_x = 1,
                             const unsigned int column_y = 2,
                             const std::string &title = "");
        //   from std::vector
        template<typename X, typename Y>
        Gnuplot& plot_xy(const X& x, const Y& y, const std::string &title = "");


        // plot x,y pairs with dy errorbars: x y dy
        //   from file
        Gnuplot& plotfile_xy_err(const std::string &filename,
                                 const unsigned int column_x  = 1,
                                 const unsigned int column_y  = 2,
                                 const unsigned int column_dy = 3,
                                 const std::string &title = "");
        //   from std::vector
        Gnuplot& plot_xy_err(const std::vector<double> &x,
                             const std::vector<double> &y,
                             const std::vector<double> &dy,
                             const std::string &title = "");


        // plot x,y,z triples: x y z
        //   from file
        Gnuplot& plotfile_xyz(const std::string &filename,
                              const unsigned int column_x = 1,
                              const unsigned int column_y = 2,
                              const unsigned int column_z = 3,
                              const std::string &title = "");
        //   from std::vector
        Gnuplot& plot_xyz(const std::vector<double> &x,
                          const std::vector<double> &y,
                          const std::vector<double> &z,
                          const std::string &title = "");



        // plot an equation of the form: y = ax + b, you supply a and b
        Gnuplot& plot_slope(const double a,
                            const double b,
                            const std::string &title = "");


        // plot an equation supplied as a std::string y=f(x), write only the function f(x) not y=
        // the independent variable has to be x
        // binary operators: ** exponentiation, * multiply, / divide, + add, - substract, % modulo
        // unary operators: - minus, ! factorial
        // elementary functions: rand(x), abs(x), sgn(x), ceil(x), floor(x), int(x), imag(x), real(x), arg(x),
        //   sqrt(x), exp(x), log(x), log10(x), sin(x), cos(x), tan(x), asin(x), acos(x), atan(x), atan2(y,x),
        //   sinh(x), cosh(x), tanh(x), asinh(x), acosh(x), atanh(x)
        // special functions: erf(x), erfc(x), inverf(x), gamma(x), igamma(a,x), lgamma(x), ibeta(p,q,x),
        //   besj0(x), besj1(x), besy0(x), besy1(x), lambertw(x)
        // statistical fuctions: norm(x), invnorm(x)
        Gnuplot& plot_equation(const std::string &equation,
                               const std::string &title = "");

        // plot an equation supplied as a std::string z=f(x,y), write only the function f(x,y) not z=
        // the independent variables have to be x and y
        Gnuplot& plot_equation3d(const std::string &equation,
                                 const std::string &title = "");


        // plot image
        Gnuplot& plot_image(const unsigned char *ucPicBuf,
                            const unsigned int iWidth,
                            const unsigned int iHeight,
                            const std::string &title = "");


    //----------------------------------------------------------------------------------

        // replot repeats the last plot or splot command.
        //  this can be useful for viewing a plot with different set options,
        //  or when generating the same plot for several devices (showonscreen, savetops)
        Gnuplot& replot();


        // resets a gnuplot session (next plot will erase previous ones)
        Gnuplot& reset_plot();

        // resets a gnuplot session and sets all variables to default
        Gnuplot& reset_all();


        // validation of gnuplot session
        bool is_valid();

};


//----------------------------------------------------------------------------------
//
// Plots a 2d graph from a list of doubles: x
//
template<typename X>
Gnuplot& Gnuplot::plot_x(const X& x, const std::string &title)
{
    if (x.size() == 0)
    {
        throw GnuplotException("std::vector too small");
        return *this;
    }

    std::ofstream tmp;
    std::string name = create_tmpfile(tmp);
    if (name == "")
        return *this;

    //
    // write the data to file
    //
    for (unsigned int i = 0; i < x.size(); i++)
        tmp << x[i] << std::endl;

    tmp.flush();
    tmp.close();


    plotfile_x(name, 1, title);

    return *this;
}

  
//----------------------------------------------------------------------------------
//
// Plots a 2d graph from a list of doubles: x y
//
template<typename X, typename Y>
Gnuplot& Gnuplot::plot_xy(const X& x, const Y& y, const std::string &title)
{
    if (x.size() == 0 || y.size() == 0)
    {
        throw GnuplotException("std::vectors too small");
        return *this;
    }

    if (x.size() != y.size())
    {
        throw GnuplotException("Length of the std::vectors differs");
        return *this;
    }


    std::ofstream tmp;
    std::string name = create_tmpfile(tmp);
    if (name == "")
        return *this;

    //
    // write the data to file
    //
    for (unsigned int i = 0; i < x.size(); i++)
        tmp << x[i] << " " << y[i] << std::endl;

    tmp.flush();
    tmp.close();


    plotfile_xy(name, 1, 2, title);

    return *this;
}

#endif
