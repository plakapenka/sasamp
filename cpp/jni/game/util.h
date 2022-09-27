#pragma once

PED_TYPE* GamePool_FindPlayerPed();
PED_TYPE* GamePool_Ped_GetAt(int iID);
int GamePool_Ped_GetIndex(PED_TYPE *pActor);

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID);
int GamePool_Vehicle_GetIndex(VEHICLE_TYPE *pVehicle);

ENTITY_TYPE *GamePool_Object_GetAt(int iID);

int LineOfSight(VECTOR* start, VECTOR* end, void* colpoint, uintptr_t ent,
	char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk);

bool IsPedModel(unsigned int uiModel);
bool IsValidModel(unsigned int uiModelID);
uint16_t GetModelReferenceCount(int nModelIndex);

void InitPlayerPedPtrRecords();
void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr);
uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr);

uintptr_t GetTexture(const char* texture);
uintptr_t LoadTextureFromDB(const char* dbname, const char* texture);
void DefinedState2d();
void SetScissorRect(void* pRect);
float DegToRad(float fDegrees);
// 0.3.7
float FloatOffset(float f1, float f2);
float GetDistanceBetween3DPoints(VECTOR* f, VECTOR* s);

const char* GetAnimByIdx(int idx);
int GetAnimIdxByName(const char* szName);
struct RwRaster* GetRWRasterFromBitmap(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY);
struct RwRaster* GetRWRasterFromBitmapPalette(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void ProjectMatrix(VECTOR* vecOut, MATRIX4X4* mat, VECTOR* vecPos);
void RwMatrixRotate(MATRIX4X4* mat, int axis, float angle);
void RwMatrixScale(MATRIX4X4* mat, VECTOR* vecScale);

int GetFreeTextDrawTextureSlot();
void DestroyTextDrawTexture(int index);
uintptr_t LoadTexture(const char* texname);
void DrawTextureUV(uintptr_t texture, RECT* rect, uint32_t dwColor, float* uv);

void WorldAddEntity(uintptr_t pEnt);
void WorldRemoveEntity(uintptr_t pEnt);
uintptr_t GetModelInfoByID(int iModelID);

void DestroyAtomicOrClump(uintptr_t rwObject);
void GetModelColSphereVecCenter(int iModel, VECTOR* vec);
float GetModelColSphereRadius(int iModel);
void RenderClumpOrAtomic(uintptr_t rwObject);
uintptr_t ModelInfoCreateInstance(int iModel);
bool IsPointInRect(float x, float y, RECT* rect);
const char* GetAnimByIdx(int idx);
int GetAnimIdxByName(const char* szName);
struct RwRaster* GetRWRasterFromBitmap(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY);
struct RwRaster* GetRWRasterFromBitmapPalette(uint8_t* pBitmap, size_t dwStride, size_t dwX, size_t dwY, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

uintptr_t GetWeaponInfo(int iWeapon, int iSkill);
int Weapon_FireSniper(WEAPON_SLOT_TYPE* pWeaponSlot, PED_TYPE* pPed);
bool IsGameEntityArePlaceable(ENTITY_TYPE* pEntity);
bool IsValidGameEntity(ENTITY_TYPE* pEntity);
bool IsValidGamePed(PED_TYPE* pPed);

uintptr_t LoadFromTxdSlot(const char* szSlot, const char* szTexture);

enum eWidgetState
{
	STATE_NONE = 0,
	STATE_FIXED,
	STATE_NEEDEDFIX	
};

enum eHID
{  
	HID_ATTACK 		 = 1,
	HID_SPRINT 		 = 2,
	HID_JUMP,
	HID_CROUCH,
	HID_ENTERCAR	 = 5,
   	HID_BRAKE 		 = 6,
	HID_HANDBRAKE,
   	HID_ACCELERATE 	 = 8,
	HID_CAMERACLOSER,
	HID_CAMERAFARTHER,
	HID_HORN,
	HID_NEXTWEAPON 	= 15,
	HID_PREVWEAPON 	= 16,
	HID_RADAR,
   	HID_PEDMOVEX	= 30,
	HID_PEDMOVEY,
	HID_MENUDOWN 	= 60,
	HID_MENUUP 		= 61,
	HID_MENULEFT,
	HID_MENURIGHT,
};

enum eWidgetIds
{	
	TYPE_NONE = 0,
	TYPE_ATTACK,
	TYPE_SPRINT,
	TYPE_SWIM,
	TYPE_ENTERCAR,
	TYPE_ACCELERATE
};


#pragma pack(1)
typedef struct _WIDGET_TYPE
{
   uintptr_t   vtable;
   int         HID_ID;
} WIDGET_TYPE;
namespace widgets
{
	eWidgetIds GetId(WIDGET_TYPE* pWidget);
	WIDGET_TYPE* GetById(int WidgetId);
	void SetById(int idWidget, WIDGET_TYPE* pWidget);
	void SetByName(const char* name, WIDGET_TYPE* pWidget);
	bool IsEnabled(WIDGET_TYPE* pWidget);
	eWidgetState ProcessFixed(WIDGET_TYPE* pWidget);
}