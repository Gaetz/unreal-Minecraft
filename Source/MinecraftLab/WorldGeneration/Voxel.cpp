// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel.h"
#include "FastNoiseLite.h"

int32 MeshTriangles[] { 2, 1, 0, 0, 3, 2};
FVector2d MeshUVs[] { FVector2D{ 0, 0 }, FVector2D{ 0, 1 }, FVector2D{ 1, 1 }, FVector2D{ 1, 0 } };
FVector Normals0[] { FVector { 0, 0, 1}, FVector { 0, 0, 1}, FVector { 0, 0, 1}, FVector { 0, 0, 1} };
FVector Normals1[] { FVector { 0, 0, -1}, FVector { 0, 0, -1}, FVector { 0, 0, -1}, FVector { 0, 0, -1} };
FVector Normals2[] { FVector { 0, 1, 0}, FVector { 0, 1, 0}, FVector { 0, 1, 0}, FVector { 0, 1, 0} };
FVector Normals3[] { FVector { 0, -1, 0}, FVector { 0, -1, 0}, FVector { 0, -1, 0}, FVector { 0, -1, 0} };
FVector Normals4[] { FVector { 1, 0, 0}, FVector { 1, 0, 0}, FVector { 1, 0, 0}, FVector { 1, 0, 0} };
FVector Normals5[] { FVector { -1, 0, 0}, FVector { -1, 0, 0}, FVector { -1, 0, 0}, FVector { -1, 0, 0} };
FVector Mask[] { FVector {0, 0, 1}, FVector {0, 0, -1}, FVector {0, 1, 0}, FVector {0, -1, 0}, FVector {1, 0, 0}, FVector {-1, 0, 0}};

// Sets default values
AVoxel::AVoxel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVoxel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxel::OnConstruction(const FTransform& Transform)
{
	ChunkZElements = 80;
	ChunkTotalElements = ChunkLineElements * ChunkLineElements * ChunkZElements;
	ChunkLineElementsP2 = ChunkLineElements * ChunkLineElements;
	VoxelHalfSize = VoxelSize / 2;

	const FString VoxelNameString = "Voxel_" + FString::FromInt(ChunkXIndex) + "_" + FString::FromInt(ChunkYIndex);
	const FName VoxelName = FName(*VoxelNameString);
	ProceduralMeshComponent = NewObject<UProceduralMeshComponent>(this, VoxelName);
	ProceduralMeshComponent->RegisterComponent();

	RootComponent = ProceduralMeshComponent;
	RootComponent->SetWorldTransform(Transform);
	Super::OnConstruction(Transform);

	GenerateChunk();
	UpdateProceduralMesh();
}

void AVoxel::GenerateChunk()
{
	ChunkFields.SetNumUninitialized(ChunkTotalElements);
	TArray<int32> Noise = ComputeChunkNoise();
	
	for(int32 x = 0; x < ChunkLineElements; ++x)
	{
		for(int32 y = 0; y < ChunkLineElements; ++y)
		{
			for(int32 z = 0; z < ChunkZElements; ++z)
			{
				const int32 Index = x + y * ChunkLineElements + z * ChunkLineElementsP2;
				
				ChunkFields[Index] = (BaseHeight + z < Noise[x + y * ChunkLineElements]) ? 1 : 0;
			}
		}
	}
}

