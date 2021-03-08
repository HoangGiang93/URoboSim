#include "Factory/RLinkFactory.h"

URLink* URLinkFactory::Load(UObject* InOuter, USDFLink* InLinkDescription, FVector InLocation)
{
  if(!InOuter && !InLinkDescription)
    {
      return nullptr;
    }

  LinkBuilder = CreateBuilder(InLinkDescription);
  if(LinkBuilder)
    {
      LinkBuilder->Init(InOuter, InLinkDescription,InLocation);
    }
  else
    {
      UE_LOG(LogTemp, Error, TEXT("LinkFactory: LinkBuilder not created because NumCollisions = 0"));
    }
  return LinkBuilder->NewLink();
}

URLink* URLinkFactory::Load(UObject* InOuter, USDFLink* InLinkDescription)
{
  if(!InOuter && !InLinkDescription)
    {
      return nullptr;
    }

  LinkBuilder = CreateBuilder(InLinkDescription);
  if(LinkBuilder)
    {
      LinkBuilder->Init(InOuter, InLinkDescription);
    }
  else
    {
      UE_LOG(LogTemp, Error, TEXT("LinkFactory: LinkBuilder not created because NumCollisions = 0"));
    }
  return LinkBuilder->NewLink();
}

URLinkBuilder* URLinkFactory::CreateBuilder(USDFLink* InLinkDescription)
{
  if(InLinkDescription->Collisions.Num()>0)
    {
      return NewObject<URLinkBuilder>(this);
    }
  else
    {
      return nullptr;
    }
}

void URLinkBuilder::Init(UObject* InOuter, USDFLink* InLinkDescription,FVector InLocation)
{
  Model = Cast<ARModel>(InOuter);
  LinkDescription = InLinkDescription;
  LoadLocation=InLocation;
}
void URLinkBuilder::Init(UObject* InOuter, USDFLink* InLinkDescription)
{
  Model = Cast<ARModel>(InOuter);
  LinkDescription = InLinkDescription;
  LoadLocation=FVector(0,0,0);
}


URLink* URLinkBuilder::NewLink()
{
  Link = NewObject<URLink>(Model, FName((LinkDescription->Name).GetCharArray().GetData()));
  Link->RegisterComponent();
  SetPose(LinkDescription->Pose);

  // Add the Collision Components to the Link
  SetCollisions();

  // Add the Visual Components to the Link
  SetVisuals();

  //Setup selfcollision
  SetCollisionProfile(LinkDescription->bSelfCollide);

  //SetInertial(Link, InLink->Inertial);

  //Enable or disable gravity for the Link
  SetSimulateGravity(LinkDescription->bGravity);

  return Link;
}

void URLinkBuilder::SetPose(FTransform InPose)
{
  LinkPose = InPose;
}

void URLinkBuilder::SetPose(FVector InLocation, FQuat InRotation)
{
  LinkPose.SetLocation(InLocation);
  LinkPose.SetRotation(InRotation);
}

void URLinkBuilder::SetVisuals()
{
  for(USDFVisual* Visual : LinkDescription->Visuals)
    {
      SetVisual(Visual);
    }
}


void URLinkBuilder::SetVisual(USDFVisual* InVisual)
{
  URStaticMeshComponent* LinkComponent = NewObject<URStaticMeshComponent>(Link, FName((InVisual->Name).GetCharArray().GetData()));
  LinkComponent->RegisterComponent();

  FVector LocationOffset = LinkPose.GetRotation().RotateVector(InVisual->Pose.GetLocation());
  LinkComponent->SetWorldLocation(LocationOffset + LinkPose.GetLocation() + LoadLocation);

  //Rotations are added by multiplying the Quaternions
  FQuat RotationOffset = LinkPose.GetRotation() * InVisual->Pose.GetRotation();
  LinkComponent->SetWorldRotation(RotationOffset);

  UStaticMesh* Visual = InVisual->Geometry->Mesh;
  if(Visual)
    {
      LinkComponent->SetStaticMesh(Visual);

      if(Link->Visuals.Num()==0)
        {
          if(Link->Collisions.Num()!=0)
            {
              LinkComponent->GetBodyInstance()->bAutoWeld=false;
              LinkComponent->AttachToComponent(Link->Collisions[0], FAttachmentTransformRules::KeepWorldTransform);
            }
        }
      else
        {
          LinkComponent->WeldTo(Link->Visuals[0]);
        }
      Link->Visuals.Add(LinkComponent);
    }
}


