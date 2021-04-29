#pragma once

#include "Controller/RController.h"
#include "ROSCommunication/Publisher/RPublisher.h"
#include "ROSCommunication/Subscriber/RSubscriber.h"
// clang-format off
#include "RActionServer.generated.h"
// clang-format on

UCLASS(BlueprintType, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URActionServerParameter : public UObject
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FString ActionName;

  UPROPERTY(EditAnywhere)
  FString ControllerName;
};

UCLASS()
class UROBOSIM_API URActionSubscriber : public URSubscriber
{
  GENERATED_BODY()

public:
  void SetController(URController *&InController) { Controller = InController; }

protected:
  URController *Controller;
};

UCLASS() class UROBOSIM_API URActionPublisher : public URPublisher
{
  GENERATED_BODY()

public:
  void SetController(URController *&InController) { Controller = InController; }

protected:
  URController *Controller;
};

UCLASS() 
class UROBOSIM_API URActionServer : public URROSCommunication
{
  GENERATED_BODY()

public:
  URActionServer(){};

public:
  void Tick() override;

  virtual void SetActionServerParameters(URActionServerParameter *&ActionServerParameters);

public:
  UPROPERTY(EditAnywhere)
  FString ControllerName;

protected:
  virtual void Init() override;

protected:
  UPROPERTY(EditAnywhere)
  FString ActionName;

  UPROPERTY(VisibleAnywhere)
  URActionSubscriber *GoalSubscriber;

  UPROPERTY(VisibleAnywhere)
  URActionSubscriber *CancelSubscriber;

  UPROPERTY(VisibleAnywhere)
  URActionPublisher *StatusPublisher;

  UPROPERTY(VisibleAnywhere)
  URActionPublisher *ResultPublisher;

  UPROPERTY(VisibleAnywhere)
  URActionPublisher *FeedbackPublisher;
};