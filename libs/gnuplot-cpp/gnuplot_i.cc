///////////////////////////////////////////////////////////
//
// A C++ interface to gnuplot.
//
// This is a direct translation from the C interface
// written by Nicolas Devillard (ndevilla@free.fr) which
// is available from http://ndevilla.free.fr/gnuplot/.
//
// As in the C interface this uses pipes and so wont
// run on a system that doesn't have POSIX pipe support
//
// Rajarshi Guha
// e-mail: rguha@indiana.edu, rajarshi@presidency.com
// http://cheminfo.informatics.indiana.edu/~rguha/code/cc++/
//
// 07/03/03
//
///////////////////////////////////////////////////////////
//
// A little correction for Win32 compatibility
// and MS VC 6.0 done by V.Chyzhdzenka
//
// Notes:
// 1. Added private method Gnuplot::init().
// 2. Temporary file is created in the current
//    folder but not in /tmp.
// 3. Added #ifdef WIN32 e.t.c. where is needed.
// 4. Added private member m_sGNUPlotFileName is
//    a name of executed GNUPlot file.
//
// Viktor Chyzhdzenka
// e-mail: chyzhdzenka@mail.ru
//
// 20/05/03
//
///////////////////////////////////////////////////////////
//
// corrections for Win32 and Linux compatibility
//
// some member functions added:
//  set_GNUPlotPath, set_terminal_std,
//  create_tmpfile, get_program_path, file_exists,
//  operator<<, replot, reset_all, savetops, showonscreen,
//  plotfile_*, plot_xy_err, plot_equation3d
// set, unset: pointsize, grid, *logscale, *autoscale,
//  smooth, title, legend, samples, isosamples,
//  hidden3d, cbrange, contour
//
// Markus Burgis
// e-mail: mail@burgis.info
//
// 10/03/08
//
///////////////////////////////////////////////////////////


#include <fstream>              // for std::ifstream
#include <sstream>              // for std::ostringstream
#include <list>                 // for std::list
#include <cstdio>               // for FILE, fputs(), fflush(), popen()
#include <cstdlib>              // for getenv()
#include "gnuplot_i.hpp"


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) //defined for 32 and 64-bit environments
 #include <io.h>                // for _access(), _mktemp()
 #define GP_MAX_TMP_FILES  27   // 27 temporary files it's Microsoft restriction
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__) //all UNIX-like OSs (Linux, *BSD, MacOSX, Solaris, ...)
 #include <unistd.h>            // for access(), mkstemp()
 #define GP_MAX_TMP_FILES  64
#else
 #error unsupported or unknown operating system
#endif




//----------------------------------------------------------------------------------
//
// initialize static data
//
int Gnuplot::tmpfile_num = 0;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
std::string Gnuplot::m_sGNUPlotFileName = "pgnuplot.exe";
std::string Gnuplot::m_sGNUPlotPath = "C:/program files/gnuplot/bin/";
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
std::string Gnuplot::m_sGNUPlotFileName = "gnuplot";
std::string Gnuplot::m_sGNUPlotPath = "/usr/bin/";
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
std::string Gnuplot::terminal_std = "windows";
#elif ( defined(unix) || defined(__unix) || defined(__unix__) ) && !defined(__APPLE__)
std::string Gnuplot::terminal_std = "x11";
#elif defined(__APPLE__)
std::string Gnuplot::terminal_std = "aqua";
#endif

//----------------------------------------------------------------------------------
//
// define static member function: set Gnuplot path manual
//   for windows: path with slash '/' not backslash '\'
//
bool Gnuplot::set_GNUPlotPath(const std::string &path)
{

    std::string tmp = path + "/" + Gnuplot::m_sGNUPlotFileName;


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    if ( Gnuplot::file_exists(tmp,0) ) // check existence
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if ( Gnuplot::file_exists(tmp,1) ) // check existence and execution permission
#endif
    {
        Gnuplot::m_sGNUPlotPath = path;
        return true;
    }
    else
    {
        Gnuplot::m_sGNUPlotPath.clear();
        return false;
    }
}


//----------------------------------------------------------------------------------
//
// define static member function: set standart terminal, used by showonscreen
//  defaults: Windows - win, Linux - x11, Mac - aqua
//
void Gnuplot::set_terminal_std(const std::string &type)
{
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if (type.find("x11") != std::string::npos && getenv("DISPLAY") == NULL)
    {
        throw GnuplotException("Can't find DISPLAY variable");
    }
#endif


    Gnuplot::terminal_std = type;
    return;
}


