/* 
  Example use of openCV to convert a colour image to greyscale
  ------------------------------------------------------------
 
  This application reads a sequence lines from an input file colourToGreyscaleInput.txt.
  Each line contains a filename of an image to be processed. 

  It is assumed that the input file is located in a data directory given by the path ../data/ 
  defined relative to the location of executable for this application.

  (This is the application file: it contains the client code that calls dedicated functions to implement the application.
  The code for these functions is defined in the implementation file. The functions are declared in the interface file.)

  David Vernon
  24 November 2017
    
  Audit Trail
  --------------------
  Removed ../data/ prefix from colourToGreyscaleInput.txt entries
  Abrham Gebreselasie
  3 March 2021
  
  Ported to Ubuntu 16.04 and OpenCV 3.3
  Abrham Gebreselasie
  10 March 2021
  

*/
 
#include "module5/colourToGreyscale.h"

int main() {

   /* removing this stops a core dump on exit. DV 28/10/2021
   #ifdef ROS
      // Turn off canonical terminal mode and character echoing
      static const int STDIN = 0;
      termios term, old_term;
      tcgetattr(STDIN, &old_term);
      tcgetattr(STDIN, &term);
      term.c_lflag &= ~(ICANON | ECHO);
      tcsetattr(STDIN, TCSANOW, &term);
   #endif 
   */
    
   const char input_filename[MAX_FILENAME_LENGTH] = "colourToGreyscaleInput.txt";    
   char input_path_and_filename[MAX_FILENAME_LENGTH];    
   char data_dir[MAX_FILENAME_LENGTH];
   char file_path_and_filename[MAX_FILENAME_LENGTH];
     

   int end_of_file;
   bool debug = true;
   char filename[MAX_FILENAME_LENGTH];

   FILE *fp_in;
   
   
   #ifdef ROS   
      strcpy(data_dir, ros::package::getPath(ROS_PACKAGE_NAME).c_str()); // get the package directory
   #else
      strcpy(data_dir, "..");
   #endif
   
   strcat(data_dir, "/data/");
   strcpy(input_path_and_filename, data_dir);
   strcat(input_path_and_filename, input_filename);
   

   if ((fp_in = fopen(input_path_and_filename,"r")) == 0) {
	  printf("Error can't open input colourToGreyscaleInput.txt\n");
     prompt_and_exit(1);
   }

   printf("Example of how to use openCV to converta colour image to a greyscale image.\n\n");
   
   do {

      end_of_file = fscanf(fp_in, "%s", filename);
      
      if (end_of_file != EOF) {
         strcpy(file_path_and_filename, data_dir);
         strcat(file_path_and_filename, filename);
         strcpy(filename, file_path_and_filename);

         printf("\nConverting to greyscale a colour image in %s \n",filename);
         colourToGreyscale(filename);
      }
   } while (end_of_file != EOF);


   fclose(fp_in);

   /* removing this stops a core dump on exit DV 28/10/2021
   #ifdef ROS
      // Reset terminal
      tcsetattr(STDIN, TCSANOW, &old_term);
   #endif
   */
   
   return 0;
}

