// Fill out your copyright notice in the Description page of Project Settings.
#include "KinectDLL.h"

typedef bool(*_openKinect)(void);
typedef float* (*_getJoints)(float, float);
typedef unsigned char* (*_getColorImage)(void);
typedef uint16* (*_getDepthImage)(void);
typedef void(*_closeKinect)(void);


_openKinect mOpenKinect;
_getJoints mGetJoints;
_getColorImage mGetColorImage;
_getDepthImage mGetDepthImage;
_closeKinect mCloseKinect;

void* vDllHandle;

template<typename T>
bool loadFunction(T& foo, FString name)
{
	foo = NULL;
	foo = T(FPlatformProcess::GetDllExport(vDllHandle, *name));
	if (!foo)
		return false;
	
	return true;
}

bool UKinectDLL::loadDLL(FString folder, FString name, FString& err)
{
	err = "dll does not exist";
	FString filePath = *FPaths::GamePluginsDir() + folder + "/" + name;
	if (!FPaths::FileExists(filePath))
		return false;

	err = "cant load dll";
	vDllHandle = FPlatformProcess::GetDllHandle(*filePath);
	if (!vDllHandle)
		return false;

	err = "functions not loaded";
	if (!loadFunction(mOpenKinect, "openKinect")) return false;
	if (!loadFunction(mGetJoints, "getJoints")) return false;
	if (!loadFunction(mGetColorImage, "getColorImage")) return false;
	if (!loadFunction(mGetDepthImage, "getDepthImage")) return false;
	if (!loadFunction(mCloseKinect, "closeKinect")) return false;
	
	err = "ok";
	return true;
}

void UKinectDLL::unloadDLL()
{
	if (!vDllHandle)
		return;

	FGenericPlatformProcess::FreeDllHandle(vDllHandle);
}

bool UKinectDLL::openKinect()
{
	return mOpenKinect();
}

TArray<FVector> UKinectDLL::getJoints(float minDist, float maxDist, bool& success, FString& err)
{
	success = false;
	TArray<FVector> res;
	err = "no dll";
	if (!vDllHandle)
		return res;

	err = "no function";
	if (!mGetJoints)
		return res;

	res.Init(FVector(0), 1);
	float* coords = mGetJoints(minDist, maxDist);
	if (coords == nullptr)
		return res;
		

	for (uint32 i = 0; i < 25; i++)
	{
		float x = coords[i * 3];
		float y = coords[i * 3 + 1];
		float z = coords[i * 3 + 2];
		res.Add(FVector(x, y, z));
	}
	success = true;
	return res;
}

UTexture2D* UKinectDLL::getColorImage(bool& success)
{
	int32 width = 1920;
	int32 height = 1080;
	success = false;
	
	
	UTexture2D* colorImage = UTexture2D::CreateTransient(width, height, PF_B8G8R8A8);

	uint8* data = mGetColorImage();
	if (!data)
		return colorImage;

	int32 size = width * height * 4;
	TArray<uint8> rawData = TArray<uint8>(data, size);

	//copy
	void* TextureData = colorImage->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, rawData.GetData(), rawData.Num());
	
	colorImage->PlatformData->Mips[0].BulkData.Unlock();
	colorImage->UpdateResource();
	
	success = true;
	return colorImage;
}

UTexture2D* UKinectDLL::getDepthImage(bool& success)
{
	int32 width = 512;
	int32 height = 424;
	success = false;


	UTexture2D* depthImage = UTexture2D::CreateTransient(width, height, PF_G16);

	uint16* data = mGetDepthImage();
	if (!data)
		return depthImage;

	int32 size = width * height;
	TArray<uint16> rawData = TArray<uint16>(data, size);

	//copy
	void* TextureData = depthImage->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, rawData.GetData(), rawData.Num() * 2);

	depthImage->PlatformData->Mips[0].BulkData.Unlock();
	depthImage->UpdateResource();

	success = true;
	return depthImage;
}

void UKinectDLL::closeKinect()
{
	mCloseKinect();
}