//----------------------------------------------------------------------------------
//
// A string tokenizer taken from http://www.sunsite.ualberta.ca/Documentation/
// /Gnu/libstdc++-2.90.8/html/21_strings/stringtok_std_h.txt
//
template <typename Container>
void stringtok (Container &container,
                std::string const &in,
                const char * const delimiters = " \t\n")
{
    const std::string::size_type len = in.length();
          std::string::size_type i = 0;

    while ( i < len )
    {
        // eat leading whitespace
        i = in.find_first_not_of (delimiters, i);

        if (i == std::string::npos)
            return;   // nothing left but white space

        // find the end of the token
        std::string::size_type j = in.find_first_of (delimiters, i);

        // push token
        if (j == std::string::npos)
        {
            container.push_back (in.substr(i));
            return;
        }
        else
            container.push_back (in.substr(i, j-i));

        // set up for next loop
        i = j + 1;
    }

    return;
}


//----------------------------------------------------------------------------------
//
// constructor: set a style during construction
//
Gnuplot::Gnuplot(const std::string &style)
{
    init();
    set_style(style);
}


//----------------------------------------------------------------------------------
//
// constructor: open a new session, plot a signal (x)
//
Gnuplot::Gnuplot(const std::vector<double> &x,
                 const std::string &title,
                 const std::string &style,
                 const std::string &labelx,
                 const std::string &labely)
{
    init();

    set_style(style);
    set_xlabel(labelx);
    set_ylabel(labely);

    plot_x(x,title);
}


//----------------------------------------------------------------------------------
//
// constructor: open a new session, plot a signal (x,y)
//
Gnuplot::Gnuplot(const std::vector<double> &x,
                 const std::vector<double> &y,
                 const std::string &title,
                 const std::string &style,
                 const std::string &labelx,
                 const std::string &labely)
{
    init();

    set_style(style);
    set_xlabel(labelx);
    set_ylabel(labely);

    plot_xy(x,y,title);
}


//----------------------------------------------------------------------------------
//
// constructor: open a new session, plot a signal (x,y,z)
//
Gnuplot::Gnuplot(const std::vector<double> &x,
                 const std::vector<double> &y,
                 const std::vector<double> &z,
                 const std::string &title,
                 const std::string &style,
                 const std::string &labelx,
                 const std::string &labely,
                 const std::string &labelz)
{
    init();

    set_style(style);
    set_xlabel(labelx);
    set_ylabel(labely);
    set_zlabel(labelz);

    plot_xyz(x,y,z,title);
}


//----------------------------------------------------------------------------------
//
// Destructor: needed to delete temporary files
//
Gnuplot::~Gnuplot()
{
    if ((tmpfile_list).size() > 0)
    {
        for (unsigned int i = 0; i < tmpfile_list.size(); i++)
            remove( tmpfile_list[i].c_str() );

        Gnuplot::tmpfile_num -= tmpfile_list.size();
    }

    // A stream opened by popen() should be closed by pclose()
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    if (_pclose(gnucmd) == -1)
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if (pclose(gnucmd) == -1)
#endif
        throw GnuplotException("Problem closing communication to gnuplot");
}


//----------------------------------------------------------------------------------
//
// Resets a gnuplot session (next plot will erase previous ones)
//
Gnuplot& Gnuplot::reset_plot()
{
    if (tmpfile_list.size() > 0)
    {
        for (unsigned int i = 0; i < tmpfile_list.size(); i++)
            remove(tmpfile_list[i].c_str());

        Gnuplot::tmpfile_num -= tmpfile_list.size();
        tmpfile_list.clear();
    }

    nplots = 0;

    return *this;
}


//----------------------------------------------------------------------------------
//
// resets a gnuplot session and sets all varibles to default
//
Gnuplot& Gnuplot::reset_all()
{
    if (tmpfile_list.size() > 0)
    {
        for (unsigned int i = 0; i < tmpfile_list.size(); i++)
            remove(tmpfile_list[i].c_str());

        Gnuplot::tmpfile_num -= tmpfile_list.size();
        tmpfile_list.clear();
    }

    nplots = 0;
    cmd("reset");
    cmd("clear");
    pstyle = "points";
    smooth = "";
    showonscreen();

    return *this;
}


//----------------------------------------------------------------------------------
//
// Find out if valid is true
//
bool Gnuplot::is_valid()
{
    return(valid);
}



//----------------------------------------------------------------------------------
//
// replot repeats the last plot or splot command
//
Gnuplot& Gnuplot::replot()
{
    if (nplots > 0)
    {
        cmd("replot");
    }

    return *this;
}


