

#include "CAIR.h"
#include "CAIR_CML.h"
#include <cmath> //for abs(), floor()
#include <limits> //for max int
#include <pthread.h>
#include <semaphore.h>
#include "../tools.h"
#include <cfloat>

using namespace std;

//=========================================================================================================//
//an image processing element
struct CML_element
{
	CML_RGBA image; //standard image pixel value
	int edge;       //the edge value of the pixel
	int weight;     //its associated weight
	int energy;     //the calculated energy for this pixel
	CML_byte gray;  //its grayscale value
	bool removed;   //flag telling me if the pixel was removed during a resize
};
//an image being processed
typedef CML_Matrix<CML_element> CML_image;
//ok, now we have a single structure of all the info for a give pixel that we'll need.
//now, we create a matrix of pointers. when we remove a seam, shift this matrix. access all elements through this matrix.
//first, though, you'll need to fill in a CML_image, then set all the pointers in a CML_image_ptr. After the resizes are done,
//you'll need to pull the image and weights back out. See Init_CML_Image() and Extract_CML_Image()
typedef CML_Matrix<CML_element *> CML_image_ptr;

//=========================================================================================================//
//Thread parameters
struct Thread_Params
{
	//Image Parameters
	CML_image_ptr * Source;
	CAIR_convolution conv;
	CAIR_energy ener;
	//Internal Stuff
	int * Path;
	CML_image * Add_Resize;
	CML_image * Add_Source;
	//Thread Parameters
	int top_y;
	int bot_y;
	int top_x;
	int bot_x;
	bool exit; //flag causing the thread to exit
	int thread_num;
};

//=========================================================================================================//
//Thread Info
Thread_Params * thread_info;

//Thread Handles
pthread_t * remove_threads;
pthread_t * edge_threads;
pthread_t * gray_threads;
pthread_t * add_threads;
int num_threads = CAIR_NUM_THREADS;

//Thread Semaphores
sem_t remove_sem[3]; //start, edge_start, finish
sem_t add_sem[3]; //start, build_start, finish
sem_t edge_sem[2]; //start, finish
sem_t gray_sem[2]; //start, finish

//early declarations on the threading functions
void Startup_Threads();
void Shutdown_Threads();


//=========================================================================================================//
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

//=========================================================================================================//
//==                                          G R A Y S C A L E                                          ==//
//=========================================================================================================//

//=========================================================================================================//
//Performs a RGB->YUV type conversion (we only want Y', the luma)
inline CML_byte Grayscale_Pixel( CML_RGBA * pixel )
{
	return (CML_byte)floor( ( 299 * pixel->red +
							  587 * pixel->green +
							  114 * pixel->blue ) / 1000.0 );
}

//=========================================================================================================//
//Our thread function for the Grayscale
void * Gray_Quadrant( void * id )
{
	int num = *((int *)id);

	while( true )
	{
		//wait for the thread to get a signal to start
		sem_wait( &(gray_sem[0]) );

		//get updated parameters
		Thread_Params gray_area = thread_info[num];

		if( gray_area.exit == true )
		{
			//thread is exiting
			break;
		}

		int width = (*(gray_area.Source)).Width();
		for( int y = gray_area.top_y; y < gray_area.bot_y; y++ )
		{
			for( int x = 0; x < width; x++ )
			{
				(*(gray_area.Source))(x,y)->gray = Grayscale_Pixel( &(*(gray_area.Source))(x,y)->image );
			}
		}

		//signal we're done
		sem_post( &(gray_sem[1]) );
	}

	return NULL;
} //end Gray_Quadrant()

//=========================================================================================================//
//Sort-of does a RGB->YUV conversion (actually, just RGB->Y)
//Multi-threaded with each thread getting a strip across the image.
void Grayscale_Image( CML_image_ptr * Source )
{
	int thread_height = (*Source).Height() / num_threads;

	//setup parameters
	for( int i = 0; i < num_threads; i++ )
	{
		thread_info[i].Source = Source;
		thread_info[i].top_y = i * thread_height;
		thread_info[i].bot_y = thread_info[i].top_y + thread_height;
	}

	//have the last thread pick up the slack
	thread_info[num_threads-1].bot_y = (*Source).Height();

	//startup the threads
	for( int i = 0; i < num_threads; i++ )
	{
		sem_post( &(gray_sem[0]) );
	}

	//now wait for them to come back to us
	for( int i = 0; i < num_threads; i++ )
	{
		sem_wait( &(gray_sem[1]) );
	}

} //end Grayscale_Image()

//=========================================================================================================//
//==                                                 E D G E                                             ==//
//=========================================================================================================//
enum edge_safe { SAFE, UNSAFE };

//=========================================================================================================//
//returns the convolution value of the pixel Source[x][y] with one of the kernels.
//Several kernels are avaialable, each with their strengths and weaknesses. The edge_safe
//param will use the slower, but safer Get() method of the CML.
int Convolve_Pixel( CML_image_ptr * Source, int x, int y, edge_safe safety, CAIR_convolution convolution)
{
	int conv = 0;

	switch( convolution )
	{
	case PREWITT:
		if( safety == SAFE )
		{
			conv = abs( (*Source).Get(x+1,y+1)->gray + (*Source).Get(x+1,y)->gray + (*Source).Get(x+1,y-1)->gray //x part of the prewitt
					   -(*Source).Get(x-1,y-1)->gray - (*Source).Get(x-1,y)->gray - (*Source).Get(x-1,y+1)->gray ) +
				   abs( (*Source).Get(x+1,y+1)->gray + (*Source).Get(x,y+1)->gray + (*Source).Get(x-1,y+1)->gray //y part of the prewitt
					   -(*Source).Get(x+1,y-1)->gray - (*Source).Get(x,y-1)->gray - (*Source).Get(x-1,y-1)->gray );
		}
		else
		{
			conv = abs( (*Source)(x+1,y+1)->gray + (*Source)(x+1,y)->gray + (*Source)(x+1,y-1)->gray //x part of the prewitt
					   -(*Source)(x-1,y-1)->gray - (*Source)(x-1,y)->gray - (*Source)(x-1,y+1)->gray ) +
				   abs( (*Source)(x+1,y+1)->gray + (*Source)(x,y+1)->gray + (*Source)(x-1,y+1)->gray //y part of the prewitt
					   -(*Source)(x+1,y-1)->gray - (*Source)(x,y-1)->gray - (*Source)(x-1,y-1)->gray );
		}
		break;

	 case V_SQUARE:
		if( safety == SAFE )
		{
			conv = (*Source).Get(x+1,y+1)->gray + (*Source).Get(x+1,y)->gray + (*Source).Get(x+1,y-1)->gray //x part of the prewitt
				  -(*Source).Get(x-1,y-1)->gray - (*Source).Get(x-1,y)->gray - (*Source).Get(x-1,y+1)->gray;
			conv *= conv;
		}
		else
		{
			conv = (*Source)(x+1,y+1)->gray + (*Source)(x+1,y)->gray + (*Source)(x+1,y-1)->gray //x part of the prewitt
				  -(*Source)(x-1,y-1)->gray - (*Source)(x-1,y)->gray - (*Source)(x-1,y+1)->gray;
			conv *= conv;
		}
		break;

	 case V1:
		if( safety == SAFE )
		{
			conv =  abs( (*Source).Get(x+1,y+1)->gray + (*Source).Get(x+1,y)->gray + (*Source).Get(x+1,y-1)->gray //x part of the prewitt
						-(*Source).Get(x-1,y-1)->gray - (*Source).Get(x-1,y)->gray - (*Source).Get(x-1,y+1)->gray );
		}
		else
		{
			conv = abs( (*Source)(x+1,y+1)->gray + (*Source)(x+1,y)->gray + (*Source)(x+1,y-1)->gray //x part of the prewitt
					   -(*Source)(x-1,y-1)->gray - (*Source)(x-1,y)->gray - (*Source)(x-1,y+1)->gray ) ;
		}
		break;
	
	 case SOBEL:
		if( safety == SAFE )
		{
			conv = abs( (*Source).Get(x+1,y+1)->gray + (2 * (*Source).Get(x+1,y)->gray) + (*Source).Get(x+1,y-1)->gray //x part of the sobel
					   -(*Source).Get(x-1,y-1)->gray - (2 * (*Source).Get(x-1,y)->gray) - (*Source).Get(x-1,y+1)->gray ) +
				   abs( (*Source).Get(x+1,y+1)->gray + (2 * (*Source).Get(x,y+1)->gray) + (*Source).Get(x-1,y+1)->gray //y part of the sobel
					   -(*Source).Get(x+1,y-1)->gray - (2 * (*Source).Get(x,y-1)->gray) - (*Source).Get(x-1,y-1)->gray );
		}
		else
		{
			conv = abs( (*Source)(x+1,y+1)->gray + (2 * (*Source)(x+1,y)->gray) + (*Source)(x+1,y-1)->gray //x part of the sobel
					   -(*Source)(x-1,y-1)->gray - (2 * (*Source)(x-1,y)->gray) - (*Source)(x-1,y+1)->gray ) +
				   abs( (*Source)(x+1,y+1)->gray + (2 * (*Source)(x,y+1)->gray) + (*Source)(x-1,y+1)->gray //y part of the sobel
					   -(*Source)(x+1,y-1)->gray - (2 * (*Source)(x,y-1)->gray) - (*Source)(x-1,y-1)->gray );
		}
		break;

	case LAPLACIAN:
		if( safety == SAFE )
		{
			conv = abs( (*Source).Get(x+1,y)->gray + (*Source).Get(x-1,y)->gray + (*Source).Get(x,y+1)->gray + (*Source).Get(x,y-1)->gray
					   -(4 * (*Source).Get(x,y)->gray) );
		}
		else
		{
			conv = abs( (*Source)(x+1,y)->gray + (*Source)(x-1,y)->gray + (*Source)(x,y+1)->gray + (*Source)(x,y-1)->gray
					   -(4 * (*Source)(x,y)->gray) );
		}
		break;
	}
	return conv;
}

