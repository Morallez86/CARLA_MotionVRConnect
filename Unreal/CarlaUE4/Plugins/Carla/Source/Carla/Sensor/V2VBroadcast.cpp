#include "Carla.h"
#include "Carla/Sensor/V2VBroadcast.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Sensor/WalkerDetectionSensor.h"
#include "Carla/Vehicle/CarlaWheeledVehicle.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Constructor: Initializes the V2V broadcast sensor, sets up the sphere component for overlap detection,
// and sets default values for tick and broadcast radius.
AV2VBroadcast::AV2VBroadcast(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereOverlap"));
    Sphere->SetupAttachment(RootComponent);
    Sphere->SetHiddenInGame(false);
    Sphere->SetCollisionProfileName(FName("OverlapAll"));
    BroadcastRadius = 1000.0f;
}

// Called when the game starts or when the sensor is spawned.
// Sets up a timer to periodically broadcast walker data and logs the owner.
void AV2VBroadcast::BeginPlay()
{
    Super::BeginPlay();
    AActor* ParentActor = GetOwner();
    if (!ParentActor)
    {
        return;
    }
    GetWorldTimerManager().SetTimer(BroadcastTimerHandle, this, &AV2VBroadcast::PeriodicBroadcast, 1.0f, true);
}

// Called when the sensor is being destroyed or removed from the world.
// Clears the periodic broadcast timer.
void AV2VBroadcast::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearTimer(BroadcastTimerHandle);
}

// Called every physics tick before the physics simulation.
// Currently just calls the parent implementation.
void AV2VBroadcast::PrePhysTick(float DeltaSeconds)
{
    Super::PrePhysTick(DeltaSeconds);
}

// Returns the sensor definition for this V2V broadcast sensor, including its configurable attributes.
FActorDefinition AV2VBroadcast::GetSensorDefinition()
{
    auto Definition = UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(TEXT("other"), TEXT("v2v_broadcast"));

    FActorVariation Radius;
    Radius.Id = TEXT("broadcast_radius");
    Radius.Type = EActorAttributeType::Float;
    Radius.RecommendedValues = {TEXT("1000.0")};
    Radius.bRestrictToRecommended = false;
    Definition.Variations.Append({Radius});

    return Definition;
}

// Sets up the sensor based on the provided actor description, including setting the broadcast radius.
void AV2VBroadcast::Set(const FActorDescription& Description)
{
    Super::Set(Description);
    BroadcastRadius = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat("broadcast_radius", Description.Variations, 1000.0f);
    Sphere->SetSphereRadius(BroadcastRadius);
}

// Sets the owner of this sensor and tries to find and store a reference to an attached WalkerDetectionSensor.
void AV2VBroadcast::SetOwner(AActor* NewOwner)
{
    Super::SetOwner(NewOwner);

    if (!IsValid(NewOwner))
    {
        return;
    }

    TArray<AActor*> AttachedActors;
    NewOwner->GetAttachedActors(AttachedActors);

    for (AActor* AttachedActor : AttachedActors)
    {
        if (IsValid(AttachedActor))
        {
            AWalkerDetectionSensor* Sensor = Cast<AWalkerDetectionSensor>(AttachedActor);
            if (Sensor)
            {
                WalkerDetectionSensor = Sensor;
            }
        }
    }
}

// Sets the reference to the attached WalkerDetectionSensor.
void AV2VBroadcast::SetWalkerDetectionSensor(AWalkerDetectionSensor* Sensor)
{
    WalkerDetectionSensor = Sensor;
}

// Periodically broadcasts walker data to nearby vehicles within the broadcast radius.
void AV2VBroadcast::PeriodicBroadcast()
{
    if (!WalkerDetectionSensor) 
    {
        return;
    }
    FScopeLock Lock(&WalkerDetectionSensor->GetDataLock());

    const TMap<int32, FSharedWalkerDatas>& TrackedWalkers = WalkerDetectionSensor->GetTrackedWalkers();
    if (TrackedWalkers.Num() == 0) return;
    
    TSet<AActor*> NearbyVehicles;
    Sphere->GetOverlappingActors(NearbyVehicles, ACarlaWheeledVehicle::StaticClass());
    NearbyVehicles.Remove(GetOwner());

    for (AActor* Vehicle : NearbyVehicles)
    {
        // Get all attached actors of type AV2VBroadcast
        TArray<AActor*> AttachedActors;
        Vehicle->GetAttachedActors(AttachedActors);

        for (AActor* AttachedActor : AttachedActors)
        {
            AV2VBroadcast* VehicleBroadcastActor = Cast<AV2VBroadcast>(AttachedActor);
            if (VehicleBroadcastActor)
            {
                // Share walker data with the nearby vehicle
                for (const auto& Entry : TrackedWalkers)
                {
                    VehicleBroadcastActor->WalkerDetectionSensor->UpdateWalkerData(Entry.Key, Entry.Value.Location, Entry.Value.Timestamp, false);
                }
            }
        }
    }
}