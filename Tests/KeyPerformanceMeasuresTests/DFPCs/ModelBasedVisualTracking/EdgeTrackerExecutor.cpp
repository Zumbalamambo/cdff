
/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include "EdgeTrackerExecutor.hpp"
#include "EdgeTrackerUtility.hpp"
#include <ctime>
#include <typeinfo>

using namespace CDFF;
using namespace DFPC;

using namespace Converters;
using namespace FrameWrapper;
using namespace SupportTypes;

using namespace EdgeTrackerHelper;

#define DELETE_IF_NOT_NULL(pointer) \
	if (pointer != NULL) \
		{ \
		delete(pointer); \
		}

/* --------------------------------------------------------------------------
 *
 * Public Member Functions
 *
 * --------------------------------------------------------------------------
 */
EdgeTrackerExecutor::EdgeTrackerExecutor()
	{
	
	inputLeftFrame = NULL;
	inputRightFrame = NULL;

	logGroundTruthError = false;
	
	inputImagesWereLoaded = false;
	outputPoseWasLoaded = false;
	dfpcExecuted = false;
	dfpcWasLoaded = false;
			
	}

EdgeTrackerExecutor::~EdgeTrackerExecutor()
	{
	
	DELETE_IF_NOT_NULL(inputLeftFrame);
	DELETE_IF_NOT_NULL(inputRightFrame);
	}

void EdgeTrackerExecutor::SetDfpc(std::string configurationFilePath, CDFF::DFPC::ModelBasedVisualTrackingInterface* dfpc)
	{
	this->configurationFilePath = configurationFilePath;
	this->dfpc = dfpc;

	ConfigureDfpc();
	dfpcWasLoaded = true;
	}

void EdgeTrackerExecutor::SetInputFilesPaths(std::string inputImagesFolder, std::string inputImagesListFileName, std::string inputPosesFolder, std::string inputPosesListFileName)
	{
		this->inputImagesFolder = inputImagesFolder;
		this->inputImagesListFileName = inputImagesListFileName;
		this->inputPosesFolder = inputPosesFolder ;
		this->inputPosesListFileName = inputPosesListFileName;

		LoadInputImagesList();
		if(!inputPosesFolder.empty())
			LoadInputPosesList();
		inputImagesWereLoaded = true;
	}


void EdgeTrackerExecutor::SetOutputFilePath(std::string outputPoseFilePath)
	{
	this->outputPoseFilePath = outputPoseFilePath;
	outputPoseWasLoaded = true;
	}

void EdgeTrackerExecutor::initPose(double* T_guess0)
	{
	
      /*  bool EPOS = false;
	//EPOS
	if(EPOS)
	{
	//T4_45
	  double T_guess00[16]= {0.984813, -0.173617, -3.39288e-05, -6482.22,
				-0.0716821, -0.406782, 0.910709, 140.687,
 				-0.158129, -0.896875, -0.413049, 52.7197, 
				 0.000000, 0.000000, 0.000000, 1.000000};
		//T5_00
	 double T_guess0[16]= {0.707102, -0.459395, 0.537552, -7272.01,
				 -0.707111, -0.459356, 0.537574, 91.05, 
				-3.06058e-05, -0.760229, -0.649655, 54.5402, 
 	 	 	 	0.000000, 0.000000, 0.000000, 1.000000};


	 double TAdapt[16]= {1.0, 0.0, 0.0, 0.0,
			     0.0 ,1.0, 0.0, 0.0,
			     0.0, 0.0, 1.0, 0.0,
			     0.0, 0.0, 0.0, 1.0};
	  
	  matrixProduct444(T_guess00, TAdapt, T_guess0);
	 }
	 else
	 {
*/
	  double T_guess00[16]= {0.755756, -0.272366, 0.595525, 213.044047,
			 	-0.008548, 0.905221, 0.424855, -202.064589,
			 	-0.654798, -0.326178, 0.681798, 519.789040,
	      	 	 	0.000000, 0.000000, 0.000000, 1.000000};
	 
	   double TAdapt[16]= {0.998782, 0.036096, 0.033639, 4.999887,
			   -0.034878 ,0.998739, -0.036096, 9.473143,
			   -0.034899, 0.034878, 0.998782, 0.348889,
			    0.000000, 0.000000, 0.000000, 1.000000};
	   matrixProduct444(T_guess00, TAdapt, T_guess0);
	//   }
	

	}