void AVoxel::UpdateProceduralMesh()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	int32 ElementID = 0;

	for(int32 x = 0; x < ChunkLineElements; ++x)
	{
		for(int32 y = 0; y < ChunkLineElements; ++y)
		{
			for(int32 z = 0; z < ChunkZElements; ++z)
			{
				int32 Index = x + y * ChunkLineElements + z * ChunkLineElementsP2;
				int32 MeshIndex = ChunkFields[Index];
				if(MeshIndex > 0)
				{
					MeshIndex--;
					// Add faces, vertices, uvs and normals
					int32 NbTriangles = 0;
					for(int32 i =0; i < 6; ++i)
					{
						int NewIndex = Index + Mask[i].X + Mask[i].Y * ChunkLineElements + Mask[i].Z * ChunkLineElementsP2;
						bool Flag = false;
						if (MeshIndex >= 20) Flag = true;
						else if((x + Mask[i].X < ChunkLineElements) && (x + Mask[i].X >= 0)
							&& (y + Mask[i].Y < ChunkLineElements) && (y + Mask[i].Y >= 0))
						{
							if(NewIndex < ChunkFields.Num() && NewIndex >= 0)
							{
								if (ChunkFields[NewIndex] < 1)
								{
									Flag = true;
								}
							}
						}
						else
						{
							Flag = true;
						}

						if (Flag)
						{
							for(int32 k = 0; k < 6; ++k)
							{
								Triangles.Add(MeshTriangles[k] + NbTriangles + ElementID);
							}
							NbTriangles += 4;

							switch (i)
							{
							case 0:
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Normals.Append(Normals0, UE_ARRAY_COUNT(Normals0));
								break;
							case 1:
                                Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
                                Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
                                Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
                                Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
                                Normals.Append(Normals1, UE_ARRAY_COUNT(Normals1));
                                break;
							case 2:
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Normals.Append(Normals2, UE_ARRAY_COUNT(Normals2));
								break;
							case 3:
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Normals.Append(Normals2, UE_ARRAY_COUNT(Normals2));
								break;
							case 4:
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Normals.Append(Normals2, UE_ARRAY_COUNT(Normals2));
								break;
							case 5:
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, -VoxelHalfSize + z * VoxelSize));
								Vertices.Add(FVector(-VoxelHalfSize + x * VoxelSize, -VoxelHalfSize + y * VoxelSize, VoxelHalfSize + z * VoxelSize));
								Normals.Append(Normals2, UE_ARRAY_COUNT(Normals2));
								break;
							}
							
							UVs.Append(MeshUVs, UE_ARRAY_COUNT(MeshUVs));
							FColor Color = FColor (255, 255, 255, 1);
							VertexColors.Add(Color);
							VertexColors.Add(Color);
							VertexColors.Add(Color);
							VertexColors.Add(Color);
						}
					}
					ElementID += NbTriangles;
				}
			}
		}
	}
	ProceduralMeshComponent->ClearAllMeshSections();
	ProceduralMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
}

TArray<int32> AVoxel::ComputeChunkNoise() const
{
	// Create and configure FastNoise object
	FastNoiseLite NoiseGen;
	NoiseGen.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	
	TArray<int32> Noise;
	Noise.SetNum(ChunkLineElementsP2);
	int32 Index { 0 };
	for (int32 y = 0; y < ChunkLineElements; ++y)
	{
		const float NoiseY = (ChunkYIndex * ChunkLineElements + y) * YMult;
		for (int32 x = 0; x < ChunkLineElements; ++x)
		{
			const float NoiseX = (ChunkXIndex * ChunkLineElements + x) * XMult;
			const float NoiseValue1 = NoiseGen.GetNoise(NoiseX, NoiseY) * Weight;
			const float NoiseValue2 = NoiseGen.GetNoise(NoiseX * Resolution2Mult, NoiseY  * Resolution2Mult) * Weight2;
			const float NoiseValue3 = NoiseGen.GetNoise(NoiseX * Resolution3Mult, NoiseY * Resolution3Mult) * Weight3;
			const float NoiseValue4 = FMath::Clamp(NoiseGen.GetNoise(NoiseX * IrregularityResolutionMult, NoiseY * IrregularityResolutionMult) * Weight * IrregularityMult, ClampedMin, ClampedMax);
			Noise[Index++] = FMath::Floor(NoiseValue1 + NoiseValue2 + NoiseValue3 + NoiseValue4);
		}
	}
	
	return Noise;
}
