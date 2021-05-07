// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Michael Neumann

#pragma once

#include "GameFramework/Actor.h"
// #include "RGraspComponent.h"
#include "Physics/RJoint.h"
#include "Physics/RLink.h"
// clang-format off
#include "RModel.generated.h"
// clang-format on

class URPluginComponent;

USTRUCT()
struct FHitComponent
{
  GENERATED_BODY()

public:
  FHitComponent() {}

  FHitComponent(UPrimitiveComponent *InMyComponent, UPrimitiveComponent *InOtherComponent)
      : MyComponent(InMyComponent), OtherComponent(InOtherComponent) {}

  UPROPERTY(VisibleAnywhere)
  UPrimitiveComponent *MyComponent = nullptr;

  UPROPERTY(VisibleAnywhere)
  UPrimitiveComponent *OtherComponent = nullptr;
};

USTRUCT()
struct FEnableGravity
{
  GENERATED_BODY()

public:
  FEnableGravity() : bBase(true), bLinks(false) {}

  UPROPERTY(EditAnywhere)
  bool bBase;

  UPROPERTY(EditAnywhere)
  bool bLinks;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyHitDelegate, FHitComponent, HitComponent);

UCLASS()
class UROBOSIM_API ARModel : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values
  ARModel();

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  // Event when this actor bumps into a blocking object, or blocks another actor that bumps into it
  virtual void NotifyHit(UPrimitiveComponent *MyComp, AActor *Other, UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit) override;

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  void Init();

  void AddJoint(URJoint *&Joint) { Joints.Add(Joint); }

  void AddLink(URLink *&Link) { Links.Add(Link); }

  bool AddPlugin(URPluginComponent *Plugin);

  TArray<URJoint *> GetJoints() const { return Joints; }

  URJoint *GetJoint(const FString &JointName) const;

  TArray<URLink *> GetLinks() const { return Links; }

  URLink *GetBaseLink() const;

  URPluginComponent *GetPlugin(const FString &PluginName) const;

  class URController *GetController(const FString &ControllerName) const;

  class URSensor *GetSensor(const FString &SensorName) const;

public:
  FNotifyHitDelegate NotifyHitDelegate;

public:
  UPROPERTY(EditAnywhere)
  FEnableGravity EnableGravity;

private:
  UPROPERTY(VisibleAnywhere)
  TArray<URJoint *> Joints;

  UPROPERTY(VisibleAnywhere)
  TArray<URLink *> Links;

  UPROPERTY(EditAnywhere)
  TArray<URPluginComponent *> Plugins;
};