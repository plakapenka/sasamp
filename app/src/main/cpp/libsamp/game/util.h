#pragma once

CPedGta* GamePool_FindPlayerPed();

int GamePool_Ped_GetIndex(CPedGta *pActor);

CVehicleGta *GamePool_Vehicle_GetAt(int iID);
int GamePool_Vehicle_GetIndex(CVehicleGta *pVehicle);

int LineOfSight(CVector* start, CVector* end, void* colpoint, uintptr_t ent,
	char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk);

bool IsValidModel(unsigned int uiModelID);
uint16_t GetModelReferenceCount(int nModelIndex);

void InitPlayerPedPtrRecords();
void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr);
uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr);

void DefinedState2d();
void SetScissorRect(void* pRect);
// 0.3.7
float FloatOffset(float f1, float f2);
float GetDistanceBetween3DPoints(CVector* f, CVector* s);

const char* GetAnimByIdx(int idx);
int GetAnimIdxByName(const char* szName);
struct RwRaster* GetRWRasterFromBitmap(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY);
struct RwRaster* GetRWRasterFromBitmapPalette(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void ProjectMatrix(CVector* vecOut, RwMatrix* mat, CVector* vecPos);
void RwMatrixOrthoNormalize(RwMatrix *matIn, RwMatrix *matOut);
void RwMatrixInvert(RwMatrix *matOut, RwMatrix *matIn);
RwMatrix* RwMatrixRotate(RwMatrix* mat, int axis, float angle);
void RwMatrixScale(RwMatrix* mat, CVector* vecScale);

int GetFreeTextDrawTextureSlot();
void DestroyTextDrawTexture(int index);
uintptr_t LoadTexture(const char* texname);
void DrawTextureUV(uintptr_t texture, RECT* rect, uint32_t dwColor, float* uv);

void WorldAddEntity(uintptr_t pEnt);

uintptr_t* GetModelInfoByID(int iModelID);

void DestroyAtomicOrClump(uintptr_t rwObject);
void GetModelColSphereVecCenter(int iModel, CVector* vec);
float GetModelColSphereRadius(int iModel);
void RenderClumpOrAtomic(uintptr_t rwObject);
uintptr_t ModelInfoCreateInstance(int iModel);
bool IsPointInRect(float x, float y, RECT* rect);
const char* GetAnimByIdx(int idx);
int GetAnimIdxByName(const char* szName);
struct RwRaster* GetRWRasterFromBitmap(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY);
struct RwRaster* GetRWRasterFromBitmapPalette(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY, uint8_t r, uint8_t g, uint8_t b, uint8_t a);