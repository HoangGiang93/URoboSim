#pragma once

#include "ROSBridgeSrvServer.h"
#include "ROSCommunication/RROSCommunication.h"
// clang-format off
#include "RServiceServer.generated.h"
// clang-format on

UCLASS(BlueprintType, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URServiceServerParameter : public UObject
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	FString Type;
};

UCLASS()
class UROBOSIM_API URServiceServer : public URROSCommunication
{
	GENERATED_BODY()

public:
	virtual void SetServiceServerParameters(URServiceServerParameter *&ServiceServerParameters);

public:
	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(VisibleAnywhere)
	FString Type;

protected:
  virtual void Init() override;

	virtual void CreateServiceServer(){};

protected:
	TSharedPtr<FROSBridgeSrvServer> ServiceServer;
};