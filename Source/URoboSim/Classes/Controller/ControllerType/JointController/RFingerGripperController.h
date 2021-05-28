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

USTRUCT()
struct FRFingerGripperControllerParameterContainer
{
  GENERATED_BODY()

public:
  FRFingerGripperControllerParameterContainer()
  {
    GraspingStrength = 1E3f;
  }

public:
  UPROPERTY(EditAnywhere)
  float GraspingStrength;
};

UCLASS()
class UROBOSIM_API URFingerGripperControllerParameter : public URGripperControllerParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FRFingerGripperControllerParameterContainer FingerGripperControllerParameters;
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

  virtual void SetGripperPosition() override;

  const float GetGripperPosition() const override;

public:
  UPROPERTY(EditAnywhere)
  FRFingerGripperControllerParameterContainer FingerGripperControllerParameters;

protected:
  virtual void CheckFingerHitEvents();

  UFUNCTION()
  void AddHitComponent(FHitComponent HitComponent);

protected:
  UPROPERTY(VisibleAnywhere)
  TArray<FFingerMesh> FingerMeshes;

  UPROPERTY(VisibleAnywhere)
  TMap<FString, int32> HitFingerCount;

  UPROPERTY(VisibleAnywhere)
  TArray<FHitComponent> HitComponents;
};