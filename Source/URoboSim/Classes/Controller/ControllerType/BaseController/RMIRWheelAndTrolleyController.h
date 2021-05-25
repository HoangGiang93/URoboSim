// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "RMIRWheelController.h"
// clang-format off
#include "RMIRWheelAndTrolleyController.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URMIRWheelAndTrolleyControllerParameter : public URMIRWheelControllerParameter
{
  GENERATED_BODY()

public:
  URMIRWheelAndTrolleyControllerParameter()
  {
    TrolleyName = TEXT("carriage_link");
  }

public:
	UPROPERTY(EditAnywhere)
	FString TrolleyName;
};

UCLASS()
class UROBOSIM_API URMIRWheelAndTrolleyController : public URMIRWheelController
{
  GENERATED_BODY()

public:
  URMIRWheelAndTrolleyController();
  
public:
  void SetControllerParameters(URControllerParameter *&ControllerParameters) override;

	void Init() override;

public:
  UPROPERTY(EditAnywhere)
	FString TrolleyName;

protected:
  void FixateTrolley();
};