void URLinkBuilder::SetCollisions()
{
    for(USDFCollision* Collision : LinkDescription->Collisions)
    {
        SetCollision(Collision);
    }
}



void URLinkBuilder::SetCollision(USDFCollision* InCollision)
{
  URStaticMeshComponent* LinkComponent;
  if(Link->Collisions.Num()==0)
    {
      LinkComponent = Link;
      Link->Collisions.Add(LinkComponent);
    }
  else
    {
      LinkComponent = NewObject<URStaticMeshComponent>(Link, FName((InCollision->Name).GetCharArray().GetData()));
      LinkComponent->RegisterComponent();
    }

  if(Model->GetRootComponent() == nullptr)
    {
      Model->SetRootComponent(LinkComponent);
    }

  LinkComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
  LinkComponent->BodyInstance.PositionSolverIterationCount = 20;
  LinkComponent->BodyInstance.VelocitySolverIterationCount = 8;

  FVector LocationOffset = LinkPose.GetRotation().RotateVector(InCollision->Pose.GetLocation());
  LinkComponent->SetWorldLocation(LocationOffset + LinkPose.GetLocation() + LoadLocation);

  //Rotations are added by multiplying the Quaternions
  FQuat RotationOffset = LinkPose.GetRotation() * InCollision->Pose.GetRotation();
  LinkComponent->SetWorldRotation(RotationOffset);

  //Static Mesh creation
  UStaticMesh* Collision = InCollision->Geometry->Mesh;
  if(Collision)
    {
      LinkComponent->SetStaticMesh(Collision);

      if(Link->Collisions.Num()==0)
        {
          LinkComponent->SetSimulatePhysics(true);
          LinkComponent->AttachToComponent(Model->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
        }
      else
        {
          LinkComponent->AttachToComponent(Link->Collisions[0], FAttachmentTransformRules::KeepWorldTransform);
          LinkComponent->WeldTo(Link->Collisions[0]);
          Link->Collisions.Add(LinkComponent);
        }
      LinkComponent->bVisible = false;
    }
  else
    {
      UE_LOG(LogTemp, Error, TEXT("Collision Mesh not valid"));
    }
}

void URLinkBuilder::SetInertial(USDFLinkInertial* InInertial)
{
  for(URStaticMeshComponent* Visual : Link->Visuals)
    {
      Visual->SetMassOverrideInKg(NAME_None, 0.001, true);
    }
  for(URStaticMeshComponent* Collision : Link->Collisions)
    {
      Collision->SetMassOverrideInKg(NAME_None, 0.001, true);
    }
  if(Link)
    {
      Link->SetMassOverrideInKg(NAME_None, InInertial->Mass, true);
    }
}

void URLinkBuilder::SetCollisionProfile(bool InSelfColide)
{
  for(URStaticMeshComponent* Visual : Link->Visuals)
    {
      Visual->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
      Visual->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    }

  for(URStaticMeshComponent* Collision : Link->Collisions)
    {
      Collision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
      Collision->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
      if(Link->Visuals.Num() == 0)
        {
          Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
        }
      else
        {
          Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
        }
      if(!InSelfColide)
        {
          Collision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
        }
    }
}

void URLinkBuilder::SetSimulateGravity(bool InUseGravity)
{
  for(URStaticMeshComponent* Collision : Link->Collisions)
    {
      Collision->SetEnableGravity(false);
    }
  Link->SetEnableGravity(InUseGravity);

  for(URStaticMeshComponent* Visual : Link->Visuals)
    {
      Visual->SetEnableGravity(false);
    }
}