//----------------------------------------------------------------------------------
//
// Change the plotting style of a gnuplot session
//
Gnuplot& Gnuplot::set_style(const std::string &stylestr)
{
    if (stylestr.find("lines")          == std::string::npos  &&
        stylestr.find("points")         == std::string::npos  &&
        stylestr.find("linespoints")    == std::string::npos  &&
        stylestr.find("impulses")       == std::string::npos  &&
        stylestr.find("dots")           == std::string::npos  &&
        stylestr.find("steps")          == std::string::npos  &&
        stylestr.find("fsteps")         == std::string::npos  &&
        stylestr.find("histeps")        == std::string::npos  &&
        stylestr.find("boxes")          == std::string::npos  &&  // 1-4 columns of data are required
        stylestr.find("filledcurves")   == std::string::npos  &&
        stylestr.find("histograms")     == std::string::npos  )   //only for one data column
//        stylestr.find("labels")         == std::string::npos  &&  // 3 columns of data are required
//        stylestr.find("xerrorbars")     == std::string::npos  &&  // 3-4 columns of data are required
//        stylestr.find("xerrorlines")    == std::string::npos  &&  // 3-4 columns of data are required
//        stylestr.find("errorbars")      == std::string::npos  &&  // 3-4 columns of data are required
//        stylestr.find("errorlines")     == std::string::npos  &&  // 3-4 columns of data are required
//        stylestr.find("yerrorbars")     == std::string::npos  &&  // 3-4 columns of data are required
//        stylestr.find("yerrorlines")    == std::string::npos  &&  // 3-4 columns of data are required
//        stylestr.find("boxerrorbars")   == std::string::npos  &&  // 3-5 columns of data are required
//        stylestr.find("xyerrorbars")    == std::string::npos  &&  // 4,6,7 columns of data are required
//        stylestr.find("xyerrorlines")   == std::string::npos  &&  // 4,6,7 columns of data are required
//        stylestr.find("boxxyerrorbars") == std::string::npos  &&  // 4,6,7 columns of data are required
//        stylestr.find("financebars")    == std::string::npos  &&  // 5 columns of data are required
//        stylestr.find("candlesticks")   == std::string::npos  &&  // 5 columns of data are required
//        stylestr.find("vectors")        == std::string::npos  &&
//        stylestr.find("image")          == std::string::npos  &&
//        stylestr.find("rgbimage")       == std::string::npos  &&
//        stylestr.find("pm3d")           == std::string::npos  )
    {
        pstyle = std::string("points");
    }
    else
    {
        pstyle = stylestr;
    }

    return *this;
}


//----------------------------------------------------------------------------------
//
// smooth: interpolation and approximation of data
//
Gnuplot& Gnuplot::set_smooth(const std::string &stylestr)
{
    if (stylestr.find("unique")    == std::string::npos  &&
        stylestr.find("frequency") == std::string::npos  &&
        stylestr.find("csplines")  == std::string::npos  &&
        stylestr.find("acsplines") == std::string::npos  &&
        stylestr.find("bezier")    == std::string::npos  &&
        stylestr.find("sbezier")   == std::string::npos  )
    {
        smooth = "";
    }
    else
    {
        smooth = stylestr;
    }

    return *this;
}


//----------------------------------------------------------------------------------
//
// unset smooth
//
Gnuplot& Gnuplot::unset_smooth()
{
    smooth = "";

    return *this;
}



//----------------------------------------------------------------------------------
//
// sets terminal type to windows / x11
//
Gnuplot& Gnuplot::showonscreen()
{
    cmd("set output");
    cmd("set terminal " + Gnuplot::terminal_std);

    return *this;
}

