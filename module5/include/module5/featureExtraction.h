/* 
  Example use of openCV to perform 2D feature extraction
  ------------------------------------------------------
 
  (This is the interface file: it contains the declarations of dedicated functions to implement the application.
  These function are called by client code in the application file. The functions are defined in the implementation file.)

  David Vernon
  24 November 2017
      
  Ported to OpenCV 4
  David Vernon
  11 July 2024
*/
 


#define GCC_COMPILER (defined(__GNUC__) && !defined(__clang__))

#if GCC_COMPILER
   #ifndef ROS
       #define ROS
   #endif
   #ifndef ROS_PACKAGE_NAME
      #define ROS_PACKAGE_NAME "module5"
   #endif
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include <iostream>
#include <string>

#ifndef ROS
   #include <conio.h>
#else
   #include <sys/select.h>
   #include <termios.h>
   #include <sys/ioctl.h>
#endif
    

//opencv
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#ifdef ROS
   // ncurses.h must be included after opencv2/opencv.hpp to avoid incompatibility
   #include <ncurses.h>
   #include <ros/ros.h>
   #include <ros/package.h>
#endif 
    
 

#define TRUE  1
#define FALSE 0
#define MAX_STRING_LENGTH 80
#define MAX_FILENAME_LENGTH 200

using namespace std;
using namespace cv;

/* function prototypes go here */

void featureExtraction(char *filename, FILE *fp_out);
void prompt_and_exit(int status);
void prompt_and_continue();

#ifdef ROS
   int _kbhit();
#endif
   
