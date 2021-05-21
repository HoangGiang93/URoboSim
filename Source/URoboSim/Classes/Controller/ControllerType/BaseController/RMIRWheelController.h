// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "RBaseController.h"
// clang-format off
#include "RMIRWheelController.generated.h"
// clang-format on

USTRUCT()
struct FMIRWheelSetting
{
  GENERATED_BODY()

public:
	FMIRWheelSetting()
	{
		WheelRadius = 5.f;
		WheelDistanceLength = 62.f;
		WheelDistanceWidth = 41.f;
		WheelLeft = FString(TEXT("left_wheel_link"));
		WheelRight = FString(TEXT("right_wheel_link"));
		WheelFrontLeftRotation = FString(TEXT("fl_caster_rotation_link"));
		WheelFrontLeftTranslation = FString(TEXT("fl_caster_wheel_link"));
		WheelFrontRightRotation = FString(TEXT("fr_caster_rotation_link"));
		WheelFrontRightTranslation = FString(TEXT("fr_caster_wheel_link"));
		WheelBackLeftRotation = FString(TEXT("bl_caster_rotation_link"));
		WheelBackLeftTranslation = FString(TEXT("bl_caster_wheel_link"));
		WheelBackRightRotation = FString(TEXT("br_caster_rotation_link"));
		WheelBackRightTranslation = FString(TEXT("br_caster_wheel_link"));
	}

public:
  UPROPERTY(EditAnywhere)
	float WheelRadius;

	UPROPERTY(EditAnywhere)
	float WheelDistanceLength;

	UPROPERTY(EditAnywhere)
	float WheelDistanceWidth;

	UPROPERTY(EditAnywhere)
	FString WheelLeft;

	UPROPERTY(EditAnywhere)
	FString WheelRight;

	UPROPERTY(EditAnywhere)
	FString WheelFrontLeftRotation;

	UPROPERTY(EditAnywhere)
	FString WheelFrontLeftTranslation;

	UPROPERTY(EditAnywhere)
	FString WheelFrontRightRotation;

	UPROPERTY(EditAnywhere)
	FString WheelFrontRightTranslation;

	UPROPERTY(EditAnywhere)
	FString WheelBackLeftRotation;

	UPROPERTY(EditAnywhere)
	FString WheelBackLeftTranslation;

	UPROPERTY(EditAnywhere)
	FString WheelBackRightRotation;

	UPROPERTY(EditAnywhere)
	FString WheelBackRightTranslation;
};

UCLASS()
class UROBOSIM_API URMIRWheelControllerParameter : public URBaseControllerParameter
{
  GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FMIRWheelSetting WheelSetting;
};

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URMIRWheelController : public URBaseController
{
  GENERATED_BODY()

public:
  URMIRWheelController();

public:
	virtual void SetControllerParameters(URControllerParameter *&ControllerParameters) override;

  virtual void Tick(const float &InDeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	FMIRWheelSetting WheelSetting;

protected:
	virtual void MoveWheelTick(const float &InDeltaTime);

protected:
  UPROPERTY(VisibleAnywhere)
  TArray<double> WheelVelocities;
};