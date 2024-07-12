/* 
  Example use of openCV to convert a colour image to greyscale
  ------------------------------------------------------------
  
  (This is the implementation file: it contains the code for dedicated functions to implement the application.
  These functions are called by client code in the application file. The functions are declared in the interface file.) 

  David Vernon
  24 November 2017

  Audit Trail
  --------------------
  Added _kbhit
  18 February 2021
     
  Ported to OpenCV 4
  David Vernon
  11 July 2024
*/
 
#include "module5/colourToGreyscale.h"
 
void colourToGreyscale(char *filename) {
  
   char inputWindowName[MAX_STRING_LENGTH]   = "Input Image";
   char outputWindowName[MAX_STRING_LENGTH]  = "Greyscale Image";
 
   Mat colourImage;
   Mat greyscaleImage;

   int row;
   int col;
   int channel;
   int temp;

   namedWindow(inputWindowName,   WINDOW_AUTOSIZE);  
   namedWindow(outputWindowName,  WINDOW_AUTOSIZE);

   colourImage = imread(filename, IMREAD_COLOR);        // Read the file
   // colourImage = imread(filename, IMREAD_GRAYSCALE); // just for testing
  
   printf("number of channels %d\n", colourImage.channels());

   if (!colourImage.data) {                            // Check for invalid input
      printf("Error: failed to read image %s\n",filename);
      prompt_and_exit(-1);
   }

   printf("Press any key to continue ...\n");

   /* test image */
   /*
   for (row=0; row < colourImage.rows; row++) {
      for (col=0; col < colourImage.cols; col++) {
         for (channel=0; channel < colourImage.channels(); channel++) {
            if (colourImage.channels() == 1) { // failsafe just in case the colourImage is not a colour image or a multichannel image 
			      colourImage.at<uchar>(row,col) = 80 * (col % 3);             // test image comprises bands of greyscale 0, 80, and 160
			   }
			   else { 
               colourImage.at<Vec3b>(row,col)[channel] = 80 * (col % 3);    // test image comprises bands of greyscale 0, 80, and 160
			   }
		   } 
      }
   }
   */
   imshow(inputWindowName, colourImage);        
  
   //CV_Assert(colourImage.type() == CV_8UC3);

   // convert to greyscale by explicit access to colour image pixels
   // we do this simply as an example of one way to access individual pixels
   // see changeQuantisation() for a more efficient method that accesses pixels using pointers
  
   greyscaleImage.create(colourImage.size(), CV_8UC1);
  
   for (row=0; row < colourImage.rows; row++) {
      for (col=0; col < colourImage.cols; col++) {
         temp = 0;
         for (channel=0; channel < colourImage.channels(); channel++) {
            if (colourImage.channels()== 1) { 
			      // failsafe just in case the colour image is not a colour image or a multichannel image 
			      //temp += colourImage.at<Vec3b>(row,col)[channel]; // don't use this
			      temp += colourImage.at<uchar>(row,col);            // use this
			   }
			   else { 
			      temp += colourImage.at<Vec3b>(row,col)[channel];
			   }
		   }
         greyscaleImage.at<uchar>(row,col) = (uchar) (temp / colourImage.channels());
      }
   }

   // alternative ... use OpenCV!!!
   // cvtColor(colourImage, greyscaleImage, COLOR_BGR2GRAY);

   imshow(outputWindowName, greyscaleImage); 

   do{
      waitKey(30);                                  // Must call this to allow openCV to display the images
   } while (!_kbhit());                             // We call it repeatedly to allow the user to move the windows
                                                    // (if we don't the window process hangs when you try to click and drag

   getchar(); // flush the buffer from the keyboard hit

   destroyWindow(inputWindowName);  
   destroyWindow(outputWindowName); 
}
 

/*=======================================================*/
/* Utility functions to prompt user to continue          */ 
/*=======================================================*/

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal ... \n");
   getchar();
   

   #ifdef ROS
      // Reset terminal to canonical mode
      static const int STDIN = 0;
      termios term;
      tcgetattr(STDIN, &term);
      term.c_lflag |= (ICANON | ECHO);
      tcsetattr(STDIN, TCSANOW, &term);
      exit(status);
   #endif

   exit(status);
}

void prompt_and_continue() {
   printf("Press any key to continue ... \n");
   getchar();
}



#ifdef ROS
/**
 Linux (POSIX) implementation of _kbhit().
 Morgan McGuire, morgan@cs.brown.edu
 */
int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
#endif
