// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Voxel.generated.h"

struct FMeshSection
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;
	int32 ElementID { 0 };
};

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
	float Weight2 = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight3 = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Freq = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseHeight = -10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClampedMin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClampedMax = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Resolution2Mult = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Resolution3Mult = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IrregularityResolutionMult = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IrregularityMult = 10;
	
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

	UFUNCTION(BlueprintCallable, Category="Voxel")
	void SetVoxel(FVector LocalPos, int32 ElementID);
};