//=========================================================================================================//
//The thread function, splitting the image into strips
void * Edge_Quadrant( void * id )
{
	int num = *((int *)id);

	while( true )
	{
		sem_wait( &(edge_sem[0]) );

		//get updated parameters
		Thread_Params edge_area = thread_info[num];

		if( edge_area.exit == true )
		{
			//thread is exiting
			break;
		}

		for( int y = edge_area.top_y; y < edge_area.bot_y; y++ )
		{
			//left most edge
			(*(edge_area.Source))(0,y)->edge = Convolve_Pixel( edge_area.Source, 0, y, SAFE, edge_area.conv );

			//fill in the middle
			int width = (*(edge_area.Source)).Width();
			for( int x = 1; x < width - 1; x++ )
			{
				(*(edge_area.Source))(x,y)->edge = Convolve_Pixel( edge_area.Source, x, y, UNSAFE, edge_area.conv );
			}

			//right most edge
			(*(edge_area.Source))(width-1,y)->edge = Convolve_Pixel( edge_area.Source, width-1, y, SAFE, edge_area.conv);
		}

		//signal we're done
		sem_post( &(edge_sem[1]) );
	}

	return NULL;
}

//=========================================================================================================//
//Performs full edge detection on Source with one of the kernels.
void Edge_Detect( CML_image_ptr * Source, CAIR_convolution conv )
{
	//There is no easy solution to the boundries. Calling the same boundry pixel to convolve itself against seems actually better
	//than padding the image with zeros or 255's.
	//Calling itself induces a "ringing" into the near edge of the image. Padding can lead to a darker or lighter edge.
	//The only "good" solution is to have the entire one-pixel wide edge not included in the edge detected image.
	//This would reduce the size of the image by 2 pixels in both directions, something that is unacceptable here.

	int thread_height = (*Source).Height() / num_threads;
	int height = (*Source).Height();
	int width = (*Source).Width();

	//setup parameters
	for( int i = 0; i < num_threads; i++ )
	{
		thread_info[i].Source = Source;
		thread_info[i].top_y = (i * thread_height) + 1; //handle very top row down below
		thread_info[i].bot_y = thread_info[i].top_y + thread_height;
		thread_info[i].conv = conv;
	}

	//have the last thread pick up the slack
	thread_info[num_threads-1].bot_y = height - 1; //handle very bottom row down below

	//create the threads
	for( int i = 0; i < num_threads; i++ )
	{
		sem_post( &(edge_sem[0]) );
	}

	//while those are running we can go back and do the boundry pixels with the extra safety checks
	for( int x = 0; x < width; x++ )
	{
		(*Source)(x,0)->edge = Convolve_Pixel( Source, x, 0, SAFE, conv );
		(*Source)(x,height-1)->edge = Convolve_Pixel( Source, x, height-1, SAFE, conv );
	}

	//now wait on them
	for( int i = 0; i < num_threads; i++ )
	{
		sem_wait( &(edge_sem[1]) );
	}

} //end Edge_Detect()

//=========================================================================================================//
//==                                           E N E R G Y                                               ==//
//=========================================================================================================//

//=========================================================================================================//
//Simple fuction returning the minimum of three values.
//The obvious MIN(MIN(x,y),z) is actually undesirable, since that would guarantee three branch checks.
//This one here sucks up a variable to guarantee only two branches.
inline int min_of_three( int x, int y, int z )
{
	int min = y;

	if( x < min )
	{
		min = x;
	}
	if( z < min )
	{
		return z;
	}

	return min;
}

//=========================================================================================================//
//Get the value from the integer matrix, return a large value if out-of-bounds in the x-direction.
inline int Get_Max( CML_image_ptr * Energy, int x, int y )
{
	if( ( x < 0 ) || ( x >= (*Energy).Width() ) )
	{
		return std::numeric_limits<int>::max();
	}
	else
	{
		return (*Energy)(x,y)->energy;
	}
}

//=========================================================================================================//
//This calculates a minimum energy path from the given start point (min_x) and the energy map.
void Generate_Path( CML_image_ptr * Energy, int min_x, int * Path )
{
	int min;
	int x = min_x;

	int height = (*Energy).Height();
	for( int y = height - 1; y >= 0; y-- ) //builds from bottom up
	{
		min = x; //assume the minimum is straight up

		if( Get_Max( Energy, x-1, y ) < Get_Max( Energy, min, y ) ) //check to see if min is up-left
		{
			min = x - 1;
		}
		if( Get_Max( Energy, x+1, y ) < Get_Max( Energy, min, y) ) //up-right
		{
			min = x + 1;
		}
		
		Path[y] = min;
		x = min;
	}
}

//=========================================================================================================//
//Forward energy cost functions. These are additional energy values for the left, up, and right seam paths.
//See the paper "Improved Seam Carving for Video Retargeting" by Michael Rubinstein, Ariel Shamir, and Shai  Avidan.
inline int Forward_CostL( CML_image_ptr * Edge, int x, int y )
{
	return (abs((*Edge)(x+1,y)->edge - (*Edge)(x-1,y)->edge) + abs((*Edge)(x,y-1)->edge - (*Edge)(x-1,y)->edge));
}

inline int Forward_CostU( CML_image_ptr * Edge, int x, int y )
{
	return (abs((*Edge)(x+1,y)->edge - (*Edge)(x-1,y)->edge));
}

inline int Forward_CostR( CML_image_ptr * Edge, int x, int y )
{
	return (abs((*Edge)(x+1,y)->edge - (*Edge)(x-1,y)->edge) + abs((*Edge)(x,y-1)->edge - (*Edge)(x+1,y)->edge));
}

//=========================================================================================================//
//Calculate the energy map of the image using the edges and weights. When Path is set to NULL, the energy map
//will be completely recalculated, otherwise if it contains a valid seam, it will use it to only update changed
//portions of the energy map.
void Energy_Map(CML_image_ptr * Source, CAIR_energy ener, int * Path)
{
	int min_x, max_x;
	int min_x_energy, max_x_energy;
	int boundry_min_x, boundry_max_x;
	int height = (*Source).Height()-1;
	int width = (*Source).Width()-1;

	if(Path == NULL)
	{
		//calculate full region
		min_x = 0;
		max_x = width;
	}
	else
	{
		//restrict calculation tree based on path location
		min_x = MAX(Path[0]-3, 0);
		max_x = MIN(Path[0]+2, width);
	}

	//set the first row with the correct energy
	for(int x = min_x; x <= max_x; x++)
	{
		(*Source)(x,0)->energy = (*Source)(x,0)->edge + (*Source)(x,0)->weight;
	}

	for(int y = 1; y <= height; y++)
	{
		//each itteration we expand the width of calculations, one in each direction
		min_x = MAX(min_x-1, 0);
		max_x = MIN(max_x+1, width);
		boundry_min_x = 0;
		boundry_max_x = 0;

		//boundry conditions
		if(max_x == width)
		{
			(*Source)(width,y)->energy = MIN((*Source)(width-1,y-1)->energy, (*Source)(width,y-1)->energy) + (*Source)(width,y)->edge + (*Source)(width,y)->weight;
			boundry_max_x = 1; //prevent this value from being calculated in the below loops
		}
		if(min_x == 0)
		{
			(*Source)(0,y)->energy = MIN((*Source)(0,y-1)->energy, (*Source)(1,y-1)->energy) + (*Source)(0,y)->edge + (*Source)(0,y)->weight;
			boundry_min_x = 1;
		}

		//store the previous max/min energies, use these to see if we can trim the tree
		min_x_energy = (*Source)(min_x,y)->energy;
		max_x_energy = (*Source)(max_x,y)->energy;

		//fill in everything besides the boundries, if needed
		if(ener == BACKWARD)
		{
			for(int x = (min_x + boundry_min_x); x <= (max_x - boundry_max_x); x++)
			{
				(*Source)(x,y)->energy = min_of_three((*Source)(x-1,y-1)->energy,
													  (*Source)(x,y-1)->energy,
													  (*Source)(x+1,y-1)->energy)
										 + (*Source)(x,y)->edge + (*Source)(x,y)->weight;
			}
		}
		else //forward energy
		{
			for(int x = (min_x + boundry_min_x); x <= (max_x - boundry_max_x); x++)
			{
				(*Source)(x,y)->energy = min_of_three((*Source)(x-1,y-1)->energy + Forward_CostL(Source,x,y),
													  (*Source)(x,y-1)->energy + Forward_CostU(Source,x,y),
													  (*Source)(x+1,y-1)->energy + Forward_CostR(Source,x,y))
										 + (*Source)(x,y)->weight;
			}
		}

		//check to see if we can restrict future calculations
		if(Path != NULL)
		{
			if((Path[y] > min_x+3) && ((*Source)(min_x,y)->energy == min_x_energy)) min_x++;
			if((Path[y] < max_x-2) && ((*Source)(max_x,y)->energy == max_x_energy)) max_x--;
		}
	}
}


