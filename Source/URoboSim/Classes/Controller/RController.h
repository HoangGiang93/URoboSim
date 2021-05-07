// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "Physics/RModel.h"
#include "ROSUtilities.h"
// clang-format off
#include "RController.generated.h"
// clang-format on

UCLASS(BlueprintType, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URControllerParameter : public UObject
{
  GENERATED_BODY()

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionFinishedDelegate);

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URController : public UObject
{
  GENERATED_BODY()

public:
  // Called every frame
  virtual void Tick(const float &InDeltaTime){};

  virtual void Init();

public:
  ARModel *GetOwner() const { return Owner; }

  void SetOwner(UObject *InOwner) { Owner = Cast<ARModel>(InOwner); }

  void SetOwner();

  virtual void SetControllerParameters(URControllerParameter *&ControllerParameters) {}

public:
  FActionFinishedDelegate ActionFinishedDelegate;

  FGoalStatusInfo GoalStatus;

  bool bCancel = false;

protected:
  virtual void CancelAction();

protected:
  bool bActive;

private:
  ARModel *Owner;
};