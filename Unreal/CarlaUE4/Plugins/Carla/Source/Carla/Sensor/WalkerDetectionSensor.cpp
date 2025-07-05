#include "Carla.h"
#include "Carla/Sensor/WalkerDetectionSensor.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Walker/WalkerBase.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Constructor: Initializes the walker detection sensor with default values.
AWalkerDetectionSensor::AWalkerDetectionSensor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    TraceRange = 1000.0f;
    CurrentHorizontalAngle = 0.0f;
}

// Called when the game starts or when the sensor is spawned.
void AWalkerDetectionSensor::BeginPlay()
{
    Super::BeginPlay();
}

// Called when the sensor is being destroyed or removed from the world.
void AWalkerDetectionSensor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

// Returns the sensor definition for this walker detection sensor, including its configurable attributes.
FActorDefinition AWalkerDetectionSensor::GetSensorDefinition()
{
    auto Definition = UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(TEXT("other"), TEXT("walker_detection"));

    FActorVariation Range;
    Range.Id = TEXT("trace_range");
    Range.Type = EActorAttributeType::Float;
    Range.RecommendedValues = {TEXT("1000.0")};
    Range.bRestrictToRecommended = false;
    Definition.Variations.Append({Range});

    return Definition;
}

// Sets up the sensor based on the provided actor description, including setting the trace range.
void AWalkerDetectionSensor::Set(const FActorDescription& Description)
{
    Super::Set(Description);
    TraceRange = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat("trace_range", Description.Variations, 1000.0f);
}

// Sets the owner of this sensor.
void AWalkerDetectionSensor::SetOwner(AActor* NewOwner)
{
    Super::SetOwner(NewOwner);
}

// Called every physics tick before the physics simulation.
// Performs the line trace and removes old walker data.
void AWalkerDetectionSensor::PrePhysTick(float DeltaSeconds)
{
    Super::PrePhysTick(DeltaSeconds);

    // Perform the line trace
    PerformLineTrace(DeltaSeconds);

    // Remove old walker data
    float CurrentTime = GetWorld()->GetTimeSeconds();
    TArray<int32> WalkersToRemove;
    for (auto& Entry : TrackedWalkers)
    {
        if (CurrentTime - Entry.Value.Timestamp > 180.0f)
        {
            WalkersToRemove.Add(Entry.Key);
        }
    }
    for (int32 WalkerID : WalkersToRemove)
    {
        TrackedWalkers.Remove(WalkerID);
    }
}

// Performs a rotating line trace to detect walkers and updates tracked walker data.
void AWalkerDetectionSensor::PerformLineTrace(float DeltaSeconds)
{
    FVector StartLocation = GetActorLocation();
    FRotator TraceRotation(0.0f, CurrentHorizontalAngle, 0.0f); // Rotate around the Z-axis (yaw)
    FVector EndLocation = StartLocation + TraceRotation.Vector() * TraceRange;

    FHitResult HitResult;
    FCollisionQueryParams TraceParams(FName(TEXT("Laser_Trace")), true, this);
    TraceParams.bTraceComplex = true;
    TraceParams.bReturnPhysicalMaterial = false;

    // Ignore the owner of the sensor
    AActor* OwnerActor = GetOwner();
    if (OwnerActor)
    {
        TraceParams.AddIgnoredActor(OwnerActor);
    }

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        AWalkerBase* Walker = nullptr;

        // Check if the HitActor is a walker or has a walker as its owner
        if (HitActor)
        {
            if (HitActor->IsA(AWalkerBase::StaticClass()))
            {
                Walker = Cast<AWalkerBase>(HitActor);
            }
            else if (HitActor->GetOwner() && HitActor->GetOwner()->IsA(AWalkerBase::StaticClass()))
            {
                Walker = Cast<AWalkerBase>(HitActor->GetOwner());
            }
        }

        // If a walker is found, update its data
        if (Walker)
        {
            UpdateWalkerData(Walker->GetUniqueID(), HitResult.ImpactPoint, GetWorld()->GetTimeSeconds(), true);
        }
    }
    // Rotate the trace for the next frame
    CurrentHorizontalAngle += DeltaSeconds * 360.0f; // Adjust the rotation speed based on DeltaSeconds
    if (CurrentHorizontalAngle >= 360.0f)
    {
        CurrentHorizontalAngle = 0.0f;
    }
    // Debug visualization
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0, 1.0f);
}

// Updates or adds walker data in the tracked walkers map, ensuring thread safety.
void AWalkerDetectionSensor::UpdateWalkerData(int32 WalkerID, const FVector& Location, float Timestamp, bool bDetectedByOwnVehicle)
{
    FScopeLock Lock(&DataLock);

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    // Find existing data for the walker
    auto* ExistingData = TrackedWalkers.Find(WalkerID);

    if (!ExistingData)
    {
        // Add new walker data if it doesn't exist
        TrackedWalkers.Add(WalkerID, FSharedWalkerDatas(WalkerID, Location, Timestamp, bDetectedByOwnVehicle));
    }
    else if (Timestamp > ExistingData->Timestamp)
    {
        // Update only the timestamp and location if the new data is more recent
        ExistingData->Location = Location;
        ExistingData->Timestamp = Timestamp;

        // Preserve the bDetectedByOwnVehicle flag if it was already true
        bool PreviousDetectedByOwnVehicle = ExistingData->bDetectedByOwnVehicle;
        ExistingData->bDetectedByOwnVehicle = PreviousDetectedByOwnVehicle || bDetectedByOwnVehicle;
    }
}

// Returns a const reference to the tracked walkers map.
const TMap<int32, FSharedWalkerDatas>& AWalkerDetectionSensor::GetTrackedWalkers() const
{
    return TrackedWalkers;
}

// Returns an array of walker locations relative to the owner actor.
TArray<FVector> AWalkerDetectionSensor::GetTrackedWalkerLocations() const
{
    TArray<FVector> RelativeLocations;
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor) return RelativeLocations;

    FVector OwnerLocation = OwnerActor->GetActorLocation();

    for (const auto& Entry : TrackedWalkers)
    {
        RelativeLocations.Add(Entry.Value.Location - OwnerLocation);
    }

    return RelativeLocations;
}

// Returns an array of walker locations in world coordinates.
TArray<FVector> AWalkerDetectionSensor::GetTrackedWalkerLocationsInWorld() const
{
    TArray<FVector> WorldLocations;
    for (const auto& Entry : TrackedWalkers)
    {
        WorldLocations.Add(Entry.Value.Location);
    }
    return WorldLocations;
}

// Returns an array of flags indicating if each walker was detected by the owning vehicle.
TArray<bool> AWalkerDetectionSensor::GetDetectedByOwnVehicleFlags() const
{
    TArray<bool> DetectedFlags;
    AActor* OwnerActor = GetOwner();
    for (const auto& WalkerData : TrackedWalkers)
    {
        DetectedFlags.Add(WalkerData.Value.bDetectedByOwnVehicle);
    }
    return DetectedFlags;
}