//=========================================================================================================//
//Energy_Path() generates the least energy Path of the Edge and Weights and returns the total energy of that path.
int Energy_Path( CML_image_ptr * Source, int * Path, CAIR_energy ener, bool first_time )
{
	//calculate the energy map
	if( first_time == true )
	{
		Energy_Map( Source, ener, NULL );
	}
	else
	{
		Energy_Map( Source, ener, Path );
	}

	//find minimum path start
	int min_x = 0;
	int width = (*Source).Width();
	int height = (*Source).Height();
	for( int x = 0; x < width; x++ )
	{
		if( (*Source)(x,height-1)->energy < (*Source)(min_x,height-1)->energy )
		{
			min_x = x;
		}
	}

	//generate the path back from the energy map
	Generate_Path( Source, min_x, Path );
	return (*Source)(min_x,height-1)->energy;
}


//=========================================================================================================//
//==                                                 A D D                                               ==//
//=========================================================================================================//

//=========================================================================================================//
//averages two pixels and returns the values
CML_RGBA Average_Pixels( CML_RGBA Pixel1, CML_RGBA Pixel2 )
{
	CML_RGBA average;

	average.alpha = ( Pixel1.alpha + Pixel2.alpha ) / 2;
	average.blue = ( Pixel1.blue + Pixel2.blue ) / 2;
	average.green = ( Pixel1.green + Pixel2.green ) / 2;
	average.red = ( Pixel1.red + Pixel2.red ) / 2;

	return average;
}

//=========================================================================================================//
//This works like Remove_Quadrant, strips across the image.
void * Add_Quadrant( void * id )
{
	int num = *((int *)id);
	Thread_Params add_area;

	while( true )
	{
		sem_wait( &(add_sem[0]) );

		//get updated_parameters
		add_area = thread_info[num];

		if( add_area.exit == true )
		{
			//thread is exiting
			break;
		}

		//restore the image and weights from the source, we only care about the removed flags
		int width = (*(add_area.Add_Resize)).Width();
		for(int y = add_area.top_y; y < add_area.bot_y; y++)
		{
			for(int x = 0; x < width; x++)
			{
				(*(add_area.Add_Resize))(x,y).image = (*(add_area.Source))(x,y)->image;
				(*(add_area.Add_Resize))(x,y).weight = (*(add_area.Source))(x,y)->weight;
			}
		}

		//signal that part is done
		sem_post( &(add_sem[2]) );

		//wait to begin the adding
		sem_wait( &(add_sem[1]) );

		//get updated_parameters
		add_area = thread_info[num];

		//now we can actually enlarge the image, inserting pixels near removed ones
		width = (*(add_area.Add_Resize)).Width();
		for(int y = add_area.top_y; y < add_area.bot_y; y++)
		{
			int add_column = 0;
			for(int x = 0; x < width; x++)
			{
				//copy over the pixel, setting the pointer, and incrimenting the large image to the next column
				(*(add_area.Add_Source))(add_column,y).image = (*(add_area.Add_Resize))(x,y).image;
				(*(add_area.Add_Source))(add_column,y).weight = (*(add_area.Add_Resize))(x,y).weight;
				(*(add_area.Source))(add_column,y) = &(*(add_area.Add_Source))(add_column,y);
				add_column++;

				if((*(add_area.Add_Resize))(x,y).removed == true)
				{
					//insert a new pixel, taking the average of the current pixel and the next pixel
					(*(add_area.Add_Source))(add_column,y).image = Average_Pixels( (*(add_area.Add_Resize))(x,y).image, (*(add_area.Add_Resize))(MIN(x+1,width-1),y).image );
					(*(add_area.Add_Source))(add_column,y).weight = ((*(add_area.Add_Resize))(x,y).weight + (*(add_area.Add_Resize))(MIN(x+1,width-1),y).weight) / 2;
					(*(add_area.Source))(add_column,y) = &(*(add_area.Add_Source))(add_column,y);
					add_column++;
				}
			}
		}

		//signal the add thread is done
		sem_post( &(add_sem[2]) );

	} //end while(true)
	return NULL;
}


//=========================================================================================================//
//Grab the current image data from Source and put it back into Resize_img. Then create the enlarged image
//by pulling the image and remove info from Resize_img and creating our new Source. Also, update Source_ptr
//while we're at it.
void Add_Path( CML_image * Resize_img, CML_image * Source, CML_image_ptr * Source_ptr, int goal_x )
{
	int height = (*Resize_img).Height();
	int thread_height = height / num_threads;

	//setup parameters
	for( int i = 0; i < num_threads; i++ )
	{
		thread_info[i].Source = Source_ptr;
		thread_info[i].Add_Source = Source;
		thread_info[i].Add_Resize = Resize_img;
		thread_info[i].top_y = i * thread_height;
		thread_info[i].bot_y = thread_info[i].top_y + thread_height;
	}

	//have the last thread pick up the slack
	thread_info[num_threads-1].bot_y = height;

	//startup the threads
	for( int i = 0; i < num_threads; i++ )
	{
		sem_post( &(add_sem[0]) );
	}

	//now wait for them to come back to us
	for( int i = 0; i < num_threads; i++ )
	{
		sem_wait( &(add_sem[2]) );
	}

	//ok, we can now resize the source to the final size
	(*Source).D_Resize(goal_x, height);
	(*Source_ptr).D_Resize(goal_x, height);

	for( int i = 0; i < num_threads; i++ )
	{
		sem_post( &(add_sem[1]) );
	}

	//now wait on them again
	for( int i = 0; i < num_threads; i++ )
	{
		sem_wait( &(add_sem[2]) );
	}

} //end Add_Path()

//forward delcration
bool CAIR_Remove( CML_image_ptr * Source, int goal_x, CAIR_convolution conv, CAIR_energy ener, bool (*CAIR_callback)(float), int total_seams, int seams_done );

//=========================================================================================================//
//Enlarge Source (and Source_ptr) to the width specified in goal_x. This is accomplished by remove the number of seams
//that are to be added, and recording what pixels were removed. We then add a new pixel next to the origional.
bool CAIR_Add( CML_image * Source, CML_image_ptr * Source_ptr, int goal_x, CAIR_convolution conv, CAIR_energy ener, bool (*CAIR_callback)(float), int total_seams, int seams_done )
{
	//create local copies of the actual source image and its set of pointers
	//we will resize this image down the number of adds in order to determine which pixels were removed
	CML_image Resize_img((*Source_ptr).Width(),(*Source_ptr).Height());
	CML_image_ptr Resize_img_ptr((*Source_ptr).Width(),(*Source_ptr).Height());
	for(int y = 0; y < (*Source_ptr).Height(); y++)
	{
		for(int x = 0; x < (*Source_ptr).Width(); x++)
		{
			Resize_img(x,y).image = (*Source_ptr)(x,y)->image;
			Resize_img(x,y).weight = (*Source_ptr)(x,y)->weight;
			Resize_img(x,y).removed = false;
			Resize_img_ptr(x,y) = &(Resize_img(x,y));
		}
	}

	//remove all the least energy seams, setting the "removed" flag for each element
	if(CAIR_Remove(&Resize_img_ptr, (*Source_ptr).Width() - (goal_x - (*Source_ptr).Width()), conv, ener, CAIR_callback, total_seams, seams_done) == false)
	{
		return false;
	}

	//enlarge the image now that we have our seam data
	Add_Path(&Resize_img, Source, Source_ptr, goal_x);
	return true;
} //end CAIR_Add()




//=========================================================================================================//
//==                                             R E M O V E                                             ==//
//=========================================================================================================//

//=========================================================================================================//
//more multi-threaded goodness
//the areas are not quadrants, rather, more like strips, but I keep the name convention
void * Remove_Quadrant( void * id )
{
	int num = *((int *)id);
	Thread_Params remove_area;

	while( true )
	{
		sem_wait( &(remove_sem[0]) );

		//get updated parameters
		remove_area = thread_info[num];

		if( remove_area.exit == true )
		{
			//thread is exiting
			break;
		}

		//remove
		for( int y = remove_area.top_y; y < remove_area.bot_y; y++ )
		{
			//reduce each row by one, the removed pixel
			int remove = (remove_area.Path)[y];
			(*(remove_area.Source))(remove,y)->removed = true;

			//now, bounds check the assignments
			if( (remove - 1) > 0 )
			{
				if( (*(remove_area.Source))(remove,y)->weight >= 0 ) //otherwise area marked for removal, don't blend
				{
					//average removed pixel back in
					(*(remove_area.Source))(remove-1,y)->image = Average_Pixels( (*(remove_area.Source))(remove,y)->image,
																				 (*(remove_area.Source)).Get(remove-1,y)->image );
				}
				(*(remove_area.Source))(remove-1,y)->gray = Grayscale_Pixel( &(*(remove_area.Source))(remove-1,y)->image );
			}

			if( (remove + 1) < (*(remove_area.Source)).Width() )
			{
				if( (*(remove_area.Source))(remove,y)->weight >= 0 ) //otherwise area marked for removal, don't blend
				{
					//average removed pixel back in
					(*(remove_area.Source))(remove+1,y)->image = Average_Pixels( (*(remove_area.Source))(remove,y)->image,
																				 (*(remove_area.Source)).Get(remove+1,y)->image );
				}
				(*(remove_area.Source))(remove+1,y)->gray = Grayscale_Pixel( &(*(remove_area.Source))(remove+1,y)->image );
			}

			//shift everyone over
			(*(remove_area.Source)).Shift_Row( remove + 1, y, -1 );
		}

		//signal that part is done
		sem_post( &(remove_sem[2]) );

		//wait to begin edge removal
		sem_wait( &(remove_sem[1]) );

		//get updated parameters
		remove_area = thread_info[num];

		//now update the edge values after the grayscale values have been corrected
		int width = (*(remove_area.Source)).Width();
		int height = (*(remove_area.Source)).Height();
		for(int y = remove_area.top_y; y < remove_area.bot_y; y++)
		{
			int remove = (remove_area.Path)[y];
			edge_safe safety = UNSAFE;

			//check to see if we might fall out of the image during a Convolve_Pixel() with a 3x3 kernel
			if( (y<=4) || (y>=height-5) || (remove<=4) || (remove>=width-5) )
			{
				safety = SAFE;
			}

			//rebuild the edges around the removed seam, assuming no larger than a 3x3 kernel was used
			//The grayscale for the current seam location (remove) and its neighbor to the left (remove-1) were directly changed when the
			//seam pixel was blended back into it. Therefore we need to update any edge value that could be affected. The kernels CAIR uses
			//are no more than 3x3 so we would need to update at least up to one pixel on either side of the changed grayscales. But, since
			//the seams can cut back into an area above or below the row we're currently on, other areas beyond our one pixel area could change.
			//Therefore we have to increase the number of edge values that are updated.
			for(int x = remove-3; x < remove+3; x++)
			{
				//safe/unsafe check above should make Convolve_Pixel() happy, but do a min/max check on the x to be sure it's happy
				(*(remove_area.Source))(MIN(MAX(x,0),width-1),y)->edge = Convolve_Pixel(remove_area.Source, MIN(MAX(x,0),width-1), y, safety, remove_area.conv);
			}
		}

		//signal we're now done
		sem_post( &(remove_sem[2]) );
	} //end while( true )

	return NULL;
} //end Remove_Quadrant()

