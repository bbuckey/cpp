/*#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <al/al.h>
#include <al/alut.h>
#include "sound.h"

void SetListenerPosition(float x, float y, float z)
{
	//set the position using 3 seperate floats
	alListener3f(AL_POSITION, x,y,z);
}

void SetListenerOrientation(float fx, float fy, float fz, float ux, float uy, float uz)
{
	//set the orientation using an array of floats
	float vec[6];
	vec[0] = fx;
	vec[1] = fy;
	vec[2] = fz;
	vec[3] = ux;
	vec[4] = uy;
	vec[5] = uz;
	alListenerfv(AL_ORIENTATION, vec);

}

void Sound::LoadSound(char fname[40], bool looping)
{
	//load our sound
	ALboolean loop;
	loop = looping;
	alutLoadWAVFile(fname,&alFormatBuffer, (void **) &alBuffer,(unsigned int *)&alBufferLen, &alFreqBuffer, &loop);

	alGenSources(1, &alSource);
	alGenBuffers(1, &alSampleSet);
	alBufferData(alSampleSet, alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);
	alSourcei(alSource, AL_BUFFER, alSampleSet);

	alutUnloadWAV(alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);			

	//set the pitch
	alSourcef(alSource,AL_PITCH,1.0f);
	//set the gain
	alSourcef(alSource,AL_GAIN,1.0f);
	//set looping to true
	alSourcei(alSource,AL_LOOPING,AL_TRUE);
}



void Sound::SetProperties(float x, float y, float z, float vx, float vy, float vz)
{
	//set the sounds position and velocity
	alSource3f(alSource,AL_POSITION,x,y,z);
	alSource3f(alSource,AL_VELOCITY,vx,vy,vz);
}

//this function makes a sound source relative so all direction and velocity
//parameters become relative to the source rather than the listener
//useful for background music that you want to stay constant relative to the listener
//no matter where they go
void Sound::SetSourceRelative()
{
	alSourcei(alSource,AL_SOURCE_RELATIVE,AL_TRUE);
}

void Sound::PlaySound()
{
	alSourcePlay(alSource);
}

void Sound::StopSound()
{
	alSourceStop(alSource);
}

void Sound::DestroySound()
{
	alDeleteSources(1,&alSource);
	alDeleteBuffers(1,&alSampleSet);
}*/