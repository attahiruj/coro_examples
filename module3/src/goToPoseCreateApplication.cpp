/*******************************************************************************************************************
*
*  Exercise 1: Implementation of the divide-and-conquer go-to-position algorithm (goto1) 
*              and the MIMO go-to-position algorithm (goto2) 
*              on the iRobot Create 2 mobile robot
*
*  The program reads an input file goToPoseInput.txt.
*
*  The first line contains a filename of the file with the locomotion parameter data
*  This allows the behaviour of the robot to be adjusted without recompiling the program. 
*
*  The remainder of the file contains a sequence of commands, one command per line.
*  
*  Each command comprises four fields:
*  a key string  (either "setpose", "goto1", or "goto2") followed by three floating point numbers.
*  
*  The three numbers give the pose of the robot (x, y, theta), specified in centimetres and radians, respectively.
*
*  In the case of the "setpose" command, the three numbers define the current absolute pose of the robot. 
*  This pose is use to register the pose data provided by odometry (which is relative to some arbitrary initial pose)
*  so that the current pose as the robot drives around  is defined relative to the specified absolute pose.
*
*  In the case of the "goto1" and "goto2" commands, the three numbers define the goal pose of the robot.
*
*  All poses are specified in metres and radians, respectively.
*
*  For each goto command, the robot drives to goal pose from its current pose using the specified algorithm.
*
*  The current robot pose is sensed by subscribing to the odom topic. This publishes robot odometry data according to wheel encoders.
*  The robot is driven by publishing velocity commands on the cmd_vel topic.
*
*   Sample Input
*   parameters.txt
*   setpose    0   0  0  
*   goto1      0 1.2  0
*   goto2      0   0  0
*
*   David Vernon
*   9 November 2021
*
*   Audit Trail
*   -----------
* 
*
*******************************************************************************************************************/

#include <module3/goToPoseCreate.h> 


main(int argc, char **argv) {
  
   bool                 debug = true;
   
   FILE                 *fp_in;                    
   std::string          packagedir;
   char                 path[MAX_FILENAME_LENGTH];
   char                 input_filename[MAX_FILENAME_LENGTH]                 = "goToPoseCreateInput.txt";
   char                 locomotion_parameter_filename[MAX_FILENAME_LENGTH]  = "";
   char                 path_and_input_filename[MAX_FILENAME_LENGTH]        = "";
   int                  end_of_file;
   bool                 success = true;

   geometry_msgs::Twist msg;
   
   float                x;
   float                y;
   float                theta;
   
   float                publish_rate         = 20;    // rate at which cmd_vel commands are published

   char                 command[10];
   
   struct locomotionParameterDataType locomotionParameterData;
   
  
   /* Initialize the ROS system and become a node */
   /* ------------------------------------------- */
   
   ros::init(argc, argv, "goToPose"); // Initialize the ROS system
   ros::NodeHandle nh;                // Become a node

   
   /* Create a subscriber object for the odom topic */
   /* --------------------------------------------- */
   
   if (debug) printf("Subscribing to odom\n");
   ros::Subscriber sub = nh.subscribe("odom", 1000, &odomMessageReceived);

   
   /* Create a publisher object for velocity commands */
   /* ----------------------------------------------- */
   
   if (debug) printf("publishing to cmd_vel\n");
   ros::Publisher  pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1000); 
   ros::Rate rate(publish_rate); // Publish  at this rate (in Hz)  until the node is shut down

   
   /* construct the full path and filename */
   /* ------------------------------------ */
   
   packagedir = ros::package::getPath(ROS_PACKAGE_NAME); // get the package directory
 
   if (debug) cout << "Package directory: " << packagedir << endl;

   strcat(path_and_input_filename, packagedir.c_str());  
   strcat(path_and_input_filename, "/data/"); 
   strcat(path_and_input_filename, input_filename);

   if (debug) printf("Input file is  %s\n",path_and_input_filename);

   /* open the input file */
   
   if ((fp_in = fopen(path_and_input_filename,"r")) == 0) {
      printf("Error: can't open %s\n",path_and_input_filename);
      prompt_and_exit(1);
   }

   
   /* get the locomotion parameter data */
   /* --------------------------------- */

   end_of_file = fscanf(fp_in, "%s", locomotion_parameter_filename); // read the locomotion parameter data  filename   
   if (end_of_file == EOF) {   
     printf("Fatal error: unable to read the locomotion parameter filename\n");
      prompt_and_exit(1);
   }
   if (debug) printf("Locomotion parameter filename %s\n", locomotion_parameter_filename);

   strcpy(path_and_input_filename, packagedir.c_str());  
   strcat(path_and_input_filename, "/data/"); 
   strcat(path_and_input_filename, locomotion_parameter_filename);

   if (debug) printf("Locomotion parameter file is  %s\n",path_and_input_filename);
   
   readLocomotionParameterData(path_and_input_filename, &locomotionParameterData);

   if (debug) { 
      printf("POSITION_TOLERANCE:        %f\n",locomotionParameterData.position_tolerance);
      printf("ANGLE_TOLERANCE_ORIENTING: %f\n",locomotionParameterData.angle_tolerance_orienting);
      printf("ANGLE_TOLERANCE_GOING:     %f\n",locomotionParameterData.angle_tolerance_going);
      printf("POSITION_GAIN_DQ:          %f\n",locomotionParameterData.position_gain_dq); 
      printf("ANGLE_GAIN_DQ:             %f\n",locomotionParameterData.angle_gain_dq); 
      printf("POSITION_GAIN_MIMO:        %f\n",locomotionParameterData.position_gain_mimo); 
      printf("ANGLE_GAIN_MIMO:           %f\n",locomotionParameterData.angle_gain_mimo);
      printf("MIN_LINEAR_VELOCITY:       %f\n",locomotionParameterData.min_linear_velocity);
      printf("MAX_LINEAR_VELOCITY:       %f\n",locomotionParameterData.max_linear_velocity);
      printf("MIN_ANGULAR_VELOCITY:      %f\n",locomotionParameterData.min_angular_velocity);
      printf("MAX_ANGULAR_VELOCITY:      %f\n",locomotionParameterData.max_angular_velocity);
   }

   /* optional: find the minimum linear and angular velocities that produce a robot movement */
   /*           this function is called only when calibrating the software                   */
  
   // findMinimumVelocities(pub, rate, locomotionParameterData.max_linear_velocity, locomotionParameterData.max_angular_velocity);
   // exit(1);

   /* process each command in the input file */
   /* -------------------------------------- */

   end_of_file=fscanf(fp_in, "%s %f %f %f", command, &x, &y, &theta);

   while (end_of_file != EOF) {

      if (debug) {
         printf("Input data: %s %f %f %f\n", command, x, y, theta);
      }

      if (strcmp(command, "setpose")==0) {

         /* initialize the odometry values to the initial pose of the robot */

         setOdometryPose(x, y, theta);

      }
      else if (strcmp(command, "goto1")==0) {

         /* use the divide and conquer algorithm to drive the robot to the required pose */
   
         goToPoseDQ(x, y, theta, locomotionParameterData, pub, rate);

      }
      else if (strcmp(command, "goto2")==0) {

         /* use the MIMO algorithm to drive the robot to the required position and then reorient the robot to the required pose */
   
         goToPoseMIMO1(x, y, theta, locomotionParameterData, pub, rate);

      }

      /* prompt user to continue between commands */

      //prompt_and_continue();
      
      end_of_file=fscanf(fp_in, "%s %f %f %f", command, &x, &y, &theta);

   }
}