//----------------------------------------------------------------------------------
//
// saves a gnuplot session to a postscript file
//
Gnuplot& Gnuplot::savetops(const std::string &filename)
{
    cmd("set terminal postscript color");

    std::ostringstream cmdstr;
    cmdstr << "set output \"" << filename << ".ps\"";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// Switches legend on
//
Gnuplot& Gnuplot::set_legend(const std::string &position)
{
    std::ostringstream cmdstr;
    cmdstr << "set key " << position;

    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// Switches legend off
//
Gnuplot& Gnuplot::unset_legend()
{
    cmd("unset key");

    return *this;
}

//----------------------------------------------------------------------------------
//
// Turns grid on
//
Gnuplot& Gnuplot::set_grid()
{
    cmd("set grid");

    return *this;
}

//----------------------------------------------------------------------------------
//
// Turns grid off
//
Gnuplot& Gnuplot::unset_grid()
{
    cmd("unset grid");

    return *this;
}

//----------------------------------------------------------------------------------
//
// turns on log scaling for the x axis
//
Gnuplot& Gnuplot::set_xlogscale(const double base)
{
    std::ostringstream cmdstr;

    cmdstr << "set logscale x " << base;
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// turns on log scaling for the y axis
//
Gnuplot& Gnuplot::set_ylogscale(const double base)
{
    std::ostringstream cmdstr;

    cmdstr << "set logscale y " << base;
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// turns on log scaling for the z axis
//
Gnuplot& Gnuplot::set_zlogscale(const double base)
{
    std::ostringstream cmdstr;

    cmdstr << "set logscale z " << base;
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// turns off log scaling for the x axis
//
Gnuplot& Gnuplot::unset_xlogscale()
{
    cmd("unset logscale x");
    return *this;
}

//----------------------------------------------------------------------------------
//
// turns off log scaling for the y axis
//
Gnuplot& Gnuplot::unset_ylogscale()
{
    cmd("unset logscale y");
    return *this;
}

//----------------------------------------------------------------------------------
//
// turns off log scaling for the z axis
//
Gnuplot& Gnuplot::unset_zlogscale()
{
    cmd("unset logscale z");
    return *this;
}


//----------------------------------------------------------------------------------
//
// scales the size of the points used in plots
//
Gnuplot& Gnuplot::set_pointsize(const double pointsize)
{
    std::ostringstream cmdstr;
    cmdstr << "set pointsize " << pointsize;
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// set isoline density (grid) for plotting functions as surfaces
//
Gnuplot& Gnuplot::set_samples(const int samples)
{
    std::ostringstream cmdstr;
    cmdstr << "set samples " << samples;
    cmd(cmdstr.str());

    return *this;
}


//----------------------------------------------------------------------------------
//
// set isoline density (grid) for plotting functions as surfaces
//
Gnuplot& Gnuplot::set_isosamples(const int isolines)
{
    std::ostringstream cmdstr;
    cmdstr << "set isosamples " << isolines;
    cmd(cmdstr.str());

    return *this;
}


//----------------------------------------------------------------------------------
//
// enables hidden line removal for surface plotting
//
Gnuplot& Gnuplot::set_hidden3d()
{
    cmd("set hidden3d");

    return *this;
}

//----------------------------------------------------------------------------------
//
// disables hidden line removal for surface plotting
//
Gnuplot& Gnuplot::unset_hidden3d()
{
    cmd("unset hidden3d");

    return *this;
}

//----------------------------------------------------------------------------------
//
// enables contour drawing for surfaces set contour {base | surface | both}
//

Gnuplot& Gnuplot::set_contour(const std::string &position)
{
    if (position.find("base")    == std::string::npos  &&
        position.find("surface") == std::string::npos  &&
        position.find("both")    == std::string::npos  )
    {
        cmd("set contour base");
    }
    else
    {
        cmd("set contour " + position);
    }

    return *this;
}

//----------------------------------------------------------------------------------
//
// disables contour drawing for surfaces
//

Gnuplot& Gnuplot::unset_contour()
{
    cmd("unset contour");

    return *this;
}


//----------------------------------------------------------------------------------
//
// enables the display of surfaces (for 3d plot)
//
Gnuplot& Gnuplot::set_surface()
{
    cmd("set surface");

    return *this;
}


//----------------------------------------------------------------------------------
//
// disables the display of surfaces (for 3d plot)
//
Gnuplot& Gnuplot::unset_surface()
{
    cmd("unset surface");

    return *this;
}


//----------------------------------------------------------------------------------
//
// Sets the title of a gnuplot session
//
Gnuplot& Gnuplot::set_title(const std::string &title)
{
    std::ostringstream cmdstr;

    cmdstr << "set title \"" << title << "\"";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// Clears the title of a gnuplot session
//
Gnuplot& Gnuplot::unset_title()
{
    set_title("");

    return *this;
}

//----------------------------------------------------------------------------------
//
// set labels
//
// set the xlabel
Gnuplot& Gnuplot::set_xlabel(const std::string &label)
{
    std::ostringstream cmdstr;

    cmdstr << "set xlabel \"" << label << "\"";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
// set the ylabel
//
Gnuplot& Gnuplot::set_ylabel(const std::string &label)
{
    std::ostringstream cmdstr;

    cmdstr << "set ylabel \"" << label << "\"";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
// set the zlabel
//
Gnuplot& Gnuplot::set_zlabel(const std::string &label)
{
    std::ostringstream cmdstr;

    cmdstr << "set zlabel \"" << label << "\"";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// set range
//
// set the xrange
Gnuplot& Gnuplot::set_xrange(const double iFrom,
                             const double iTo)
{
    std::ostringstream cmdstr;

    cmdstr << "set xrange[" << iFrom << ":" << iTo << "]";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
// set autoscale x
//
Gnuplot& Gnuplot::set_xautoscale()
{
    cmd("set xrange restore");
    cmd("set autoscale x");

    return *this;
}

//----------------------------------------------------------------------------------
// set the yrange
//
Gnuplot& Gnuplot::set_yrange(const double iFrom,
                             const double iTo)
{
    std::ostringstream cmdstr;

    cmdstr << "set yrange[" << iFrom << ":" << iTo << "]";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
// set autoscale y
//
Gnuplot& Gnuplot::set_yautoscale()
{
    cmd("set yrange restore");
    cmd("set autoscale y");

    return *this;
}

//----------------------------------------------------------------------------------
// set the zrange
//
Gnuplot& Gnuplot::set_zrange(const double iFrom,
                             const double iTo)
{
    std::ostringstream cmdstr;

    cmdstr << "set zrange[" << iFrom << ":" << iTo << "]";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
// set autoscale z
//
Gnuplot& Gnuplot::set_zautoscale()
{
    cmd("set zrange restore");
    cmd("set autoscale z");

    return *this;
}

//----------------------------------------------------------------------------------
//
// set the palette range
//
Gnuplot& Gnuplot::set_cbrange(const double iFrom,
                              const double iTo)
{
    std::ostringstream cmdstr;

    cmdstr << "set cbrange[" << iFrom << ":" << iTo << "]";
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// Plots a linear equation y=ax+b (where you supply the
// slope a and intercept b)
//
Gnuplot& Gnuplot::plot_slope(const double a,
                             const double b,
                             const std::string &title)
{
    std::ostringstream cmdstr;
    //
    // command to be sent to gnuplot
    //
    if (nplots > 0  &&  two_dim == true)
        cmdstr << "replot ";
    else
        cmdstr << "plot ";

    cmdstr << a << " * x + " << b << " title \"";

    if (title == "")
        cmdstr << "f(x) = " << a << " * x + " << b;
    else
        cmdstr << title;

    cmdstr << "\" with " << pstyle;

    //
    // Do the actual plot
    //
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// Plot an equation which is supplied as a std::string y=f(x) (only f(x) expected)
//
Gnuplot& Gnuplot::plot_equation(const std::string &equation,
                                const std::string &title)
{
    std::ostringstream cmdstr;
    //
    // command to be sent to gnuplot
    //
    if (nplots > 0  &&  two_dim == true)
        cmdstr << "replot ";
    else
        cmdstr << "plot ";

    cmdstr << equation << " title \"";

    if (title == "")
        cmdstr << "f(x) = " << equation;
    else
        cmdstr << title;

    cmdstr << "\" with " << pstyle;

    //
    // Do the actual plot
    //
    cmd(cmdstr.str());

    return *this;
}

//----------------------------------------------------------------------------------
//
// plot an equation supplied as a std::string y=(x)
//
Gnuplot& Gnuplot::plot_equation3d(const std::string &equation,
                                  const std::string &title)
{
    std::ostringstream cmdstr;
    //
    // command to be sent to gnuplot
    //
    if (nplots > 0  &&  two_dim == false)
        cmdstr << "replot ";
    else
        cmdstr << "splot ";

    cmdstr << equation << " title \"";

    if (title == "")
        cmdstr << "f(x,y) = " << equation;
    else
        cmdstr << title;

    cmdstr << "\" with " << pstyle;

    //
    // Do the actual plot
    //
    cmd(cmdstr.str());

    return *this;
}


//----------------------------------------------------------------------------------
//
// Plots a 2d graph from a list of doubles (x) saved in a file
//
Gnuplot& Gnuplot::plotfile_x(const std::string &filename,
                             const unsigned int column,
                             const std::string &title)
{
    //
    // check if file exists
    //
    if( !(Gnuplot::file_exists(filename,4)) ) // check existence and read permission
    {
        std::ostringstream except;
        if( !(Gnuplot::file_exists(filename,0)) ) // check existence
            except << "File \"" << filename << "\" does not exist";
        else
            except << "No read permission for File \"" << filename << "\"";
        throw GnuplotException( except.str() );
        return *this;
    }


    std::ostringstream cmdstr;
    //
    // command to be sent to gnuplot
    //
    if (nplots > 0  &&  two_dim == true)
        cmdstr << "replot ";
    else
        cmdstr << "plot ";

    cmdstr << "\"" << filename << "\" using " << column;

    if (title == "")
        cmdstr << " notitle ";
    else
        cmdstr << " title \"" << title << "\" ";

    if(smooth == "")
        cmdstr << "with " << pstyle;
    else
        cmdstr << "smooth " << smooth;

    //
    // Do the actual plot
    //
    cmd(cmdstr.str()); //nplots++; two_dim = true;  already in cmd();

    return *this;
}



//----------------------------------------------------------------------------------
//
// Plots a 2d graph from a list of doubles (x y) saved in a file
//
Gnuplot& Gnuplot::plotfile_xy(const std::string &filename,
                              const unsigned int column_x,
                              const unsigned int column_y,
                              const std::string &title)
{
    //
    // check if file exists
    //
    if( !(Gnuplot::file_exists(filename,4)) ) // check existence and read permission
    {
        std::ostringstream except;
        if( !(Gnuplot::file_exists(filename,0)) ) // check existence
            except << "File \"" << filename << "\" does not exist";
        else
            except << "No read permission for File \"" << filename << "\"";
        throw GnuplotException( except.str() );
        return *this;
    }


    std::ostringstream cmdstr;
    //
    // command to be sent to gnuplot
    //
    if (nplots > 0  &&  two_dim == true)
        cmdstr << "replot ";
    else
        cmdstr << "plot ";

    cmdstr << "\"" << filename << "\" using " << column_x << ":" << column_y;

    if (title == "")
        cmdstr << " notitle ";
    else
        cmdstr << " title \"" << title << "\" ";

    if(smooth == "")
        cmdstr << "with " << pstyle;
    else
        cmdstr << "smooth " << smooth;

    //
    // Do the actual plot
    //
    cmd(cmdstr.str());

    return *this;
}


//----------------------------------------------------------------------------------
//
// Plots a 2d graph with errorbars from a list of doubles (x y dy) saved in a file
//
Gnuplot& Gnuplot::plotfile_xy_err(const std::string &filename,
                                  const unsigned int column_x,
                                  const unsigned int column_y,
                                  const unsigned int column_dy,
                                  const std::string &title)
{
    //
    // check if file exists
    //
    if( !(Gnuplot::file_exists(filename,4)) ) // check existence and read permission
    {
        std::ostringstream except;
        if( !(Gnuplot::file_exists(filename,0)) ) // check existence
            except << "File \"" << filename << "\" does not exist";
        else
            except << "No read permission for File \"" << filename << "\"";
        throw GnuplotException( except.str() );
        return *this;
    }


    std::ostringstream cmdstr;
    //
    // command to be sent to gnuplot
    //
    if (nplots > 0  &&  two_dim == true)
        cmdstr << "replot ";
    else
        cmdstr << "plot ";

    cmdstr << "\"" << filename << "\" using " << column_x << ":" << column_y;

    if (title == "")
        cmdstr << " notitle ";
    else
        cmdstr << " title \"" << title << "\" ";

    cmdstr << "with " << pstyle << ", \"" << filename << "\" using "
           << column_x << ":" << column_y << ":" << column_dy << " notitle with errorbars";


    //
    // Do the actual plot
    //
    cmd(cmdstr.str());

    return *this;
}


//----------------------------------------------------------------------------------
//
// plot x,y pairs with dy errorbars
//
Gnuplot& Gnuplot::plot_xy_err(const std::vector<double> &x,
                              const std::vector<double> &y,
                              const std::vector<double> &dy,
                              const std::string &title)
{
    if (x.size() == 0 || y.size() == 0 || dy.size() == 0)
    {
        throw GnuplotException("std::vectors too small");
        return *this;
    }

    if (x.size() != y.size() || y.size() != dy.size())
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
        tmp << x[i] << " " << y[i] << " " << dy[i] << std::endl;

    tmp.flush();
    tmp.close();


    // Do the actual plot
    plotfile_xy_err(name, 1, 2, 3, title);

    return *this;
}


//----------------------------------------------------------------------------------
//
// Plots a 3d graph from a list of doubles (x y z) saved in a file
//
Gnuplot& Gnuplot::plotfile_xyz(const std::string &filename,
                               const unsigned int column_x,
                               const unsigned int column_y,
                               const unsigned int column_z,
                               const std::string &title)
{
    //
    // check if file exists
    //
    if( !(Gnuplot::file_exists(filename,4)) ) // check existence and read permission
    {
        std::ostringstream except;
        if( !(Gnuplot::file_exists(filename,0)) ) // check existence
            except << "File \"" << filename << "\" does not exist";
        else
            except << "No read permission for File \"" << filename << "\"";
        throw GnuplotException( except.str() );
        return *this;
    }


    std::ostringstream cmdstr;
    //
    // command to be sent to gnuplot
    //
    if (nplots > 0  &&  two_dim == false)
        cmdstr << "replot ";
    else
        cmdstr << "splot ";

    cmdstr << "\"" << filename << "\" using " << column_x << ":" << column_y << ":" << column_z;

    if (title == "")
        cmdstr << " notitle with " << pstyle;
    else
        cmdstr << " title \"" << title << "\" with " << pstyle;

    //
    // Do the actual plot
    //
    cmd(cmdstr.str());

    return *this;
}


//----------------------------------------------------------------------------------
//
// Plots a 3d graph from a list of doubles: x y z
//
Gnuplot& Gnuplot::plot_xyz(const std::vector<double> &x,
                           const std::vector<double> &y,
                           const std::vector<double> &z,
                           const std::string &title)
{
    if (x.size() == 0 || y.size() == 0 || z.size() == 0)
    {
        throw GnuplotException("std::vectors too small");
        return *this;
    }

    if (x.size() != y.size() || x.size() != z.size())
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
    {
        tmp << x[i] << " " << y[i] << " " << z[i] <<std::endl;
    }

    tmp.flush();
    tmp.close();


    plotfile_xyz(name, 1, 2, 3, title);

    return *this;
}



//----------------------------------------------------------------------------------
//
/// *  note that this function is not valid for versions of GNUPlot below 4.2
//
Gnuplot& Gnuplot::plot_image(const unsigned char * ucPicBuf,
                             const unsigned int iWidth,
                             const unsigned int iHeight,
                             const std::string &title)
{
    std::ofstream tmp;
    std::string name = create_tmpfile(tmp);
    if (name == "")
        return *this;

    //
    // write the data to file
    //
    int iIndex = 0;
    for(int iRow = 0; iRow < iHeight; iRow++)
    {
        for(int iColumn = 0; iColumn < iWidth; iColumn++)
        {
            tmp << iColumn << " " << iRow  << " " << static_cast<float>(ucPicBuf[iIndex++]) << std::endl;
        }
    }

    tmp.flush();
    tmp.close();


    std::ostringstream cmdstr;
    //
    // command to be sent to gnuplot
    //
    if (nplots > 0  &&  two_dim == true)
        cmdstr << "replot ";
    else
        cmdstr << "plot ";

    if (title == "")
        cmdstr << "\"" << name << "\" with image";
    else
        cmdstr << "\"" << name << "\" title \"" << title << "\" with image";

    //
    // Do the actual plot
    //
    cmd(cmdstr.str());

    return *this;
}



//----------------------------------------------------------------------------------
//
// Sends a command to an active gnuplot session
//
Gnuplot& Gnuplot::cmd(const std::string &cmdstr)
{
    if( !(valid) )
    {
        return *this;
    }


    // int fputs ( const char * str, FILE * stream );
    // writes the string str to the stream.
    // The function begins copying from the address specified (str) until it reaches the
    // terminating null character ('\0'). This final null-character is not copied to the stream.
    fputs( (cmdstr+"\n").c_str(), gnucmd );

    // int fflush ( FILE * stream );
    // If the given stream was open for writing and the last i/o operation was an output operation,
    // any unwritten data in the output buffer is written to the file.
    // If the argument is a null pointer, all open files are flushed.
    // The stream remains open after this call.
    fflush(gnucmd);


    if( cmdstr.find("replot") != std::string::npos )
    {
        return *this;
    }
    else if( cmdstr.find("splot") != std::string::npos )
    {
        two_dim = false;
        nplots++;
    }
    else if( cmdstr.find("plot") != std::string::npos )
    {
        two_dim = true;
        nplots++;
    }

    return *this;
}



//----------------------------------------------------------------------------------
//
// Sends a command to an active gnuplot session, identical to cmd()
//
Gnuplot& Gnuplot::operator<<(const std::string &cmdstr)
{
    cmd(cmdstr);
    return *this;
}



//----------------------------------------------------------------------------------
//
// Opens up a gnuplot session, ready to receive commands
//
void Gnuplot::init()
{
    // char * getenv ( const char * name );  get value of an environment variable
    // Retrieves a C string containing the value of the environment variable whose name is specified as argument.
    // If the requested variable is not part of the environment list, the function returns a NULL pointer.
#if ( defined(unix) || defined(__unix) || defined(__unix__) ) && !defined(__APPLE__)
    if (getenv("DISPLAY") == NULL)
    {
        valid = false;
        throw GnuplotException("Can't find DISPLAY variable");
    }
#endif


    // if gnuplot not available
    if (!Gnuplot::get_program_path())
    {
        valid = false;
        throw GnuplotException("Can't find gnuplot");
    }


    //
    // open pipe
    //
    std::string tmp = Gnuplot::m_sGNUPlotPath + "/" + Gnuplot::m_sGNUPlotFileName;

    // FILE *popen(const char *command, const char *mode);
    // The popen() function shall execute the command specified by the string command,
    // create a pipe between the calling program and the executed command, and
    // return a pointer to a stream that can be used to either read from or write to the pipe.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    gnucmd = _popen(tmp.c_str(),"w");
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    gnucmd = popen(tmp.c_str(),"w");
#endif

    // popen() shall return a pointer to an open stream that can be used to read or write to the pipe.
    // Otherwise, it shall return a null pointer and may set errno to indicate the error.
    if (!gnucmd)
    {
        valid = false;
        throw GnuplotException("Couldn't open connection to gnuplot");
    }

    nplots = 0;
    valid = true;
    smooth = "";

    //set terminal type
    showonscreen();

    return;
}


//----------------------------------------------------------------------------------
//
// Find out if a command lives in m_sGNUPlotPath or in PATH
//
bool Gnuplot::get_program_path()
{
    //
    // first look in m_sGNUPlotPath for Gnuplot
    //
    std::string tmp = Gnuplot::m_sGNUPlotPath + "/" + Gnuplot::m_sGNUPlotFileName;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    if ( Gnuplot::file_exists(tmp,0) ) // check existence
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if ( Gnuplot::file_exists(tmp,1) ) // check existence and execution permission
#endif
    {
        return true;
    }


    //
    // second look in PATH for Gnuplot
    //
    char *path;
    // Retrieves a C string containing the value of the environment variable PATH
    path = getenv("PATH");


    if (path == NULL)
    {
        throw GnuplotException("Path is not set");
        return false;
    }
    else
    {
        std::list<std::string> ls;

        //split path (one long string) into list ls of strings
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
        stringtok(ls,path,";");
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
        stringtok(ls,path,":");
#endif

        // scan list for Gnuplot program files
        for (std::list<std::string>::const_iterator i = ls.begin(); i != ls.end(); ++i)
        {
            tmp = (*i) + "/" + Gnuplot::m_sGNUPlotFileName;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
            if ( Gnuplot::file_exists(tmp,0) ) // check existence
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
            if ( Gnuplot::file_exists(tmp,1) ) // check existence and execution permission
#endif
            {
                Gnuplot::m_sGNUPlotPath = *i; // set m_sGNUPlotPath
                return true;
            }
        }

        tmp = "Can't find gnuplot neither in PATH nor in \"" + Gnuplot::m_sGNUPlotPath + "\"";
        throw GnuplotException(tmp);

        Gnuplot::m_sGNUPlotPath = "";
        return false;
    }
}



//----------------------------------------------------------------------------------
//
// check if file exists
//
bool Gnuplot::file_exists(const std::string &filename, int mode)
{
    if ( mode < 0 || mode > 7)
    {
        throw std::runtime_error("In function \"Gnuplot::file_exists\": mode has to be an integer between 0 and 7");
        return false;
    }

    // int _access(const char *path, int mode);
    //  returns 0 if the file has the given mode,
    //  it returns -1 if the named file does not exist or is not accessible in the given mode
    // mode = 0 (F_OK) (default): checks file for existence only
    // mode = 1 (X_OK): execution permission
    // mode = 2 (W_OK): write permission
    // mode = 4 (R_OK): read permission
    // mode = 6       : read and write permission
    // mode = 7       : read, write and execution permission
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    if (_access(filename.c_str(), mode) == 0)
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if (access(filename.c_str(), mode) == 0)
#endif
    {
        return true;
    }
    else
    {
        return false;
    }

}



//----------------------------------------------------------------------------------
//
// Opens a temporary file
//
std::string Gnuplot::create_tmpfile(std::ofstream &tmp)
{

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    char name[] = "gnuplotiXXXXXX"; //tmp file in working directory
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    char name[] = "/tmp/gnuplotiXXXXXX"; // tmp file in /tmp
#endif

    //
    // check if maximum number of temporary files reached
    //
    if (Gnuplot::tmpfile_num == GP_MAX_TMP_FILES - 1)
    {
        std::ostringstream except;
        except << "Maximum number of temporary files reached (" << GP_MAX_TMP_FILES
               << "): cannot open more files" << std::endl;

        throw GnuplotException( except.str() );
        return "";
    }

    // int mkstemp(char *name);
    // shall replace the contents of the string pointed to by "name" by a unique filename,
    // and return a file descriptor for the file open for reading and writing.
    // Otherwise, -1 shall be returned if no suitable file could be created.
    // The string in template should look like a filename with six trailing 'X' s;
    // mkstemp() replaces each 'X' with a character from the portable filename character set.
    // The characters are chosen such that the resulting name does not duplicate the name of an existing file at the time of a call to mkstemp()


    //
    // open temporary files for output
    //
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    if (_mktemp(name) == NULL)
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if (mkstemp(name) == -1)
#endif
    {
        std::ostringstream except;
        except << "Cannot create temporary file \"" << name << "\"";
        throw GnuplotException(except.str());
        return "";
    }

    tmp.open(name);
    if (tmp.bad())
    {
        std::ostringstream except;
        except << "Cannot create temporary file \"" << name << "\"";
        throw GnuplotException(except.str());
        return "";
    }

    //
    // Save the temporary filename
    //
    tmpfile_list.push_back(name);
    Gnuplot::tmpfile_num++;

    return name;
}
