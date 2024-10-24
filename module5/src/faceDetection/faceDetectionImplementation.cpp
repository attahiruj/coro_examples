/* 
  Example use of openCV to perform face detection using Haar features and boosted classification
  ----------------------------------------------------------------------------------------------
  
  (This is the implementation file: it contains the code for dedicated functions to implement the application.
  These functions are called by client code in the application file. The functions are declared in the interface file.) 

  David Vernon
  24 November 2017

  Ported to Ubuntu 16.04 and OpenCV 3.3
  David Vernon
  1 November 2022
  
  Ported to OpenCV 4
  David Vernon
  11 July 2024
*/

#include "module5/faceDetection.h"
 
void faceDetection(char *filename, CascadeClassifier& cascade) {
  
   VideoCapture camera;
   char inputWindowName[MAX_STRING_LENGTH]         = "Input Image";
   char outputWindowName[MAX_STRING_LENGTH]        = "Cascade of Haar Classifiers";
   Mat inputImage;
   Mat outputImage;
   char c;
   vector<Rect> faces;
   Mat gray;
   Mat current_frame;

   char input_path_and_filename[MAX_FILENAME_LENGTH];    
   char data_dir[MAX_FILENAME_LENGTH];
   char file_path_and_filename[MAX_FILENAME_LENGTH];
  
   int end_of_file;
   bool debug = true;
   char local_filename[MAX_FILENAME_LENGTH];
   
   namedWindow(outputWindowName,     WINDOW_AUTOSIZE);
   
   /* check to see if the image is the camera device     */
   /* if so, grab images live from the camera            */
   /* otherwise proceed to process the image in the file */
   
   if (strcmp(filename,"camera") != 0) {

      #ifdef ROS   
        strcpy(data_dir, ros::package::getPath(ROS_PACKAGE_NAME).c_str()); // get the package directory
      #else
        strcpy(data_dir, "..");
      #endif
   
      strcat(data_dir, "/data/");
      strcpy(local_filename, data_dir);
      strcat(local_filename, filename);
	 
      inputImage = imread(local_filename, IMREAD_COLOR); // Read the file

      if (!inputImage.data) {                            // Check for invalid input
         printf("Error: failed to read image %s\n",local_filename);
         prompt_and_exit(-1);
      }

      printf("Press any key to continue ...\n");

      cvtColor(inputImage, gray, COLOR_BGR2GRAY );
      equalizeHist(gray, gray );
      cascade.detectMultiScale( gray, faces, 1.1, 2, CASCADE_SCALE_IMAGE, Size(30, 30) );

      for (int count = 0; count < (int)faces.size(); count++ )
         rectangle(inputImage, faces[count], cv::Scalar(255,0,0), 2);

      imshow(outputWindowName, inputImage);  
   }
   else {
     
      /* --------------------------------------------------------------------------------------------
       * Adapted from code provided as part of "A Practical Introduction to Computer Vision with OpenCV"
       * by Kenneth Dawson-Howe � Wiley & Sons Inc. 2014.  All rights reserved.
       */
   	
      // Cascade of Haar classifiers (most often shown for face detection).

      //camera.open(1); // David Vernon ... this is the original code and uses an external USB camera
      camera.open(0);   // David Vernon ... use this for the internal web camera
 
      camera.set(CAP_PROP_FRAME_WIDTH, 320);  // David Vernon ... has no effect on my webcam so resizing below
      camera.set(CAP_PROP_FRAME_HEIGHT, 240);
    
      if (camera.isOpened()) { 
	// Mat current_frame; // David Vernon ... moved to start of function
	 do {
	    camera >> current_frame;
	    if (current_frame.empty())
	       break;
  
	   // vector<Rect> faces; // David Vernon ... moved to start of function
	   // Mat gray;           // David Vernon ... moved to start of function

           //resize(current_frame,current_frame,Size(),0.5,0.5); // David Vernon 
            
	   cvtColor( current_frame, gray, COLOR_BGR2GRAY );
	   equalizeHist( gray, gray ); // David Vernon: irrespective of the equalization, well illumiated images are required
	   cascade.detectMultiScale( gray, faces, 1.1, 2, CASCADE_SCALE_IMAGE, Size(30, 30) );

	   for (int count = 0; count < (int)faces.size(); count++ )
	      rectangle(current_frame, faces[count], cv::Scalar(255,0,0), 2);

              imshow(outputWindowName, current_frame );
              c = waitKey(10);   // This makes the image appear on screen ... DV changed from original
           // } while (c == -1); // David Vernon
           } while (!_kbhit()); 
       }
   }
   /* --------------------------------------------------------------------------------------------- */

   do{
      waitKey(30);                                  
   } while (!_kbhit());       

   getchar(); // flush the buffer from the keyboard hit
    
   destroyWindow(outputWindowName);  
}
 


/*=======================================================*/
/* Utility functions to prompt user to continue          */ 
/*=======================================================*/

void prompt_and_exit(int status) {
   printf("Press any key to continue and close terminal ... \n");
   getchar();
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
