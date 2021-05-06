#pragma once

#include "ROSBridgeSrvServer.h"
#include "ROSCommunication/RROSCommunication.h"
// clang-format off
#include "RServiceServer.generated.h"
// clang-format on

USTRUCT()
struct FRServiceServerParameterContainer
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FString ServiceName;

  UPROPERTY(EditAnywhere)
  FString ServiceType;
};

UCLASS(BlueprintType, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URServiceServerParameter : public UObject
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
	FRServiceServerParameterContainer CommonServiceServerParameters;
};

UCLASS()
class UROBOSIM_API URServiceServer : public URROSCommunication
{
	GENERATED_BODY()

public:
	virtual void SetServiceServerParameters(URServiceServerParameter *&ServiceServerParameters);

public:
	UPROPERTY(EditAnywhere)
	FRServiceServerParameterContainer CommonServiceServerParameters;

protected:
  virtual void Init() override;

	virtual void CreateServiceServer(){};

protected:
	TSharedPtr<FROSBridgeSrvServer> ServiceServer;
};