//=========================================================================================================//
//Remove a seam from Source. Blend the seam's image and weight back into the Source. Update edges, grayscales,
//and set corresponding removed flags.
void Remove_Path( CML_image_ptr * Source, int * Path, CAIR_convolution conv )
{
	int thread_height = (*Source).Height() / num_threads;

	//setup parameters
	for( int i = 0; i < num_threads; i++ )
	{
		thread_info[i].Source = Source;
		thread_info[i].Path = Path;
		thread_info[i].conv = conv;
		thread_info[i].top_y = i * thread_height;
		thread_info[i].bot_y = thread_info[i].top_y + thread_height;
	}

	//have the last thread pick up the slack
	thread_info[num_threads-1].bot_y = (*Source).Height();

	//start the threads
	for( int i = 0; i < num_threads; i++ )
	{
		sem_post( &(remove_sem[0]) );
	}
	//now wait on them
	for( int i = 0; i < num_threads; i++ )
	{
		sem_wait( &(remove_sem[2]) );
	}

	//now we can safely resize everyone down
	(*Source).Resize_Width( (*Source).Width() - 1 );

	//now get the threads to handle the edge
	//we must wait for the grayscale to be complete before we can recalculate changed edge values
	for( int i = 0; i < num_threads; i++ )
	{
		sem_post( &(remove_sem[1]) );
	}

	//now wait on them, ... again
	for( int i = 0; i < num_threads; i++ )
	{
		sem_wait( &(remove_sem[2]) );
	}
} //end Remove_Path()

//=========================================================================================================//
//Removes all requested vertical paths form the image.
bool CAIR_Remove( CML_image_ptr * Source, int goal_x, CAIR_convolution conv, CAIR_energy ener, bool (*CAIR_callback)(float), int total_seams, int seams_done )
{
	int removes = (*Source).Width() - goal_x;
	int * Min_Path = new int[(*Source).Height()];

	//setup the images
	Grayscale_Image( Source );
	Edge_Detect( Source, conv );

	//remove each seam
	for( int i = 0; i < removes; i++ )
	{
		//If you're going to maintain some sort of progress counter/bar, here's where you would do it!
		if( (CAIR_callback != NULL) && (CAIR_callback( (float)(i+seams_done)/total_seams ) == false) )
		{
			delete[] Min_Path;
			return false;
		}

		//determine the least energy path
		if( i == 0 )
		{
			//first time through, build the energy map
			Energy_Path( Source, Min_Path, ener, true );
		}
		else
		{
			//next time through, only update the energy map from the last remove
			Energy_Path( Source, Min_Path, ener, false );
		}

		//remove the seam from the image, update grayscale and edge values
		Remove_Path( Source, Min_Path, conv );
	}

	delete[] Min_Path;
	return true;
} //end CAIR_Remove()

//=========================================================================================================//
//Startup all threads, create all needed semaphores.
void Startup_Threads()
{
	//create semaphores
	sem_init( &(remove_sem[0]), 0, 0 ); //start
	sem_init( &(remove_sem[1]), 0, 0 ); //edge_start
	sem_init( &(remove_sem[2]), 0, 0 ); //finish
	sem_init( &(add_sem[0]), 0, 0 ); //start
	sem_init( &(add_sem[1]), 0, 0 ); //build_start
	sem_init( &(add_sem[2]), 0, 0 ); //finish
	sem_init( &(edge_sem[0]), 0, 0 ); //start
	sem_init( &(edge_sem[1]), 0, 0 ); //finish
	sem_init( &(gray_sem[0]), 0, 0 ); //start
	sem_init( &(gray_sem[1]), 0, 0 ); //finish

	//create the thread handles
	remove_threads = new pthread_t[num_threads];
	edge_threads   = new pthread_t[num_threads];
	gray_threads   = new pthread_t[num_threads];
	add_threads    = new pthread_t[num_threads];

	thread_info = new Thread_Params[num_threads];

	//startup the threads
	for( int i = 0; i < num_threads; i++ )
	{
		thread_info[i].exit = false;
		thread_info[i].thread_num = i;

		pthread_create( &(remove_threads[i]), NULL, Remove_Quadrant, (void *)(&(thread_info[i].thread_num)) );
		pthread_create( &(edge_threads[i]), NULL, Edge_Quadrant, (void *)(&(thread_info[i].thread_num)) );
		pthread_create( &(gray_threads[i]), NULL, Gray_Quadrant, (void *)(&(thread_info[i].thread_num)) );
		pthread_create( &(add_threads[i]), NULL, Add_Quadrant, (void *)(&(thread_info[i].thread_num)) );
	}
}

//=========================================================================================================//
//Stops all threads. Deletes all semaphores and mutexes.
void Shutdown_Threads()
{
	//notify the threads
	for( int i = 0; i < num_threads; i++ )
	{
		thread_info[i].exit = true;
	}

	//start them up
	for( int i = 0; i < num_threads; i++ )
	{
		sem_post( &(remove_sem[0]) );
		sem_post( &(add_sem[0]) );
		sem_post( &(edge_sem[0]) );
		sem_post( &(gray_sem[0]) );
	}

	//wait for the joins
	for( int i = 0; i < num_threads; i++ )
	{
		pthread_join( remove_threads[i], NULL );
		pthread_join( edge_threads[i], NULL );
		pthread_join( gray_threads[i], NULL );
		pthread_join( add_threads[i], NULL );
	}

	//remove the thread handles
	delete[] remove_threads;
	delete[] edge_threads;
	delete[] gray_threads;
	delete[] add_threads;

	delete[] thread_info;

	//delete the semaphores
	sem_destroy( &(remove_sem[0]) ); //start
	sem_destroy( &(remove_sem[1]) ); //edge_start
	sem_destroy( &(remove_sem[2]) ); //finish
	sem_destroy( &(add_sem[0]) ); //start
	sem_destroy( &(add_sem[1]) ); //build_start
	sem_destroy( &(add_sem[2]) ); //finish
	sem_destroy( &(edge_sem[0]) ); //start
	sem_destroy( &(edge_sem[1]) ); //finish
	sem_destroy( &(gray_sem[0]) ); //start
	sem_destroy( &(gray_sem[1]) ); //finish
}


//=========================================================================================================//
//store the provided image and weights into a CML_image, and build a CML_image_ptr
void Init_CML_Image(CML_color * Source, CML_int * S_Weights, CML_image * Image, CML_image_ptr * Image_ptr)
{
	int x = (*Source).Width();
	int y = (*Source).Height(); //S_Weights should match

	(*Image).D_Resize(x,y);
	(*Image_ptr).D_Resize(x,y);

	for(int j = 0; j < y; j++)
	{
		for(int i = 0; i < x; i++)
		{
			(*Image)(i,j).image = (*Source)(i,j);
			(*Image)(i,j).weight = (*S_Weights)(i,j);

			(*Image_ptr)(i,j) = &((*Image)(i,j));
		}
	}
}

//=========================================================================================================//
//pull the resized image and weights back out of the CML_image through the resized CML_image_ptr
void Extract_CML_Image(CML_image_ptr * Image_ptr, CML_color * Dest, CML_int * D_Weights)
{
	int x = (*Image_ptr).Width();
	int y = (*Image_ptr).Height();

	(*Dest).D_Resize(x,y);
	(*D_Weights).D_Resize(x,y);

	for(int j = 0; j < y; j++)
	{
		for(int i = 0; i < x; i++)
		{
			(*Dest)(i,j) = (*Image_ptr)(i,j)->image;
			(*D_Weights)(i,j) = (*Image_ptr)(i,j)->weight;
		}
	}
}


//=========================================================================================================//
//Set the number of threads that CAIR should use. Minimum of 1 required.
//WARNING: Never call this function while CAIR() is processing an image, otherwise bad things will happen!
void CAIR_Threads( int thread_count )
{
	if( thread_count < 1 )
	{
		num_threads = 1;
	}
	else
	{
		num_threads = thread_count;
	}
}

