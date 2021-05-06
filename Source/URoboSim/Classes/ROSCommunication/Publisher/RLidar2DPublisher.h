#pragma once

#include "RPublisher.h"
#include "Sensor/SensorType/RLidar2D.h"
// clang-format off
#include "RLidar2DPublisher.generated.h"
// clang-format on

USTRUCT()
struct FRLidar2DPublisherParameterContainer
{
  GENERATED_BODY()

public:
  FRLidar2DPublisherParameterContainer()
  {
    LidarReferenceROSLinkName = TEXT("base_laser_link");
    LidarName = TEXT("Laser");
  }

public:
  UPROPERTY(EditAnywhere)
  FString LidarReferenceROSLinkName;

  UPROPERTY(EditAnywhere)
  FString LidarName;
};

UCLASS()
class UROBOSIM_API URLidar2DPublisherParameter : public URPublisherParameter
{
  GENERATED_BODY()

public:
  URLidar2DPublisherParameter()
  {
    CommonPublisherParameters.Topic = TEXT("base_scan");
    CommonPublisherParameters.MessageType = TEXT("sensor_msgs/LaserScan");
  }

public:
  UPROPERTY(EditAnywhere)
  FRLidar2DPublisherParameterContainer Lidar2DPublisherParameters;
};

UCLASS()
class UROBOSIM_API URLidar2DPublisher : public URPublisher
{
  GENERATED_BODY()

public:
  URLidar2DPublisher();

public:
  void Publish() override;

  void SetPublishParameters(URPublisherParameter *&PublisherParameters) override;

protected:
  void Init() override;

public:
  UPROPERTY(EditAnywhere)
  FRLidar2DPublisherParameterContainer Lidar2DPublisherParameters;

private:
  void SetLidar();

private:
  URLidar2D *Lidar;
};
