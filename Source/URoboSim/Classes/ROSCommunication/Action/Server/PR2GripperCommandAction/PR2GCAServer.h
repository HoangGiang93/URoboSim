#pragma once

#include "ROSCommunication/Action/Server/RActionServer.h"
// clang-format off
#include "PR2GCAServer.generated.h"
// clang-format on

USTRUCT()
struct FRPR2GCAServerParameterContainer
{
  GENERATED_BODY()

public:
  FRPR2GCAServerParameterContainer()
  {
    FrameId = TEXT("");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId;
};

UCLASS()
class UROBOSIM_API URPR2GCAServerParameter : public URActionServerParameter
{
  GENERATED_BODY()

public:
  URPR2GCAServerParameter()
  {
    CommonActionServerParameters.ActionName = TEXT("gripper_controller");
    CommonActionServerParameters.ControllerName = TEXT("GripperController");
  }

public:
  UPROPERTY(EditAnywhere)
  FRPR2GCAServerParameterContainer PR2GCAServerParameters;
};

UCLASS()
class UROBOSIM_API URPR2GCAServer : public URActionServer
{
  GENERATED_BODY()

public:
  URPR2GCAServer();

public:
  void SetActionServerParameters(URActionServerParameter *&ActionServerParameters) override;

public:
  UPROPERTY(EditAnywhere)
  FRPR2GCAServerParameterContainer PR2GCAServerParameters;

protected:
  void CreateActionServer() override;
};
