// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "RBaseController.h"
// clang-format off
#include "ROmniwheelController.generated.h"
// clang-format on

USTRUCT()
struct FWheelSetting
{
  GENERATED_BODY()

public:
	FWheelSetting()
	{
		WheelRadius = 10.f;
		WheelToCenterSum = 70.65f;
		WheelFrontLeft = FString(TEXT("wheel_front_left"));
		WheelFrontRight = FString(TEXT("wheel_front_right"));
		WheelBackLeft = FString(TEXT("wheel_back_left"));
		WheelBackRight = FString(TEXT("wheel_back_right"));
	}
	
public:
  UPROPERTY(EditAnywhere)
	float WheelRadius;

	UPROPERTY(EditAnywhere)
	float WheelToCenterSum;

	UPROPERTY(EditAnywhere)
	FString WheelFrontLeft;

	UPROPERTY(EditAnywhere)
	FString WheelFrontRight;

	UPROPERTY(EditAnywhere)
	FString WheelBackLeft;

	UPROPERTY(EditAnywhere)
	FString WheelBackRight;
};

UCLASS()
class UROBOSIM_API UROmniwheelControllerParameter : public URBaseControllerParameter
{
  GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FWheelSetting WheelSetting;
};

UCLASS()
class UROBOSIM_API UROmniwheelController : public URBaseController
{
  GENERATED_BODY()

public:
  UROmniwheelController();

	virtual void SetControllerParameters(URControllerParameter *&ControllerParameters) override;

  virtual void Tick(const float &InDeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	FWheelSetting WheelSetting;

protected:
	virtual void MoveWheelTick(const float &InDeltaTime);

protected:
	UPROPERTY(VisibleAnywhere)
  TArray<double> WheelVelocities;
};