void EdgeTrackerExecutor::initVelocity(double* velocity0)
	{
	// often starts at  Rest 
	 double  startVelocity[6] = {0.00, 0.00, 0.00, 0.00, 0.00, 0.00};
	 velocity0 = &startVelocity[0];
	
	}

void EdgeTrackerExecutor::ExecuteDfpc()
     {
	ASSERT(inputImagesWereLoaded && dfpcWasLoaded, "Cannot execute DFPC, the input images or the DFPC itself are not loaded");
	ASSERT(leftImageFileNamesList.size() == rightImageFileNamesList.size(), "Left images list and right images list do not have same dimensions");
		
	double time_images;
	asn1SccTime imageAcquisitionTime;
	//Input: initial time- usually set to 0;
	double initTime = 0;
	asn1SccTime initialTime;
	initialTime.microseconds = initTime;
	dfpc->initTimeInput(initialTime);

	//Input: Initial pose 
	
	double vel0[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        double rotTrasl[6];
	double T_guess0[16];
	//matrixIdentity(T_guess0,4);
	initPose(T_guess0);

	printMatrix(" ## init T_guess0: ",T_guess0,4,4);
		
        double distanceInitial = fabs(T_guess0[3])+fabs(T_guess0[7])+fabs(T_guess0[11]);
	ASSERT(distanceInitial > 500, "Initial position is greater than 500 mm ");
	AngleAxisFromT(T_guess0, rotTrasl); //deg 

	asn1SccRigidBodyState initState;
	
	initState = ConvertStateToAsnState(rotTrasl, vel0);

	 dfpc->initInput(initState);
	        
	int successCounter = 0;
	float processingTime = 0;

	
	if(isFileExist(outputPoseFilePath))
	{
		if(remove(outputPoseFilePath.c_str()) != 0 )
   	  	 perror( "Error deleting file" );
	}
  	
	 std::ofstream writer;
	 double T_true[16];
 	
	for(int imageIndex = 0; imageIndex < (int)leftImageFileNamesList.size(); imageIndex++)
	{
	 std::stringstream leftImageFilePath, rightImageFilePath;
	 leftImageFilePath << inputImagesFolder << "/" << leftImageFileNamesList.at(imageIndex);
	 rightImageFilePath << inputImagesFolder << "/" << rightImageFileNamesList.at(imageIndex);

	LoadInputImage(leftImageFilePath.str(), inputLeftFrame);
	LoadInputImage(rightImageFilePath.str(), inputRightFrame);

	dfpc->imageLeftInput(inputLeftFrame); 
	dfpc->imageRightInput(inputRightFrame);
		
	if(logGroundTruthError)
	{
			/////------------------- Ground truth pose----------------------
	  std::stringstream poseFilePath;
	  poseFilePath << inputPosesFolder << "/" << poseFileNamesList.at(imageIndex);
	  LoadInputPose(poseFilePath.str(),T_true);
		
	  printMatrix(" ## ground truth matrix read from file: ",T_true,4,4);

	  //init tracker
	  AngleAxisFromT(T_true, rotTrasl); //deg, mm
	  initState = ConvertStateToAsnState(rotTrasl);
	  dfpc->initInput(initState);
				
	  }


	////// Input: image time (image timestamp) ////////
	time_images = (double) imageIndex*dt_images;
	imageAcquisitionTime.microseconds = time_images*1000000;
	dfpc->imageTimeInput(imageAcquisitionTime);

	     		
	if(imageIndex==0)
	{
	//useInitialGuess = true;
	dfpc->doInitInput(true);
	}
	
	if(imageIndex>0)
	 //useInitialGuess = false;
	 dfpc->doInitInput(false);
		
	clock_t startTime = clock();

	dfpc->run();
	
	clock_t endTime = clock();
	processingTime = float(endTime - startTime)*1000 / CLOCKS_PER_SEC;

	outputSuccess = dfpc->successOutput();
	if(!outputSuccess)
	 {
	    std::cout << "########## TRACKING: TARGET LOST " << outputSuccess << std::endl;
	    
	   // dfpc->doInitInput(true);	
	 }		
	else
	{
	 if(imageIndex>0) 
	 {
	  //memcpy(T_guess0,T_est,16*sizeof(double));
	  //memcpy(vel0,vel_est,6*sizeof(double));
	  dfpc->initInput(dfpc->stateOutput()); 
	  //time0 = time_images;
	  dfpc->initTimeInput(imageAcquisitionTime); 
	 }
         std::cout << "########## TRACKING: SUCCESS " << std::endl;
	}

	//----output--
	outputPose = dfpc->stateOutput();
	if(!outputSuccess)
		setState(outputPose, 0.0);
	//Log poses to file
	writer.open(outputPoseFilePath.c_str(), std::ios::out | std::ios::app);
	if(logGroundTruthError)
	  SaveOutputPose(writer, T_true);
	else
	 SaveOutputPose(writer);
	writer.close();
	successCounter = (outputSuccess ? successCounter+1 : successCounter);
	std::cout<<"Processing time (ms): "<<processingTime<<std::endl;
       }
	std::cout<<" The tracking was successful for the given image sequence: "<<successCounter<<std::endl;
	dfpcExecuted = true;
	
    }

void EdgeTrackerExecutor::SaveOutputPose(std::ofstream& writer, double* T_guess0)
	{
			
	double rotTrasl[6];
	double velocity[6];
      

	ConvertAsnStateToState(outputPose, rotTrasl,velocity);
	 
    	// Log to print output
	std::cout<<" output states: [ rx  ry  rz  tx  ty  tz  wx  wy  wz  vx  vy  vz] \n";
	 for (int i = 0;i<6;i++)
	   std::cout<<rotTrasl[i]<<" ";
	 for (int i = 0;i<6;i++)
	   std::cout<<velocity[i]<<" ";
	 
	  std::cout<<std::endl;

	// Log groundtruth pose erorrs (Orientation [radian])  
     	if(logGroundTruthError)
     	{
	double R_true[9];
	double t_true[3];
	rotTranslFromT(T_guess0,  R_true, t_true);
	double R_est[9];
	double t_est[3];
	double r_est[3];
	for(int i = 0;i < 3; i++)
	{
	 r_est[i] = rotTrasl[i]*M_PI/180.0;
	 t_est[i] = rotTrasl[i+3];
	}

	matrixRvecToRmat(r_est, R_est);
	
	double R_estT[9];
	matrixTranspose( R_est, R_estT, 3,3);
	double dR[9];
	matrixProduct333(R_true,  R_estT, dR);
	double drho[3];
	matrixRmatToRvec(dR, drho); // [radian]
	double dtranslation[3]; 

	dtranslation[0] = t_true[0]-t_est[0];
	dtranslation[1] = t_true[1]-t_est[1];
	dtranslation[2] = t_true[2]-t_est[2];
	// Log to print output
	printMatrix(" ## Orientation error [radian] ",drho,3,1);
	printMatrix(" ## Position error [mm] ",dtranslation,3,1);
	// Log to file
	 for (int i=0;i<3;i++)
	   { 
	   	writer<<drho[i];
	    	writer<<" ";
	   }
	 for (int i=0;i<3;i++)
	   {
	    writer<< dtranslation[i];
	    writer<<" ";

	   }

	   writer<<" \n ";
     	 }
     // Log the estimated pose (orientation [deg]) to file
    	 else
    	 {
	 for (int i=0;i<6;i++)
	 {  
	  writer<<rotTrasl[i]<<" ";
	 }
	 for (int i=0;i<6;i++)
	 {  
	   writer<<velocity[i]<<" ";
	 }
	 writer<<"\n";
         }	 

	}

/* --------------------------------------------------------------------------
 *
 * Private Member Functions
 *
 * --------------------------------------------------------------------------
 */
void EdgeTrackerExecutor::LoadInputImage(std::string filePath, FrameWrapper::FrameConstPtr& frame )
	{
	cv::Mat src_image = cv::imread(filePath, 0);
	cv::Mat image;
	filterMedian(src_image, image, 5);
	assert(src_image.cols > 0 && src_image.rows >0 && "Error: Loaded input image is empty");
	DELETE_IF_NOT_NULL(frame);
	frame= frameConverter.Convert(image);

	}

void EdgeTrackerExecutor::LoadInputPose(std::string filePath, double* T_gt)
	{
	std::ifstream pose;
	pose.open((char*)filePath.c_str(),std::ios::in);
	double val;
	for(int i=0;i<12;i++)
	{
	 pose>>val;
	 T_gt[i]= val;
	}

	T_gt[12]= 0;
	T_gt[13]= 0;
	T_gt[14]= 0;
	T_gt[15]= 1;

	pose.close();

	}

void EdgeTrackerExecutor::LoadInputPosesList()
	{
	std::stringstream posesListFilePath;
	posesListFilePath << inputPosesFolder << "/" << inputPosesListFileName;
	std::ifstream posesListFile(posesListFilePath.str().c_str());
	assert(posesListFile.good() && "Error it was not possible to open the poses list file");
	std::string line;
	std::getline(posesListFile, line);
	std::getline(posesListFile, line);
	std::getline(posesListFile, line);
	while (std::getline(posesListFile, line))
	{
	 std::vector<std::string> stringsList;
	 boost::split(stringsList, line, boost::is_any_of(" "));
	if(line.size()>0)
	{
	 poseFileNamesList.push_back( std::string(stringsList.at(0)) );
			
	}
	else
	  break;

	}


	posesListFile.close();

	}


void EdgeTrackerExecutor::LoadInputImagesList()
	{
	std::stringstream imagesListFilePath;
	imagesListFilePath << inputImagesFolder << "/" << inputImagesListFileName;
	std::ifstream imagesListFile(imagesListFilePath.str().c_str());
	assert(imagesListFile.good() && "Error it was not possible to open the images list file");
	std::string line;
	std::getline(imagesListFile, line);
	std::getline(imagesListFile, line);
	std::getline(imagesListFile, line);
	while (std::getline(imagesListFile, line))
	{
		
	std::vector<std::string> stringsList;
	boost::split(stringsList, line, boost::is_any_of(" "));
		
	if(line.size()>0)
	{
	 leftImageFileNamesList.push_back( std::string(stringsList.at(1)) );
	 rightImageFileNamesList.push_back( std::string(stringsList.at(2)) );

	}
	else
	  break;

	}

	imagesListFile.close();

	}

void EdgeTrackerExecutor::ConfigureDfpc()
	{
	dfpc->setConfigurationFile(configurationFilePath);
	dfpc->setup();
	}

void EdgeTrackerExecutor::filterMedian(cv::Mat& image, cv::Mat& filteredImage, int aperture_size)
	{
	 
	 cv::medianBlur(image, filteredImage, aperture_size);
	
	}

inline bool EdgeTrackerExecutor::isFileExist(const std::string& name)
	{
        std::ifstream infile(name.c_str());
        return infile.good();
	}
void EdgeTrackerExecutor::setState(asn1SccRigidBodyState& state, const double value)
	{
	 state.orient.arr[0] = value;
	 state.orient.arr[1] = value;
 	 state.orient.arr[2] = value;

	 state.pos.arr[0] = value;
	 state.pos.arr[1] = value;
 	 state.pos.arr[2] = value;

	 state.angular_velocity.arr[0] = value;
	 state.angular_velocity.arr[1] = value;
 	 state.angular_velocity.arr[2] = value;
	 state.velocity.arr[0] = value; 
	 state.velocity.arr[1] = value; 
	 state.velocity.arr[2] = value; 
	
	}

void EdgeTrackerExecutor::ConvertAsnStateToState(asn1SccRigidBodyState& state, double* pose, double* velocity)
	{
	 
	 pose[0] = state.orient.arr[0];
	 pose[1] = state.orient.arr[1];
 	 pose[2] = state.orient.arr[2];

	 pose[3] = state.pos.arr[0];
	 pose[4] = state.pos.arr[1];
 	 pose[5] = state.pos.arr[2];

	 if(velocity != NULL)
	 {
	  velocity[0] = state.angular_velocity.arr[0];
	  velocity[1] = state.angular_velocity.arr[1];
 	  velocity[2] = state.angular_velocity.arr[2];
	  velocity[3] = state.velocity.arr[0]; 
	  velocity[4] = state.velocity.arr[1]; 
	  velocity[5] = state.velocity.arr[2]; 
	 }
	
	
	}

asn1SccRigidBodyState EdgeTrackerExecutor::ConvertStateToAsnState(double* pose, double* velocity)
	{
	asn1SccRigidBodyState asnState; 
	asnState.orient.arr[0] = pose[0];
	asnState.orient.arr[1] = pose[1];
 	asnState.orient.arr[2] = pose[2];

	asnState.pos.arr[0] = pose[3];
	asnState.pos.arr[1] = pose[4];
 	asnState.pos.arr[2] = pose[5];

	if(velocity != NULL)
	{
	  asnState.angular_velocity.arr[0] = velocity[0];
	  asnState.angular_velocity.arr[1] = velocity[1];
 	  asnState.angular_velocity.arr[2] = velocity[2];

	  asnState.velocity.arr[0] = velocity[3];
	  asnState.velocity.arr[1] = velocity[4];
 	  asnState.velocity.arr[2] = velocity[5];
	}
	 

	return asnState ;

	}


