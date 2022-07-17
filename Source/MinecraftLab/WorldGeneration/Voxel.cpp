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
				// Bug sur le choix du material, vérifier
				if (BaseHeight + z == Noise[x + y * ChunkLineElements]) ChunkFields[Index] = 1;
				else if (BaseHeight + z < Noise[x + y * ChunkLineElements]) ChunkFields[Index] = 1;
				else ChunkFields[Index] = 0;
				
				//ChunkFields[Index] = (BaseHeight + z < Noise[x + y * ChunkLineElements]) ? 1 : 0;
			}
		}
	}
}

void AVoxel::UpdateProceduralMesh()
{
	TArray<FMeshSection> MeshSections;
	MeshSections.SetNum(Materials.Num());
	int ElementNumber { 0 };
	
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
					TArray<FVector>& Vertices = MeshSections[MeshIndex].Vertices;
					TArray<int32>& Triangles = MeshSections[MeshIndex].Triangles;
					TArray<FVector>& Normals = MeshSections[MeshIndex].Normals;
					TArray<FVector2D>& UVs = MeshSections[MeshIndex].UVs;
					TArray<FColor>& VertexColors = MeshSections[MeshIndex].VertexColors;
					TArray<FProcMeshTangent>& Tangents = MeshSections[MeshIndex].Tangents;
					const int32 ElementID = MeshSections[MeshIndex].ElementID;
					
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
							// We'll use i as a marker in the material to tell the top side needs the top texture
							FColor Color = FColor (255, 255, 255, i);
							VertexColors.Add(Color);
							VertexColors.Add(Color);
							VertexColors.Add(Color);
							VertexColors.Add(Color);
						}
					}
					ElementNumber += NbTriangles;
					MeshSections[MeshIndex].ElementID += NbTriangles;
				}
			}
		}
	}

	ProceduralMeshComponent->ClearAllMeshSections();
	for(int i = 0; i < MeshSections.Num(); ++i)
	{
		const auto CurrentMeshSection = MeshSections[i];
		if (CurrentMeshSection.Vertices.Num() > 0)
		{
			ProceduralMeshComponent->CreateMeshSection(0, CurrentMeshSection.Vertices, CurrentMeshSection.Triangles,
				CurrentMeshSection.Normals, CurrentMeshSection.UVs, CurrentMeshSection.VertexColors,
				CurrentMeshSection.Tangents, true);
		}
	}
	
	int MatIndex { 0 };
	while (MatIndex < Materials.Num())
	{
		ProceduralMeshComponent->SetMaterial(MatIndex, Materials[MatIndex]);
		++MatIndex;
	}
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

void AVoxel::SetVoxel(FVector LocalPos, int32 ElementID)
{
	const int32 X = LocalPos.X / VoxelSize;
	const int32 Y = LocalPos.Y / VoxelSize;
	const int32 Z = LocalPos.Z / VoxelSize;
	for(int Xi = X - 1; Xi < X + 2; ++Xi)
	{
		for(int Yi = Y - 1; Yi < Y + 2; ++Yi)
		{
			for(int Zi = Z - 1; Zi < Z + 2; ++Zi)
			{
				const int32 Index = Xi + Yi * ChunkLineElements + Zi * ChunkLineElementsP2;
				ChunkFields[Index] = ElementID;
			}
		}
	}
	//const int32 Index = X + Y * ChunkLineElements + Z * ChunkLineElementsP2;
	//ChunkFields[Index] = ElementID;
	UpdateProceduralMesh();
}
