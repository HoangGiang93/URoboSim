#pragma once

#include "RGripperController.h"
// clang-format off
#include "RFingerGripperController.generated.h"
// clang-format on

USTRUCT()
struct FFingerMesh
{
  GENERATED_BODY()

public:
  FFingerMesh() {}

  FFingerMesh(const FString &InFingerName)
  : FingerName(InFingerName) {}

  UPROPERTY(VisibleAnywhere)
  FString FingerName;

  UPROPERTY(VisibleAnywhere)
  TArray<FString> FingerMeshNames;
};

UCLASS()
class UROBOSIM_API URFingerGripperControllerParameter : public URGripperControllerParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  float GripperHitDistance = 0.5f;
};

UCLASS()
class UROBOSIM_API URFingerGripperController : public URGripperController
{
  GENERATED_BODY()

public:
  virtual void Tick(const float &InDeltaTime) override;

  virtual void Init() override;

  virtual void SetControllerParameters(URControllerParameter *&ControllerParameters) override;

  virtual void GraspObject() override;

public:
  UPROPERTY(EditAnywhere)
  float GripperHitDistance = 0.5f;

protected:
  virtual void CheckFingerHitEvents();

protected:
  UPROPERTY(VisibleAnywhere)
  TArray<FFingerMesh> FingerMeshes;

  UPROPERTY(VisibleAnywhere)
  TMap<FString, int32> HitFingerCount;
};