template<class T>
bool isInVector(vector<T> v, T val){
    for(int i = 0; i<(int)v.size(); i++){
       // cerr << "v[i] : " << v[i] << ", val : " << val << endl;
        if(v[i] == val){
            return true;
        }
    }
    return false;
}

void printTree(Tree<Data>& t, int rang, vector<Tree<Data>*>& explored){
    for(int i = 0; i<t.getChildrenSize(); i++){
        if(!isInVector(explored,t[i])){
            cerr << "enfant " << rang << " branche " << i << " : " << t[i] << endl;
            explored.push_back(t[i]);
        }
        printTree(*(t[i]),rang+1, explored);
    }
}

int* findMinPath(CML_image_ptr * Source_ptr, CAIR_convolution conv, CAIR_energy ener){
//    cerr << "ENTREE ADD DATA" << endl;

    CML_image Resize_img((*Source_ptr).Width(),(*Source_ptr).Height());
    CML_image_ptr Resize_img_ptr((*Source_ptr).Width(),(*Source_ptr).Height());

    for(int y = 0; y < (*Source_ptr).Height(); y++)
    {
        for(int x = 0; x < (*Source_ptr).Width(); x++)
        {
            Resize_img(x,y).image = (*Source_ptr)(x,y)->image;
            Resize_img(x,y).weight = (*Source_ptr)(x,y)->weight;
            Resize_img(x,y).removed = false;
            Resize_img_ptr(x,y) = &(Resize_img(x,y));
        }
    }

    int * Min_Path = new int[Resize_img_ptr.Height()];
    Grayscale_Image( &Resize_img_ptr);
    Edge_Detect( &Resize_img_ptr, conv );

    Energy_Path( &Resize_img_ptr, Min_Path, ener, true );

    return Min_Path;
}

//prérequis segX[0] < segX[2]
double* intersection(Point seg1, Point seg2){
    /*
    cerr << "INTERSECTION ENTREE" << endl;
    cerr << "seg1 x1 " << seg1[0] << ", seg1 y1 " << seg1[1] << ", seg1 x2 " << seg1[2] << ", seg1 y2 " << seg1[3] << endl;
    cerr << "seg2 x1 " << seg2[0] << ", seg2 y1 " << seg2[1] << ", seg2 x2 " << seg2[2] << ", seg2 y2 " << seg2[3] << endl;
    */
    double a1;
    double b1;
    double resX = 0;
    double resY;
    double* res = new double[2];
    if(seg1[0] != seg1[2]){
        a1 = seg1[1]-seg1[3];
        a1 /= seg1[0]-seg1[2];
        b1 = seg1[1]-(a1*seg1[0]);
    }else{
        a1 = DBL_MAX;
        b1 = seg1[0];
        resX = seg1[0];
    }
    //cerr << "a1 " << a1 << ", b1 " << b1 << endl;
    double a2;
    double b2;
    if(seg2[0] != seg2[2]){
        a2 = seg2[1]-seg2[3];
        a2 /= seg2[0]-seg2[2];
        b2 = seg2[1]-(a2*seg2[0]);
    }else{
        a2 = DBL_MAX;
        b2 = seg2[0];
        resX = seg2[0];
    }
    //cerr << "a2 " << a2 << ", b2 " << b2 << endl;
    if(a2 == a1){
        //cerr << "INTERSECTION SORTIE NOT FIND: PARALLELE" << endl;
        res[0] = -1;
        res[1] = -1;
        return res;
    }
    //voir cas perpendiculaire (pente nulle et pente verticale)
    if(a1 != DBL_MAX && a2 != DBL_MAX){
        resX = b1-b2;
        resX /= a2-a1;
        resY = a1*resX+b1;
    }else{
        if(a1 == DBL_MAX){
            resY = a2*resX+b2;
        }else{
            resY = a1*resX+b1;
        }
    }
    double limMinX = MAX(MIN(seg1[0],seg1[2]),MIN(seg2[0],seg2[2]));
    double limMaxX = MIN(MAX(seg1[0],seg1[2]),MAX(seg2[0],seg2[2]));
    double limMinY = MAX(MIN(seg1[1],seg1[3]),MIN(seg2[1],seg2[3]));
    double limMaxY = MIN(MAX(seg1[1],seg1[3]),MAX(seg2[1],seg2[3]));
    if(resX <= limMaxX && resX >= limMinX && resY <= limMaxY && resY >= limMinY){
        //cerr << "INTERSECTION SORTIE FIND" << " resX " << resX << ", resY " << resY << endl;
        res[0] = resX;
        res[1] = resY;
        return res;
    }
    //cerr << "INTERSECTION SORTIE NOT FIND: HORS DES SEGS" << " resX " << resX << ", resY " << resY << endl;
    res[0] = -1;
    res[1] = -1;
    return res;
}

void findData(Tree<Data>* t, Tree<Data>* root, Point seg2, vector<double*>& interMatch, vector<int>& indexMatch){
    //cerr << "FIND DATA ENTREE" << endl;
    double* inter;
    Data* d = t->getValue();
//    if(d != NULL){
//        cerr << "seg2 x0 " << seg2[0] << " seg2 y0 " << seg2[1] << " seg2 x1 " << seg2[2] << " seg2 y1 " << seg2[3] << endl;
//        cerr << "x min " << d->xMin() << " x max " << d->xMax() << " y min " << d->yMin() << " y max " << d->yMax() << endl;
//    }
    if(d != NULL /*&& seg2[0] >= d->xMin() && seg2[1] >= d->yMin() && seg2[2] <= d->xMax() && seg2[3] <= d->yMax()*/){
        for(int i = 0; i<d->getDataSize(); i++){
            //if(seg2[0] <= (*d)[i][2] && seg2[2] >= (*d)[i][0] && seg2[1] <= (*d)[i][3] && seg2[3] >= (*d)[i][1]){
                inter = intersection((*d)[i], seg2);
                //cerr << "inter : " << inter[0] << ", " << inter[1] << endl;
                if(inter[0] != -1 && root->notInRecentTree(t)){
                    interMatch.push_back(inter);
                    indexMatch.push_back(i);
                    cerr << "ADD IN RECENT TREE : " << t << endl;
                    root->addRecentDataTree(t);
                    //cerr << "FIND DATA SORTIE NON NULL non rec " << endl;
                    return;
                }
            //}
        }
    }
    for(int i = 0; i<t->getChildrenSize(); i++){
        findData((*t)[i], root, seg2, interMatch, indexMatch);
    }
    //cerr << "FIND DATA SORTIE" << endl;
    return;
}

void afficherVec(vector<Tree<Data>*> v){
    if(v.size() == 0){
        cerr << "VECTEUR NULL LORS DE SON AFFICHAGE" << endl;
    }
    for(int i = 0; i<(int)v.size(); i++){
        cerr << " | " << v[i];
    }
    cerr << endl;
}


void moveTreeRecParent(Tree<Data>* t, vector<Tree<Data>*>& exploredTree, int delta){
    //cerr << "ENTREE MOVE PARENT REC" << endl;
    Tree<Data>* parent;
    Data* data;
    for(int i = 0; i<t->getParentSize(); i++){
        parent = t->getParent(i);
        if(!isInVector(exploredTree, parent)){
            data = parent->getValue();
            if(data == NULL){
                cerr << "PROBLEME DANS MOVE PARENT" << endl;
            }
            for(int k = 0; k<data->getDataSize(); k++){
                (*data)[k][0] += delta;
                (*data)[k][2] += delta;
            }
            data->shiftMinX(delta);
            data->shiftMaxX(delta);
            exploredTree.push_back(parent);
            moveTreeRecParent(parent, exploredTree, delta);
        }
    }
    //cerr << "SORTIE MOVE PARENT REC" << endl;
}

void moveTreeRecChild(Tree<Data>* t, vector<Tree<Data>*>& exploredTree, int delta){
    //cerr << "ENTREE MOVE CHILD REC" << endl;
    Tree<Data>* child;
    Data* data;
    for(int i = 0; i<t->getChildrenSize(); i++){
        child = (*t)[i];
        if(!isInVector(exploredTree, child)){
            data = child->getValue();
            for(int k = 0; k<data->getDataSize(); k++){
                (*data)[k][0] += delta;
                (*data)[k][2] += delta;
            }
            data->shiftMinX(delta);
            data->shiftMaxX(delta);
            exploredTree.push_back(child);
        }
        moveTreeRecChild(child, exploredTree, delta);
        moveTreeRecParent(child, exploredTree, delta);
    }
    //cerr << "SORTIE MOVE CHILD REC" << endl;
}

void moveData(vector<Tree<Data>*> recentTree, int delta){
    cerr << "ENTREE MOVE DATA size : " << recentTree.size() << endl;
    vector<Tree<Data>*> exploredTree = recentTree;
    for(int k = 0; k<(int)recentTree.size(); k++){
        moveTreeRecChild(recentTree[k],exploredTree, delta);
        for(int i = 0; i<(recentTree[k])->getChildrenSize(); i++){
            moveTreeRecParent((*(recentTree[k]))[i],exploredTree, delta);
        }
    }
    cerr << "SORTIE MOVE DATA" << endl;
}

