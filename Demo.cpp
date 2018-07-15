#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h> 
#include "DVPCamera.h"


//#define TEST_TRIG
//#define SOFT_TRIG

void* test(void *p)
{

	dvpStatus status;
	dvpHandle h;
	dvpUint32 i,j,k;
	bool trigMode = false;

	char *name = (char*)p;
	char PicName[64];

	printf("test start, %s\r\n", name);
	k = 0;
	do 
	{
		/* Open the device */
		status = dvpOpenByName(name, OPEN_NORMAL, &h);
		if (status != DVP_STATUS_OK)
		{
			printf("dvpOpenByName failed with err:%d\r\n", status);
			break;
		}

		dvpRegion region;
		double exp;
		float gain;
		/* print ROI information */
		status = dvpGetRoi(h, &region);
		if (status != DVP_STATUS_OK)
		{
			printf("dvpGetRoi failed with err:%d\r\n", status);
			break;
		}
		printf("%s, region: x:%d, y:%d, w:%d, h:%d\r\n", name, region.X, region.Y, region.W, region.H);

		/* print exposure and gain information */
		status = dvpGetExposure(h, &exp);
		if (status != DVP_STATUS_OK)
		{
			printf("dvpGetExposure failed with err:%d\r\n", status);
			break;
		}

		status = dvpGetAnalogGain(h, &gain);
		if (status != DVP_STATUS_OK)
		{
			printf("dvpGetAnalogGain failed with err:%d\r\n", status);
			break;
		}

		printf("%s, exposure: %lf, gain: %f\r\n", name, exp, gain);


		uint32_t v;
		dvpSelectionDescr descr;
		dvpFrame frame;
		void *p;

		status = dvpGetQuickRoiSelDescr(h, &descr);
		if (status != DVP_STATUS_OK)
		{
			break;
		}
		/* Grab frames for every preset ROI */ 
		for (i=0; i<descr.uCount; i++)
		{
			/* Change image size */
			status = dvpSetQuickRoiSel(h, i);
			if (status != DVP_STATUS_OK)
			{
				break;
			}
			/* Start stream */
			status = dvpStart(h);
			if (status != DVP_STATUS_OK)
			{
				break;
			}

			/* Grab frames */
			for (j=0; j<10; j++)
			{
#ifdef SOFT_TRIG
				if (trigMode)
				{
					// trig a frame
					status = dvpTriggerFire(h);
					if (status != DVP_STATUS_OK)
					{
						printf("Fail to trig a frame\r\n");           
					}
				}
#endif
				status = dvpGetFrame(h, &frame, &p, 1000);
				if (status != DVP_STATUS_OK)
				{
					if (trigMode)
						continue;
					else
						break;
				}
				
				dvpFrameCount framecount;
				status = dvpGetFrameCount(h, &framecount);
				if(status != DVP_STATUS_OK)
				{
					printf("get framecount failed\n");
				}
				printf("framerate: %f, fProcRate: %f\n", framecount.fFrameRate, framecount.fProcRate);

				/* Show frame information */
				printf("%s, frame:%llu, timestamp:%llu, %d*%d, %dbytes, format:%d\r\n", 
					name,
					frame.uFrameID,
					frame.uTimestamp,
					frame.iWidth,
					frame.iHeight,
					frame.uBytes,
					frame.format);

				// 
				 				sprintf(PicName, "%s_pic_%d.jpg",name, k);
    				 				status = dvpSavePicture(&frame, p, PicName, 90);
				// 				if (status == DVP_STATUS_OK)
				// 				{
				// 					printf("Save to %s OK\r\n", PicName);
				// 				}

				k++;
			}        

			/* Stop stream */
			status = dvpStop(h);
			if (status != DVP_STATUS_OK)
			{
				break;
			}
		}

		if (status != DVP_STATUS_OK)
		{
			break;
		}
	}while(0);

	dvpClose(h);

	printf("test quit, %s, status:%d\r\n", name, status);
}


int main(int argc, char *argv[])
{

	printf("start...\r\n");

	dvpUint32 i,j,count;
	dvpCameraInfo info[8];   
	dvpStatus status;


	/* Update device list */
	dvpRefresh(&count);
	if (count > 8)
		count = 8;

	for (i = 0; i < count; i++)
	{    
		if(dvpEnum(i, &info[i]) == DVP_STATUS_OK)    
		{        
			printf("Camera FriendlyName : %s\r\n", info[i].FriendlyName);    
		}
	}

	/* No device found */
	if (count == 0)
		return 0;

	pthread_attr_t *threadAttr = new pthread_attr_t[count]; 
	pthread_t *th = new pthread_t[count];
	int r;


	for (j=0; j<count; j++)
	{
		pthread_attr_init(&threadAttr[j]); 
		r = pthread_create(&th[j], &threadAttr[j], test, (void*)info[j].FriendlyName); 
		if (r == 0)
		{
			printf("create thread for %s OK\r\n", info[j].FriendlyName);
		}
		else
		{
			printf("pthread_create failed, returned code:%d\r\n", r);
		}
	}

	for (j=0; j<count; j++)
	{
		if (th[j] != NULL)
		{
			void *s;
			pthread_join(th[j], &s);
		}
	}

	delete []threadAttr;
	delete []th;
	return 0;
}
