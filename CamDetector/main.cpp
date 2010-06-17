#include "cv.h"
#include "highgui.h"
#include <fann.h>
#include <floatfann.h> 
 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h> 
#include <float.h> 
#include <limits.h>
#include <time.h> 
#include <ctype.h>  
 
#ifdef _EiC
#define WIN32
#endif

//OPENCV UTILS
#define imu(img,i,j) (((uchar *)(img->imageData + img->widthStep*(i)))[j])

#define getRed(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2])   // Red
#define getGreen(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) // Green
#define getBlue(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0])  // Blue

#define getValue(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0])        // Value
#define setValue(img,i,j,v) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) = v // Value

using namespace std;

void getImageChannelRGB(IplImage * src, IplImage * dst, int channel) {
	for(int i=0;i<src->height;i++)	{
		for(int j=0;j<src->width;j++)		{
			if (channel==0) 
				imu(dst,i,j) = getBlue(src, i, j);
			if (channel==1) 
				imu(dst,i,j) = getGreen(src, i, j);
			if (channel==2)
				imu(dst,i,j) = getRed(src, i, j);
		}
	}
} 

static CvMemStorage* storage = 0;
struct fann *ann;

void detect_and_draw( IplImage* image, int resize , int lum, int Rs, int Gs, int Bs, int Rb, int Gb, int Bb );
IplImage *generateMap( IplImage *img, int Rs, int Gs, int Bs, int Rb, int Gb, int Bb  );

int main( int argc, char** argv )
{
	const char* annFileName = "network.net";
	int resize = 1;
	int lum = 0;
	
	int Rb = 0;
	int Gb = 0;
	int Bb = 0;
	
	int Rs = -1;
	int Gs = -1;
	int Bs = -1;
	
	for ( int i = 1; i < argc; ) {
		if ( strcmp(argv[i], "-n" ) == 0 ) {
			annFileName = argv[ i + 1 ];
		} else if ( strcmp(argv[i], "-r" ) == 0 ) {
			resize = atoi( argv[i + 1] );
		} else if ( strcmp(argv[i], "-l" ) == 0 ) {
			lum = atoi( argv[i + 1] );
		} else if ( strcmp(argv[i], "-b" ) == 0 ) {
			Rb = atoi( argv[i + 1] );
			Gb = atoi( argv[i + 2] );
			Bb = atoi( argv[i + 3] );
			i += 2;
		} else if ( strcmp(argv[i], "-s" ) == 0 ) {
			Rs = atoi( argv[i + 1] );
			Gs = atoi( argv[i + 2] );
			Bs = atoi( argv[i + 3] );
			i += 2;
		} else {
			cout << "ERROR - Usage: CamDetector -n network -r resize -l luminance -b Rb Gb Bb -s Rs Gs Bs" << endl;
			return 1;
		}
		i += 2;
	}
	
	ann = fann_create_from_file(annFileName);
    CvCapture* capture = 0;
    IplImage *frame, *frame_copy = 0;
    
    storage = cvCreateMemStorage(0);
    
    capture = cvCaptureFromCAM( 0 );

    cvNamedWindow( "original", 1 );
  //  cvNamedWindow( "preprocessed", 1 );
    cvNamedWindow( "skin", 1 );

    if( capture )
    {
        for(;;)
        {
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );
            if( !frame )
                break;
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                            IPL_DEPTH_8U, frame->nChannels );
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            else
                cvFlip( frame, frame_copy, 0 );
            
            detect_and_draw( frame_copy, resize, lum, Rs, Gs, Bs, Rb, Gb, Bb );

            if( cvWaitKey( 10 ) >= 0 )
                break;
        }

        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }
    
    cvDestroyWindow("result");
   // cvDestroyWindow("preprocessed");
    cvDestroyWindow("skin");
    
    fann_destroy(ann);

    return 0;
}

