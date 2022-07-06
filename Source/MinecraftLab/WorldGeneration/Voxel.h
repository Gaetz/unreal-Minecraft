// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Voxel.generated.h"

UCLASS()
class MINECRAFTLAB_API AVoxel : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInstance*> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 RandomSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 VoxelSize = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 ChunkLineElements = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 ChunkXIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	int32 ChunkYIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float XMult = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YMult = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZMult = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Freq = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseHeight = -10;

	UPROPERTY()
	int32 ChunkTotalElements;

	UPROPERTY()
	int32 ChunkZElements;

	UPROPERTY()
	int32 ChunkLineElementsP2;

	UPROPERTY()
	int32 VoxelHalfSize;

	UPROPERTY()
	TArray<int32> ChunkFields;

	UPROPERTY()
	UProceduralMeshComponent* ProceduralMeshComponent;
	
public:	
	// Sets default values for this actor's properties
	AVoxel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void GenerateChunk();
	void UpdateProceduralMesh();
	
	TArray<int32> ComputeChunkNoise() const;
};
