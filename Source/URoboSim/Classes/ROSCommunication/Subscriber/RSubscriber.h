// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ROSCommunication/RROSCommunication.h"
// clang-format off
#include "RSubscriber.generated.h"
// clang-format on

USTRUCT()
struct FRSubscriberParameterContainer
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FString Topic;

  UPROPERTY(VisibleAnywhere)
  FString MessageType;
};

UCLASS(BlueprintType, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URSubscriberParameter : public UObject
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FRSubscriberParameterContainer CommonSubscriberParameters;
};

UCLASS()
class UROBOSIM_API URSubscriber : public URROSCommunication
{
  GENERATED_BODY()

public:
  void Tick() override;

public:
  virtual void SetSubscriberParameters(URSubscriberParameter *&SubscriberParameters);

public:
  UPROPERTY(EditAnywhere)
  FRSubscriberParameterContainer CommonSubscriberParameters;

protected:
  virtual void Init() override;
  
  virtual void CreateSubscriber(){}

protected:
  TSharedPtr<FROSBridgeSubscriber> Subscriber;
};