IplImage *equalizeLum( IplImage *img, int lum ) {
	IplImage *auxImg = cvCreateImage( cvSize( img->width, img->height ), IPL_DEPTH_8U, 3 );
	cvCvtColor( img, auxImg, CV_BGR2YCrCb );
	for ( int i = 0; i < auxImg->height; i++ ) {
		for ( int j = 0; j < auxImg->width; j++ ) {
			int v = getValue( auxImg, i, j ) + lum;
			if ( v > 255 ) {
				v = 255;
			}
			setValue( auxImg, i, j, v );
		}
	}
	IplImage *resultImg = cvCreateImage( cvSize( img->width, img->height ), IPL_DEPTH_8U, 3 );
	cvCvtColor( auxImg, resultImg, CV_YCrCb2BGR );
	cvReleaseImage( &auxImg );
	return resultImg;
}

void detect_and_draw( IplImage* img, int resize, int lum, int Rs, int Gs, int Bs, int Rb, int Gb, int Bb )
{
	IplImage *aux = equalizeLum(img, lum);
    IplImage *map = generateMap(aux, Rs, Gs, Bs, Rb, Gb, Bb);
	IplImage *mapMedian = cvCreateImage( cvSize( img->width, img->height ), IPL_DEPTH_8U, 3 );
	cvSmooth( map, mapMedian,
               CV_MEDIAN ); 

    cvShowImage( "original", img );
    IplImage *mapResize = cvCreateImage( cvSize( resize*img->width, resize*img->height ), IPL_DEPTH_8U, 3 );
    cvResize(mapMedian, mapResize, 0);
    cvShowImage( "skin", mapResize );
    cvReleaseImage( &map ); 
    cvReleaseImage( &mapMedian );
    cvReleaseImage( &mapResize ); 
    cvReleaseImage( &aux );
}

IplImage *generateMap( IplImage *img, int Rs, int Gs, int Bs, int Rb, int Gb, int Bb ) {
	IplImage *result = cvCreateImage( cvSize( img->width, img->height ), IPL_DEPTH_8U, 3 );
	fann_type *calc_out;
    fann_type input[3];

     
	
	IplImage *redChannel = cvCreateImage( cvSize( img->width, img->height ), IPL_DEPTH_8U, 1 );
	getImageChannelRGB( img, redChannel, 2 );
	
	IplImage *greenChannel = cvCreateImage( cvSize( img->width, img->height ), IPL_DEPTH_8U, 1 );
	getImageChannelRGB( img, greenChannel, 1 );

	IplImage *blueChannel = cvCreateImage( cvSize( img->width, img->height ), IPL_DEPTH_8U, 1 );
	getImageChannelRGB( img, blueChannel, 0 );


	for ( int i = 0; i < redChannel->height; i++ ) {
		for ( int j = 0; j < redChannel->width; j++ ) {			
			int redValue = ((uchar *)(redChannel->imageData + i*redChannel->widthStep))[j];
			int greenValue = ((uchar *)(greenChannel->imageData + i*greenChannel->widthStep))[j];
			int blueValue = ((uchar *)(blueChannel->imageData + i*blueChannel->widthStep))[j];
			double redNorm = 2*(redValue / 255.0)-1;
			double blueNorm = 2*( blueValue / 255.0)-1;
			double greenNorm = 2*(greenValue / 255.0)-1;	
			input[0] = redNorm;
    		input[1] = blueNorm;			
    		input[2] = greenNorm;
    		calc_out = fann_run(ann, input);    		
    		if ( calc_out[0] > calc_out[1] ) {
    			if ( Rs == -1 || Gs == -1 || Bs == -1 ) {
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+2] = redValue;
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+1] = greenValue;
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+0] = blueValue;
    			} else {
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+2] = Rs;
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+1] = Gs;
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+0] = Bs;
    			}
    		} else {
    			if ( Rb == -1 || Gb == -1 || Bb == -1 ) {
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+2] = redValue;
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+1] = greenValue;
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+0] = blueValue;
    			} else {
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+2] = Rb;
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+1] = Gb;
    				((uchar *)(result->imageData + i*result->widthStep))[j*3+0] = Bb;
    			}
    		}

		}
	}
	cvReleaseImage(&redChannel);
	cvReleaseImage(&greenChannel);
	cvReleaseImage(&blueChannel);
    
    return result;
}