void reductionRecentData(Tree<Data>* tRecent, Data* d, int delta, int index, bool& doClear){
    cerr << "data size début : " << d->getDataSize() << endl;
    delta = d->reduceData(delta,index,doClear);
    if(d->getDataSize() == 0){
        //cerr << "LES ENNUIS COMMENCENT" << endl;
        //fusionner 2 noeuds
        for(int k = 0; k<tRecent->getParentSize(); k++){
            (tRecent->getParent(k))->setChildrenVec(tRecent->getChildrenVec());
        }
        Data* childData;
        Data* parentData = (tRecent->getParent(0))->getValue();
        for(int k = 0; k<tRecent->getChildrenSize(); k++){
            ((*tRecent)[k])->setParentsVec(tRecent->getParentsVec());
            if(parentData != NULL){
                int parentDataSize = parentData->getDataSize() - 1;
                childData = tRecent->getValue();
                (*childData)[0][0] = (*parentData)[parentDataSize][0];
                (*childData)[0][1] = (*parentData)[parentDataSize][1];
                (*childData)[0][2] = (*parentData)[parentDataSize][2];
                (*childData)[0][3] = (*parentData)[parentDataSize][3];
            }
        }
        delete d;
    }
    if(delta < 0){
        cerr << "GROS PROBLEME DELTA ENCORE <0" << endl;
    }
}

void cutData(Data* d, Data* firstHalf, Data* secondHalf, double interX, double interY, int index, int delta){
    cerr << "ENTREE CUT DATA" << endl;
    for(int i = 0; i<index; i++){
        firstHalf->addData((*d)[i]);
    }
    Point halfPoint(4);
    halfPoint[0] = (*d)[index][0];
    halfPoint[1] = (*d)[index][1];
    halfPoint[2] = interX;
    halfPoint[3] = interY;
    firstHalf->addData(halfPoint);
    firstHalf->setMean(d->getMean());
    firstHalf->setVar(d->getVar());
    firstHalf->setSample(d->getSample());
    halfPoint[0] = interX+delta-1;
    halfPoint[1] = interY;
    halfPoint[2] = (*d)[index][2];
    halfPoint[3] = (*d)[index][3];
    secondHalf->addData(halfPoint);
    secondHalf->setMean(d->getMean());
    secondHalf->setVar(d->getVar());
    secondHalf->setSample(d->getSample());
    for(int i = index+1; i<d->getDataSize(); i++){
        secondHalf->addData((*d)[i]);
    }
    cerr << "SORTIE CUT DATA" << endl;
}

void updateRecentData(Tree<Data>& root, int delta){
    // ECRIRE LE CAS OU ILS NE SONT PAS TOUS EFFACES
    moveData(root.getRecentVecTree(), delta);
    int recentSize = root.getRecentTreeSize();
    bool doClear = false;
    Data* d;
    Tree<Data>* tRecent;
    //vector<Tree<Data>*> copyRecentTree = root.getRecentVecTree();
    for(int i = 0; i<recentSize; i++){
        tRecent = root.getRecentDataTree(i);
        d = tRecent->getValue();
        int pos = d->getPosInsert();
        if(delta < 0){
            reductionRecentData(tRecent, d, delta, pos, doClear);
            /*
            int ecart = (*d)[pos][2] - (*d)[pos][0];
            if(delta+ecart <= 0){
                d->eraseData();
            }else{
                for(int j = pos+1; j<d->getDataSize(); j++){
                    (*d)[j][0] += delta;
                    (*d)[j][2] += delta;
                }
                (*d)[pos][2] += delta;
            }
            */
        }else if(delta > 0){
            (*d)[pos][2] += delta;
            for(int k = pos+1; k<d->getDataSize(); k++){
                (*d)[k][0] += delta;
                (*d)[k][2] += delta;
            }
            d->shiftMaxX(delta);
            if(Tools::isGaussian((*d)[pos],d->getMean(),d->getVar(),d->getSample())){
                cerr << "ERASE DATA, i : " << i << endl;
                //root.eraseRecentDataTree(tRecent);
                doClear = true;
            }
        }
    }
    if(doClear){
        root.clearRecent();
    }
}

void newRecentData(Tree<Data>& root, int* min, int imageHeight, int delta){
    cerr << "ENTREE NEW RECENT DATA" << endl;
    Point seg2(4);
    vector<double*> interMatch;
    vector<int> indexMatch;
    //trouve l'intersection entre le chemin d'énergie min et les data
    for(int i = 0; i+1<imageHeight; i++){
        //cerr << "PIXEL " << min[i] << ", " << i << endl;
        seg2[0] = min[i];
        seg2[1] = i;
        seg2[2] = min[i+1];
        seg2[3] = i+1;
        findData(&root, &root, seg2, interMatch, indexMatch);
    }
    bool doClear = false;
    Tree<Data>* tRecent;
    Data* d;
    vector<Tree<Data>*> newRecentTree = root.getRecentVecTree();
    for(int i = 0; i<root.getRecentTreeSize(); i++){
        cerr << "i : " << i << endl;
        tRecent = root.getRecentDataTree(i);
        d = tRecent->getValue();
        if(d == NULL){
            cerr << "DATA NULL problème" << endl;
        }
        if(delta > 0){
            if(root.testGenerateNode()){
                cerr << "NEW NODE" << endl;
                int nbNode = 3;
                int connNode = 1;
                vector<Data*> newDatas;
                Data* firstHalf = new Data();
                Data* secondHalf = new Data();
                cutData(d,firstHalf,secondHalf,interMatch[i][0],interMatch[i][1],indexMatch[i],delta);
                if(firstHalf == NULL){
                    cerr << "firstHalf null" << endl;
                }
                if(secondHalf == NULL){
                    cerr << "secondHalf null" << endl;
                }
                Point someMean(4);
                someMean[0] = 0;
                someMean[1] = 0;
                someMean[2] = 50;
                someMean[3] = 25;
                double** someVar = new double*[4];
                for(int j = 0; j<4; j++){
                    someVar[j] = new double[4];
                    for(int l = 0; l<4; l++){
                        someVar[j][l] = 0;
                    }
                }
                someVar[3][3] = 6050;
                for(int k = 0; k<nbNode; k++){
                    newDatas.push_back(new Data());
                    (newDatas[k])->addData(Tools::generateY(someMean,someVar,interMatch[i][0],interMatch[i][0]+delta-1,interMatch[i][1]));
                    (newDatas[k])->setPosInsert(0);
                    (newDatas[k])->setMean(someMean);
                    (newDatas[k])->setVar(someVar);
                    for(int n = 0; n<d->getSampleSize(); n++){
                        (newDatas[k])->addSample(Tools::generateMulDim(someMean,someVar));
                    }
                }
                (*secondHalf)[0][0] = interMatch[i][0]+delta-1;
                (*secondHalf)[0][1] = (*(newDatas[connNode]))[0][3];
                tRecent->insertNode(firstHalf,secondHalf,newDatas,nbNode,connNode);
                for(int k = 0; k<(int)newRecentTree.size(); k++){
                    if(newRecentTree[k] == tRecent){
                        newRecentTree.erase(newRecentTree.begin()+k);
                    }
                }
                for(int k = 0; k<tRecent->getChildrenSize(); k++){
                    newRecentTree.push_back((*tRecent)[k]);
                }
                vector<Tree<Data>*> explored;
//                explored.push_back(&root);
                cerr << "enfant -1 branche 0 : " << &root << endl;
                printTree(root,0, explored);
            }else{
                cerr << "GENERATION ET INSERATION DATA" << endl;
                Point gen = Tools::generateY(d->getMean(),d->getVar(), interMatch[i][0], interMatch[i][0]+delta-1, interMatch[i][1]);
//            Point gen = Tools::pickInSample(d, interMatch[i], interMatch[i]+delta-1);
                d->insertData4D(gen,interMatch[i][0],indexMatch[i]);
            }
        }else if(delta < 0){
            reductionRecentData(tRecent, d, delta, indexMatch[i], doClear);
        }

        if((*d)[indexMatch[i]][0] == (int)interMatch[i][0]){
            cerr << "INTERSECTION NOEUD" << endl;
            //difficile à tester donc on suppose que ça marche ... pour le moment donc à vérifier en cas de problème inconnu
//            for(int k = 0; k<(int)parents.size(); k++){
//                dParent = (parents[k])->getValue();
//                if(dParent != NULL){
//                    cerr << "DECALAGE PARENT" << endl;
//                    (*dParent)[dParent->getDataSize()-1][3] = gen[3];
//                }
//            }
        }

    }
    if(newRecentTree.size() != 0){
        root.clearRecent();
        root.setRecentVec(newRecentTree);
    }
    cerr << "AFFICHAGE VECTEUR : " << endl;
    afficherVec(root.getRecentVecTree());
    moveData(root.getRecentVecTree(), delta);
    if(doClear){
        root.clearRecent();
    }
    cerr << "SORTIE NEW RECENT DATA" << endl;
}

void CAIR_Data(CML_color * Source, CML_int * S_Weights, int goal_x, CAIR_convolution conv, CAIR_energy ener, Tree<Data>& t){
    cerr << "CAIR DATA ENTREE" << endl;

    int delta = goal_x - (*Source).Width();
    cerr << "delta : " << delta << endl;
    int recentSize = t.getRecentTreeSize();
    if(recentSize != 0){
        updateRecentData(t, delta);
        cerr << "APRES UPDATE recent size : " << t.getRecentTreeSize() << endl;        
    }else{
        Startup_Threads();
        CML_image Image(1,1);
        CML_image_ptr Image_Ptr(1,1);
        Init_CML_Image(Source, S_Weights, &Image, &Image_Ptr);
        int* min = findMinPath(&Image_Ptr, conv, ener);
        newRecentData(t, min, Image_Ptr.Height(), delta);
        Shutdown_Threads();
        delete[] min;
    }
    vector<Tree<Data>*> explored;
//    explored.push_back(&t);
    cerr << "enfant -1 branche 0 : " << &t << endl;
    printTree(t,0, explored);
    cerr << "CAIR DATA SORTIE" << endl;
}

