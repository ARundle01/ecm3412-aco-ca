// This header defines custom exceptions for the binPacking executable
#ifndef __BINPACKINGEXCEPTIONS_HPP
#define __BINPACKINGEXCEPTIONS_HPP

/*-- Includes --*/
#include <exception>

/*-- Use the std namespace --*/
using namespace std;

/*-- Exception Class Definitions --*/

/* Number of arguments supplied to executable is invalid */
class InvalidArgsException: public exception
{
    public:
        virtual const char *what() const throw()
        {
            return "Error: Invalid number of arguments supplied";
        }
};

/* Problem type supplied is invalid */
class InvalidProblemException: public exception
{
    public:
        virtual const char *what() const throw()
        {
            return "Error: Invalid problem type supplied";
        }
};

/* Number of ants supplied is invalid */
class InvalidNumAntsException: public exception
{
    public:
        virtual const char *what() const throw()
        {
            return "Error: Invalid number of ants supplied";
        }
};

/* Evaporation rate supplied is invalid */
class InvalidEvaporationRateException: public exception
{
    public:
        virtual const char *what() const throw()
        {
            return "Error: Invalid evaporation rate supplied";
        }
};

/*-- End of Header --*/
#endif