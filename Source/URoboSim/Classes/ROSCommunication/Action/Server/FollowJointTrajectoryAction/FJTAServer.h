#pragma once

#include "ROSCommunication/Action/Server/RActionServer.h"
#include "ROSCommunication/Service/Client/RGetJointsClient.h"
// clang-format off
#include "FJTAServer.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URFJTAServerParameter : public URActionServerParameter
{
  GENERATED_BODY()

public:
  URFJTAServerParameter()
  {
    ActionName = TEXT("/whole_body_controller/body/follow_joint_trajectory");
    ControllerName = TEXT("JointTrajectoryController");
    FrameId = TEXT("odom");
    JointParamPath = TEXT("/whole_body_controller/body/joints");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId; 

  UPROPERTY(EditAnywhere)
  FString JointParamPath; 
};

UCLASS()
class UROBOSIM_API URFJTAServer final : public URActionServer
{
  GENERATED_BODY()

public:
  URFJTAServer();

public:
  void SetActionServerParameters(URActionServerParameter *&ActionServerParameters) override;

public:
  UPROPERTY(EditAnywhere)
  FString FrameId; 

  UPROPERTY(EditAnywhere)
  FString JointParamPath; 

protected:
  void Init() override;

  void CreateActionServer() override;

private:
  URGetJointsClient *GetJointsClient;
};