//=========================================================================================================//
//==                                          F R O N T E N D                                            ==//
//=========================================================================================================//
//The Great CAIR Frontend. This baby will retarget Source using S_Weights into the dimensions supplied by goal_x and goal_y into D_Weights and Dest.
//Weights allows for an area to be biased for removal/protection. A large positive value will protect a portion of the image,
//and a large negative value will remove it. Do not exceed the limits of int's, as this will cause an overflow. I would suggest
//a safe range of -2,000,000 to 2,000,000 (this is a maximum guideline, much smaller weights will work just as well for most images).
//Weights must be the same size as Source. D_Weights will contain the weights of Dest after the resize. Dest is the output,
//and as such has no constraints (its contents will be destroyed, just so you know). 
//The internal order is this: remove horizontal, remove vertical, add horizontal, add vertical.
//CAIR can use multiple convolution methods to determine the image energy. 
//Prewitt and Sobel are close to each other in results and represent the "traditional" edge detection.
//V_SQUARE and V1 can produce some of the better quality results, but may remove from large objects to do so. Do note that V_SQUARE
//produces much larger edge values, any may require larger weight values (by about an order of magnitude) for effective operation.
//Laplacian is a second-derivative operator, and can limit some artifacts while generating others.
//CAIR also can use the new improved energy algorithm called "forward energy." Removing seams can sometimes add energy back to the image
//by placing nearby edges directly next to each other. Forward energy can get around this by determining the future cost of a seam.
//Forward energy removes most serious artifacts from a retarget, but is slightly more costly in terms of performance.
bool CAIR( CML_color * Source, CML_int * S_Weights, int goal_x, int goal_y, CAIR_convolution conv, CAIR_energy ener, CML_int * D_Weights, CML_color * Dest, bool (*CAIR_callback)(float) )
{
	//if no change, then just copy to the source to the destination
	if( (goal_x == (*Source).Width()) && (goal_y == (*Source).Height() ) )
	{
		(*Dest) = (*Source);
		(*D_Weights) = (*S_Weights);
		return true;
	}

	//calculate the total number of operations needed
	int total_seams = abs((*Source).Width()-goal_x) + abs((*Source).Height()-goal_y);
	int seams_done = 0;

	//create threads for the run
	Startup_Threads();

	//build the image for internal use
	CML_image Image(1,1);
	CML_image_ptr Image_Ptr(1,1);
	Init_CML_Image(Source, S_Weights, &Image, &Image_Ptr);

	if( goal_x < (*Source).Width() )
	{
		//reduce width
		if( CAIR_Remove( &Image_Ptr, goal_x, conv, ener, CAIR_callback, total_seams, seams_done ) == false )
		{
			Shutdown_Threads();
			return false;
		}
		seams_done += abs((*Source).Width()-goal_x);
	}

	if( goal_y < (*Source).Height() )
	{
		//reduce height
		//works like above, except hand it a rotated image
		CML_image_ptr TImage_Ptr(1,1);
		TImage_Ptr.Transpose(&Image_Ptr);

		if( CAIR_Remove( &TImage_Ptr, goal_y, conv, ener, CAIR_callback, total_seams, seams_done ) == false )
		{
			Shutdown_Threads();
			return false;
		}
		
		//store back the transposed info
		Image_Ptr.Transpose(&TImage_Ptr);
		seams_done += abs((*Source).Height()-goal_y);
	}

	if( goal_x > (*Source).Width() )
	{
		//increase width
		if( CAIR_Add( &Image, &Image_Ptr, goal_x, conv, ener, CAIR_callback, total_seams, seams_done ) == false )
		{
			Shutdown_Threads();
			return false;
		}
		seams_done += abs((*Source).Width()-goal_x);
	}

	if( goal_y > (*Source).Height() )
	{
		//increase height
		//works like above, except hand it a rotated image
		CML_image_ptr TImage_Ptr(1,1);
		TImage_Ptr.Transpose(&Image_Ptr);

		if( CAIR_Add( &Image, &TImage_Ptr, goal_y, conv, ener, CAIR_callback, total_seams, seams_done ) == false )
		{
			Shutdown_Threads();
			return false;
		}
		
		//store back the transposed info
		Image_Ptr.Transpose(&TImage_Ptr);
		seams_done += abs((*Source).Height()-goal_y);
	}

	//pull the image data back out
	Extract_CML_Image(&Image_Ptr, Dest, D_Weights);

	//shutdown threads, remove semaphores and mutexes
	Shutdown_Threads();
	return true;
} //end CAIR()

//=========================================================================================================//
//==                                                E X T R A S                                          ==//
//=========================================================================================================//
//Simple function that generates the grayscale image of Source and places the result in Dest.
void CAIR_Grayscale( CML_color * Source, CML_color * Dest )
{
	Startup_Threads();

	CML_int weights((*Source).Width(),(*Source).Height()); //don't care about the values
	CML_image image(1,1);
	CML_image_ptr image_ptr(1,1);

	Init_CML_Image(Source,&weights,&image,&image_ptr);
	Grayscale_Image( &image_ptr );

	(*Dest).D_Resize( (*Source).Width(), (*Source).Height() );

	for( int x = 0; x < (*Source).Width(); x++ )
	{
		for( int y = 0; y < (*Source).Height(); y++ )
		{
			(*Dest)(x,y).red = image(x,y).gray;
			(*Dest)(x,y).green = image(x,y).gray;
			(*Dest)(x,y).blue = image(x,y).gray;
			(*Dest)(x,y).alpha = (*Source)(x,y).alpha;
		}
	}

	Shutdown_Threads();
}

//=========================================================================================================//
//Simple function that generates the edge-detection image of Source and stores it in Dest.
void CAIR_Edge( CML_color * Source, CAIR_convolution conv, CML_color * Dest )
{
	Startup_Threads();

	CML_int weights((*Source).Width(),(*Source).Height()); //don't care about the values
	CML_image image(1,1);
	CML_image_ptr image_ptr(1,1);

	Init_CML_Image(Source,&weights,&image,&image_ptr);
	Grayscale_Image(&image_ptr);
	Edge_Detect( &image_ptr, conv );

	(*Dest).D_Resize( (*Source).Width(), (*Source).Height() );

	for( int x = 0; x < (*Source).Width(); x++ )
	{
		for( int y = 0; y < (*Source).Height(); y++ )
		{
			int value = image(x,y).edge;

			if( value > 255 )
			{
				value = 255;
			}

			(*Dest)(x,y).red = (CML_byte)value;
			(*Dest)(x,y).green = (CML_byte)value;
			(*Dest)(x,y).blue = (CML_byte)value;
			(*Dest)(x,y).alpha = (*Source)(x,y).alpha;
		}
	}

	Shutdown_Threads();
}

//=========================================================================================================//
//Simple function that generates the vertical energy map of Source placing it into Dest.
//All values are scaled down to their relative gray value. Weights are assumed all zero.
void CAIR_V_Energy( CML_color * Source, CAIR_convolution conv, CAIR_energy ener, CML_color * Dest )
{
	Startup_Threads();

	CML_int weights((*Source).Width(),(*Source).Height());
	weights.Fill(0);
	CML_image image(1,1);
	CML_image_ptr image_ptr(1,1);

	Init_CML_Image(Source,&weights,&image,&image_ptr);
	Grayscale_Image(&image_ptr);
	Edge_Detect( &image_ptr, conv );

	//calculate the energy map
	Energy_Map( &image_ptr, ener, NULL );

	int max_energy = 0; //find the maximum energy value
	for( int y = 0; y < image.Height(); y++ )
	{
		for( int x = 0; x < image.Width(); x++ )
		{
			if( image(x,y).energy > max_energy )
			{
				max_energy = image(x,y).energy;
			}
		}
	}
	
	(*Dest).D_Resize( (*Source).Width(), (*Source).Height() );

	for( int y = 0; y < image.Height(); y++ )
	{
		for( int x = 0; x < image.Width(); x++ )
		{
			//scale the gray value down so we can get a realtive gray value for the energy level
			int value = (int)(((double)image(x,y).energy / max_energy) * 255);
			if( value < 0 )
			{
				value = 0;
			}

			(*Dest)(x,y).red = (CML_byte)value;
			(*Dest)(x,y).green = (CML_byte)value;
			(*Dest)(x,y).blue = (CML_byte)value;
			(*Dest)(x,y).alpha = (*Source)(x,y).alpha;
		}
	}

	Shutdown_Threads();
} //end CAIR_V_Energy()

//=========================================================================================================//
//Simple function that generates the horizontal energy map of Source placing it into Dest.
//All values are scaled down to their relative gray value. Weights are assumed all zero.
void CAIR_H_Energy( CML_color * Source, CAIR_convolution conv, CAIR_energy ener, CML_color * Dest )
{
	CML_color Tsource( 1, 1 );
	CML_color Tdest( 1, 1 );

	Tsource.Transpose( Source );
	CAIR_V_Energy( &Tsource, conv, ener, &Tdest );

	(*Dest).Transpose( &Tdest );
}

