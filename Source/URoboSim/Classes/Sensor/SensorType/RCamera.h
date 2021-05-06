#pragma once

#include "Sensor/RSensor.h"
#include "RGBDCamera.h"
// clang-format off
#include "RCamera.generated.h"
// clang-format on

USTRUCT()
struct FRCameraParameterContainer
{
  GENERATED_BODY()

public:
  FRCameraParameterContainer()
  {
    CameraName = TEXT("RGBDCamera");
    ReferenceLinkName = TEXT("");
    CameraPoseOffset.SetRotation(FQuat(FRotator(90.f, 0.f, 90.f)));
  }

public:
  UPROPERTY(EditAnywhere)
  FString CameraName;

  UPROPERTY(EditAnywhere)
  FString ReferenceLinkName;

  UPROPERTY(EditAnywhere)
  FTransform CameraPoseOffset;
};

UCLASS()
class UROBOSIM_API URCameraParameter final : public URSensorParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FRCameraParameterContainer CameraParameters;
};

UCLASS()
class UROBOSIM_API URCamera final : public URSensor
{
  GENERATED_BODY()

public:
  URCamera();

public:
  // Called every frame
  void Tick(const float &InDeltaTime) override;

  void Init() override;

  void SetSensorParameters(URSensorParameter *&SensorParameters) override;

public:
  UPROPERTY(EditAnywhere)
  FRCameraParameterContainer CameraParameters;

private:
  UStaticMeshComponent *ReferenceLink = nullptr;

  ARGBDCamera* Camera;
};