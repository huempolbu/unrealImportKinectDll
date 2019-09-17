// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Core.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KinectDLL.generated.h"

/**
 * 
 */
UCLASS()
class VIRTUALDRESSROOM_API UKinectDLL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Kinect")
	static bool loadDLL(FString folder, FString name, FString& err);

	UFUNCTION(BlueprintCallable, Category = "Kinect")
	static void unloadDLL();

	UFUNCTION(BlueprintCallable, Category = "Kinect")
	static bool openKinect();

	UFUNCTION(BlueprintCallable, Category = "Kinect")
	static TArray<FVector> getJoints(float minDist, float maxDist, bool &success, FString &err);

	UFUNCTION(BlueprintCallable, Category = "Kinect")
	static UTexture2D* getColorImage(bool& success);

	UFUNCTION(BlueprintCallable, Category = "Kinect")
	static UTexture2D* getDepthImage(bool& success);

	UFUNCTION(BlueprintCallable, Category = "Kinect")
	static void closeKinect();
};