//=========================================================================================================//
//Experimental automatic object removal.
//Any area with a negative weight will be removed. This function has three modes, determined by the choice paramater.
//AUTO will have the function count the veritcal and horizontal rows/columns and remove in the direction that has the least.
//VERTICAL will force the function to remove all negative weights in the veritcal direction; likewise for HORIZONTAL.
//Because some conditions may cause the function not to remove all negative weights in one pass, max_attempts lets the function
//go through the remoal process as many times as you're willing.
bool CAIR_Removal( CML_color * Source, CML_int * S_Weights, CAIR_direction choice, int max_attempts, CAIR_convolution conv, CAIR_energy ener, CML_int * D_Weights, CML_color * Dest, bool (*CAIR_callback)(float) )
{
	int negative_x = 0;
	int negative_y = 0;
	CML_color Temp( 1, 1 );
	Temp = (*Source);
	(*D_Weights) = (*S_Weights);

	for( int i = 0; i < max_attempts; i++ )
	{
		negative_x = 0;
		negative_y = 0;

		//count how many negative columns exist
		for( int x = 0; x < (*D_Weights).Width(); x++ )
		{
			for( int y = 0; y < (*D_Weights).Height(); y++ )
			{
				if( (*D_Weights)(x,y) < 0 )
				{
					negative_x++;
					break; //only breaks the inner loop
				}
			}
		}

		//count how many negative rows exist
		for( int y = 0; y < (*D_Weights).Height(); y++ )
		{
			for( int x = 0; x < (*D_Weights).Width(); x++ )
			{
				if( (*D_Weights)(x,y) < 0 )
				{
					negative_y++;
					break;
				}
			}
		}

		switch( choice )
		{
		case AUTO :
			//remove in the direction that has the least to remove
			if( negative_y < negative_x )
			{
				if( CAIR( &Temp, D_Weights, Temp.Width(), Temp.Height() - negative_y, conv, ener, D_Weights, Dest, CAIR_callback ) == false )
				{
					return false;
				}
				Temp = (*Dest);
			}
			else
			{
				if( CAIR( &Temp, D_Weights, Temp.Width() - negative_x, Temp.Height(), conv, ener, D_Weights, Dest, CAIR_callback ) == false )
				{
					return false;
				}
				Temp = (*Dest);
			}
			break;

		case HORIZONTAL :
			if( CAIR( &Temp, D_Weights, Temp.Width(), Temp.Height() - negative_y, conv, ener, D_Weights, Dest, CAIR_callback ) == false )
			{
				return false;
			}
			Temp = (*Dest);
			break;

		case VERTICAL :
			if( CAIR( &Temp, D_Weights, Temp.Width() - negative_x, Temp.Height(), conv, ener, D_Weights, Dest, CAIR_callback ) == false )
			{
				return false;
			}
			Temp = (*Dest);
			break;
		}
	}

	//now expand back out to the origional
	return CAIR( &Temp, D_Weights, (*Source).Width(), (*Source).Height(), conv, ener, D_Weights, Dest, CAIR_callback );
} //end CAIR_Removal()

//The following Image Map functions are deprecated until better alternatives can be made.
#if 0
//=========================================================================================================//
//Precompute removals in the x direction. Map will hold the largest width the corisponding pixel is still visible.
//This will calculate all removals down to 3 pixels in width.
//Right now this only performs removals and only the x-direction. For the future enlarging is planned. Precomputing for both directions
//doesn't work all that well and generates significant artifacts. This function is intended for "content-aware multi-size images" as mentioned
//in the doctors' presentation. The next logical step would be to encode Map into an existing image format. Then, using a function like
//CAIR_Map_Resize() the image can be resized on a client machine with very little overhead.
void CAIR_Image_Map( CML_color * Source, CML_int * Weights, CAIR_convolution conv, CAIR_energy ener, CML_int * Map )
{
	Startup_Threads();
	Resize_Threads( (*Source).Height() );

	(*Map).D_Resize( (*Source).Width(), (*Source).Height() );
	(*Map).Fill( 0 );

	CML_color Temp( 1, 1 );
	Temp = (*Source);
	CML_int Temp_Weights( 1, 1 );
	Temp_Weights = (*Weights); //don't change Weights since there is no change to the image

	for( int i = Temp.Width(); i > 3; i-- ) //3 is the minimum safe amount with 3x3 convolution kernels without causing problems
	{
		//grayscale
		CML_gray Grayscale( Temp.Width(), Temp.Height() );
		Grayscale_Image( &Temp, &Grayscale );

		//edge detect
		CML_int Edge( Temp.Width(), Temp.Height() );
		Edge_Detect( &Grayscale, &Edge, conv );

		//find the energy values
		int * Path = new int[(*Source).Height()];
		CML_int Energy( Temp.Width(), Temp.Height() );
		Energy_Path( &Edge, &Temp_Weights, &Energy, Path, ener, true );

		Remove_Path( &Temp, Path, &Temp_Weights, &Edge, &Grayscale, &Energy, conv );

		//now set the corisponding map value with the resolution
		for( int y = 0; y < Temp.Height(); y++ )
		{
			int index = 0;
			int offset = Path[y];

			while( (*Map)(index,y) != 0 ) index++; //find the pixel that is in location zero (first unused)
			while( offset > 0 )
			{
				if( (*Map)(index,y) == 0 ) //find the correct x index
				{
					offset--;
				}
				index++;
			}
			while( (*Map)(index,y) != 0 ) index++; //if the current and subsequent pixels have been removed

			(*Map)(index,y) = i; //this is now the smallest resolution this pixel will be visible
		}

		delete[] Path;
	}

	Shutdown_Threads();

} //end CAIR_Image_Map()

//=========================================================================================================//
//An "example" function on how to decode the Map to quickly resize an image. This is only for the width, since multi-directional
//resizing produces significant artifacts. Do note this will produce different results than standard CAIR(), because this resize doesn't
//average pixels back into the image as does CAIR(). This function could be multi-threaded much like Remove_Path() for even faster performance.
void CAIR_Map_Resize( CML_color * Source, CML_int * Map, int goal_x, CML_color * Dest )
{
	(*Dest).D_Resize( goal_x, (*Source).Height() );

	for( int y = 0; y < (*Source).Height(); y++ )
	{
		int input_x = 0; //map the Source's pixels to the Dests smaller size
		for( int x = 0; x < goal_x; x++ )
		{
			while( (*Map)(input_x,y) > goal_x ) input_x++; //skip past pixels not in this resolution

			(*Dest)(x,y) = (*Source)(input_x,y);
			input_x++;
		}
	}
}
#endif

//=========================================================================================================//
//==                                             C A I R  H D                                            ==//
//=========================================================================================================//
//This works as CAIR, except here maximum quality is attempted. When removing in both directions some amount, CAIR_HD()
//will determine which direction has the least amount of energy and then removes in that direction. This is only done
//for removal, since enlarging will not benifit, although this function will perform addition just like CAIR().
//Inputs are the same as CAIR().
bool CAIR_HD( CML_color * Source, CML_int * S_Weights, int goal_x, int goal_y, CAIR_convolution conv, CAIR_energy ener, CML_int * D_Weights, CML_color * Dest, bool (*CAIR_callback)(float) )
{
	Startup_Threads();

	//if no change, then just copy to the source to the destination
	if( (goal_x == (*Source).Width()) && (goal_y == (*Source).Height()) )
	{
		(*Dest) = (*Source);
		(*D_Weights) = (*S_Weights);
		return true;
	}

	int total_seams = abs((*Source).Width()-goal_x) + abs((*Source).Height()-goal_y);
	int seams_done = 0;

	//build the internal image
	//I only use one image, but two image pointers. This way I can reuse the data.
	CML_image Temp(1,1);
	CML_image_ptr Temp_ptr(1,1);
	CML_image_ptr TTemp_ptr(1,1);
	Init_CML_Image( Source, S_Weights, &Temp, &Temp_ptr );
	TTemp_ptr.Transpose(&Temp_ptr);

	//grayscale (same for normal and transposed)
	Grayscale_Image( &Temp_ptr );

	//edge detect (same for normal and transposed)
	Edge_Detect( &Temp_ptr, conv );

	//do this loop when we can remove in either direction
	while( (Temp_ptr.Width() > goal_x) && (Temp_ptr.Height() > goal_y) )
	{
		//find the least energy seam, and its total energy for the normal image
		int * Path = new int[Temp_ptr.Height()];
		int energy_x = Energy_Path( &Temp_ptr, Path, ener, true );

		//now rebuild the energy, with the transposed pointers
		int * TPath = new int[TTemp_ptr.Height()];
		int energy_y = Energy_Path( &TTemp_ptr, TPath, ener, true );

		if( energy_y < energy_x )
		{
			Remove_Path( &TTemp_ptr, TPath, conv );

			//rebuild the losers pointers
			Temp_ptr.Transpose( &TTemp_ptr );
		}
		else
		{
			Remove_Path( &Temp_ptr, Path, conv );

			//rebuild the losers pointers
			TTemp_ptr.Transpose( &Temp_ptr );
		}

		delete[] Path;
		delete[] TPath;

		if( (CAIR_callback != NULL) && (CAIR_callback( (float)(seams_done)/total_seams ) == false) )
		{
			Shutdown_Threads();
			return false;
		}
		seams_done++;
	}

	//one dimension is the now on the goal, so finish off the other direction
	Extract_CML_Image(&Temp_ptr, Dest, D_Weights); //we should be able to get away with using the Dest as the Source
	Shutdown_Threads();
	return CAIR( Dest, D_Weights, goal_x, goal_y, conv, ener, D_Weights, Dest, CAIR_callback );
} //end CAIR_HD()
