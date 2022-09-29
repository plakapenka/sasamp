#include "main.h"
#include "../util/patch.h"

#define FLAG_ALPHA_TEST           0x01
#define FLAG_LIGHTING             0x02
#define FLAG_ALPHA_MODULATE       0x04
#define FLAG_COLOR_EMISSIVE       0x08
#define FLAG_COLOR                0x10
#define FLAG_TEX0                 0x20
#define FLAG_ENVMAP               0x40          // normal envmap
#define FLAG_BONE3                0x80
#define FLAG_BONE4                0x100
#define FLAG_CAMERA_BASED_NORMALS 0x200
#define FLAG_FOG                  0x400
#define FLAG_TEXBIAS              0x800
#define FLAG_BACKLIGHT            0x1000
#define FLAG_LIGHT1               0x2000
#define FLAG_LIGHT2               0x4000
#define FLAG_LIGHT3               0x8000
#define FLAG_DETAILMAP            0x10000
#define FLAG_COMPRESSED_TEXCOORD  0x20000
#define FLAG_PROJECT_TEXCOORD     0x40000
#define FLAG_WATER                0x80000
#define FLAG_COLOR2               0x100000
#define FLAG_SPHERE_XFORM         0x800000      // this renders the scene as a sphere map for vehicle reflections
#define FLAG_SPHERE_ENVMAP        0x1000000     // spherical real-time envmap
#define FLAG_TEXMATRIX            0x2000000
#define FLAG_GAMMA                0x4000000


typedef struct {
  // Checks for GL_OES_depth24
  char has24BitDepthCap;                   // 0x00
  // Checks for GL_OES_packed_depth_stencil
  char hasPackedDepthStencilCap;           // 0x01
  // Checks for GL_NV_depth_nonlinear
  char hasDepthNonLinearCap;               // 0x02
  // Checks for GL_EXT_texture_compression_dxt1 or GL_EXT_texture_compression_s3tc
  char hasTextureCompressionDXT1OrS3TCCap; // 0x03
  // Checks for GL_AMD_compressed_ATC_texture
  char hasTextureCompressionATCCap;        // 0x04
  // Checks for GL_IMG_texture_compression_pvrtc
  char hasTextureCompressionPVRTCCap;      // 0x05
  // Checks for GL_OES_rgb8_rgba8
  char has32BitRenderTargetCap;            // 0x06
  // Checks for GL_EXT_texture_filter_anisotropic
  char hasAnisotropicFilteringCap;         // 0x07
  // Set when OS_SystemChip() <= 1
  char unk_08;                             // 0x08
  // Always set to 0
  char unk_09;                             // 0x09
  // Checks for GL_QCOM_binning_control
  char hasBinningControlCap;               // 0x0A
  // Checks for GL_QCOM_alpha_test
  char hasAlphaTestCap;                    // 0x0B
  // Checks for Adreno (TM) 320 or GL_AMD_compressed_ATC_texture
  char isAdreno;                           // 0x0C
  // Set when there is no compression support
  char isMaliChip;                         // 0x0D
  // Checks for 225 or 540
  char isSlowGPU;                          // 0x0E
  char unk_0f;                             // 0x0F
} RQCapabilities;

RQCapabilities *RQCaps;
int *RQMaxBones;

char pxlbuf[1024 * 10];
char vtxbuf[1024 * 10];

char* byte_617290 = 0; //pxl
char* byte_617258 = 0;

char* byte_619294 = 0; //vertex

#define PXL_SOURCE(str) \
		strcat(byte_617290, str) \

#define VTX_SOURCE(str) \
        strcat(byte_619294, str) \


void (*BuildPixelSource)(unsigned int flags);
void BuildPixelSource_hook(unsigned int flags)
{
  /*char v2; // r6
  unsigned int v3; // r3
  bool v5; // r3
  const char *v6; // lr
  const char *v7; // lr
  int v8; // r0
  int v9; // r1
  int v10; // r2
  int v11; // r3
  int *v12; // lr
  int v13; // r1
  int v14; // r2
  int v15; // r3
  int v16; // r0
  int v17; // r1
  int v18; // r2
  int v19; // r3
  int *v20; // lr
  int v21; // r0
  int v22; // r1
  int v23; // r2
  int v24; // r3
  int v25; // r1
  int v26; // [sp+4h] [bp+4h]
  unsigned int v27; // [sp+8h] [bp+8h]
  unsigned int v28; // [sp+Ch] [bp+Ch]
  int v29[128]; // [sp+24h] [bp+24h] BYREF

    v26 = 13;
    PXL_SOURCE("precision mediump float;");

    if ( (flags & FLAG_TEX0) != 0 )
    {
        PXL_SOURCE("uniform sampler2D Diffuse;");
        PXL_SOURCE("varying mediump vec2 Out_Tex0;");
    }
    if ( (flags & (FLAG_SPHERE_ENVMAP | FLAG_ENVMAP)) != 0 )
    {
        PXL_SOURCE("uniform sampler2D EnvMap;");
        PXL_SOURCE("uniform lowp float EnvMapCoefficient;");
        v28 = flags & FLAG_ENVMAP;
        if ( (flags & FLAG_ENVMAP) != 0 )
        PXL_SOURCE("varying mediump vec2 Out_Tex1;");
        else
      PXL_SOURCE("varying mediump vec3 Out_Refl;");
    }
    else
    {
        v28 = flags & FLAG_ENVMAP;
        if ( (flags & FLAG_DETAILMAP) != 0 )
        {
            PXL_SOURCE("uniform sampler2D EnvMap;");
            PXL_SOURCE("uniform float DetailTiling;");
        }
    }
  
    if ( (flags & FLAG_FOG) != 0 )
    {
        PXL_SOURCE("varying mediump float Out_FogAmt;");
        PXL_SOURCE("uniform lowp vec3 FogColor;");
    }
    if ( (flags & (FLAG_LIGHTING | FLAG_COLOR)) != 0 )
    {
        PXL_SOURCE("varying lowp vec4 Out_Color;");
    }
    if ( (flags & FLAG_LIGHT1) != 0 && (flags & 0x10001C0) != 0 )
    {
        PXL_SOURCE("varying lowp vec3 Out_Spec;");
    }
    if ( (flags & 4) != 0 )
    {
        PXL_SOURCE("uniform lowp float AlphaModulate;");
    }
    v27 = flags & FLAG_WATER;
    if ( (flags & FLAG_WATER) != 0 )
    {
        PXL_SOURCE("varying mediump vec2 Out_WaterDetail;");
        PXL_SOURCE("varying mediump vec2 Out_WaterDetail2;");
        PXL_SOURCE("varying mediump float Out_WaterAlphaBlend;"); 

        PXL_SOURCE("uniform sampler2D reflectionTexture;");
        PXL_SOURCE("uniform sampler2D refractionTexture;");
    }

    PXL_SOURCE("void main()");
    PXL_SOURCE("{");
    PXL_SOURCE("lowp vec4 fcolor;");
  
    if ( (flags & FLAG_TEX0) == 0 )
    {
        if ( (flags & (FLAG_LIGHTING | FLAG_COLOR)) != 0 )
        {
            PXL_SOURCE("fcolor = Out_Color;");
            if ( !v28 )
                goto LABEL_32;
        }
        else
        {
            PXL_SOURCE("fcolor = 0.0;");
            if ( !v28 )
                goto LABEL_32;
        }
        PXL_SOURCE("fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");
    
LABEL_32:
    if ( (flags & FLAG_SPHERE_ENVMAP) == 0 )
        goto LABEL_33;
    goto LABEL_29;
    }
    if ( (flags & FLAG_TEXBIAS) != 0 )
    {
        PXL_SOURCE("lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -1.5);");  
    }
    else
    {
   // if ( byte_61725E )
      //PXL_SOURCE("lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);");
    //else
        PXL_SOURCE("lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -0.5);");
    
    }
    PXL_SOURCE("fcolor = diffuseColor;");
  
    if ( (flags & (FLAG_LIGHTING | FLAG_COLOR)) == 0 )
    {
LABEL_67:
        if ( v27 )
            goto LABEL_26;
        goto LABEL_27;
    }
    if ( (flags & FLAG_DETAILMAP) == 0 )
    {
        PXL_SOURCE("fcolor *= Out_Color;");
        goto LABEL_67;
    }
    if ( v27 )
    {
        PXL_SOURCE("float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;");
        PXL_SOURCE("fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);");
        //PXL_SOURCE("vec4 reflectTexture = texture2D(reflectionTexture, Out_WaterDetail2);");
        //PXL_SOURCE("vec4 refractTexture = texture2D(refractionTexture, Out_WaterDetail2);");
        //PXL_SOURCE("fcolor = mix(reflectTexture, refractTexture, 0.5);");
LABEL_26:
        PXL_SOURCE("fcolor.a += Out_WaterAlphaBlend;");
        goto LABEL_27;
    }
    PXL_SOURCE("fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * DetailTiling, -0.5).xyz * 2.0, Out_Color.w);");
LABEL_27:
    if ( !v28 )
        goto LABEL_32;
    PXL_SOURCE("fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");
  
    if ( (flags & FLAG_SPHERE_ENVMAP) != 0 )
    {
LABEL_29:
        PXL_SOURCE("vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);");
        PXL_SOURCE("ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);");
        PXL_SOURCE("lowp vec4 ReflTexture =  texture2D(EnvMap, ReflPos);");
        PXL_SOURCE("fcolor.xyz = mix(fcolor.xyz,ReflTexture.xyz, EnvMapCoefficient);");
        PXL_SOURCE("fcolor.w += ReflTexture.b * 0.125;");   
    }
LABEL_33:
    v2 = *byte_617258;
    if ( ((flags & FLAG_LIGHT1) != 0) > *byte_617258 && (flags & 0x10001C0) != 0 )
    {
        PXL_SOURCE("fcolor.xyz += Out_Spec;");
    }
    v3 = flags & FLAG_FOG;
    if ( (flags & FLAG_FOG) != 0 )
        v3 = 1;
    if ( v3 > v2 )
    {
    PXL_SOURCE("fcolor.xyz = mix(fcolor.xyz, FogColor, Out_FogAmt);");
    
    }
    if ( (flags & FLAG_GAMMA) != 0 )
    {
        PXL_SOURCE("fcolor.xyz += fcolor.xyz * 0.5;");
    }
    PXL_SOURCE("gl_FragColor = fcolor;");
    if ( (flags & FLAG_ALPHA_TEST) != 0 )
    {
        PXL_SOURCE("/*ATBEGIN*//*");
    
        v5 = v26 == 9;
        if ( (flags & FLAG_TEX0) == 0 )
        v5 = 0;
    if ( v5 )
    {
      if ( (flags & FLAG_TEXBIAS) != 0 )
      {
        v6 = "if (diffuseColor.a < 0.8) { discard; }";
      }
      else if ( (flags & FLAG_TEXBIAS) != 0 )
      {
        PXL_SOURCE("gl_FragColor.a = Out_Color.a;");
        
        v6 = "if (diffuseColor.a < 0.5) { discard; }";
      }
      else
      {
        v6 = "if (diffuseColor.a < 0.2) { discard; }";
      }
    }
    else if ( (flags & FLAG_TEXBIAS) != 0 )
    {
      v6 = "if (gl_FragColor.a < 0.8) { discard; }";
    }
    else
    {
      if ( (flags & FLAG_CAMERA_BASED_NORMALS) != 0 )
      {
        PXL_SOURCE("if (gl_FragColor.a < 0.5) { discard; }");
        PXL_SOURCE("gl_FragColor.a = Out_Color.a;");
LABEL_57:
        PXL_SOURCE("/*ATEND*//*");  
        goto LABEL_43;
      }
      v6 = "if (gl_FragColor.a < 0.2) { discard; }";
    }
	
	PXL_SOURCE(v6);
    goto LABEL_57;
  }
LABEL_43:
  if ( (flags & FLAG_ALPHA_MODULATE) != 0 )
  {
    PXL_SOURCE("gl_FragColor.a *= AlphaModulate;");
    
  }
  PXL_SOURCE("}");
  return; */

    int v2; // r8
  bool v3; // zf
  size_t v4; // r0
  const char *v5; // r2
  char s[512]; // [sp+10h] [bp-220h] BYREF
  int v8; // [sp+210h] [bp-20h]

  byte_617290[0] = 0;
  strcpy(s, "#version 100\n");
  strcat(byte_617290, s);
  
  strcpy(s, "precision mediump float;");
  strcat(byte_617290, s);
  
  if ( (flags & 0x20) != 0 )
  {
    strcpy(s, "uniform sampler2D Diffuse;");
    strcat(byte_617290, s);
    
    strcpy(s, "varying mediump vec2 Out_Tex0;");
    strcat(byte_617290, s);
    
  }
  if ( (flags & 0x1000040) != 0 )
  {
    strcpy(s, "uniform sampler2D EnvMap;");
    strcat(byte_617290, s);
    
    strcpy(s, "uniform lowp float EnvMapCoefficient;");
    strcat(byte_617290, s);
    
    if ( (flags & 0x40) != 0 )
      strcpy(s, "varying mediump vec2 Out_Tex1;");
    else
      strcpy(s, "varying mediump vec3 Out_Refl;");
  }
  else
  {
    if ( (flags & 0x10000) == 0 )
      goto LABEL_10;
    strcpy(s, "uniform sampler2D EnvMap;");
    strcat(byte_617290, s);
    
    strcpy(s, "uniform float DetailTiling;");
  }
  strcat(byte_617290, s);
  
LABEL_10:
  if ( (flags & 0x400) != 0 )
  {
    strcpy(s, "varying mediump float Out_FogAmt;");
    strcat(byte_617290, s);
    
    strcpy(s, "uniform lowp vec3 FogColor;");
    strcat(byte_617290, s);
    
    strcpy(s, "uniform sampler2D FogMap;");
    strcat(byte_617290, s);
    
    strcpy(s, "varying lowp vec4 Out_ClipSpace;");
    strcat(byte_617290, s);
    
  }
  if ( (flags & 0x12) != 0 )
  {
    strcpy(s, "varying lowp vec4 Out_Color;");
    strcat(byte_617290, s);
    
  }
  if ( (flags & 4) != 0 )
  {
    strcpy(s, "uniform lowp float AlphaModulate;");
    strcat(byte_617290, s);
    
  }
  v2 = flags & 0x80000;
  if ( (flags & FLAG_WATER) != 0 )
  {
    strcpy(s, "varying mediump vec2 Out_WaterDetail;");
    strcat(byte_617290, s);
    
    strcpy(s, "varying mediump vec2 Out_WaterDetail2;");
    strcat(byte_617290, s);
    
    strcpy(s, "varying mediump float Out_WaterAlphaBlend;");
    strcat(byte_617290, s);

    //strcpy(s, "uniform sampler2D reflectionTexture;");
    //strcat(byte_617290, s);

    //strcpy(s, "uniform sampler2D refractionTexture;");
    //strcat(byte_617290, s);
    
  }
  if ( (flags & 0x2000) != 0 )
  {
    strcpy(s, "varying lowp vec3 Out_Spec;");
    strcat(byte_617290, s);
    
  }
  strcpy(s, "void main()");
  strcat(byte_617290, s);
  
  strcpy(s, (const char *)"{");
  strcat(byte_617290, s);
  
  strcpy(s, "lowp vec4 fcolor;");
  strcat(byte_617290, s);
  
  if ( (flags & 0x400) != 0 )
  {
    strcpy(s, "lowp vec2 ndc = (Out_ClipSpace.xy/Out_ClipSpace.w) / 2.0 + 0.5;");
    strcat(byte_617290, s);
    
    strcpy(s, "lowp vec2 fogTexCoords = vec2(ndc.x, ndc.y);");
    strcat(byte_617290, s);
    
    strcpy(s, "lowp vec4 fogColorNew = texture2D(FogMap, fogTexCoords);");
    strcat(byte_617290, s);
    
  }
  if ( (flags & 0x20) == 0 )
  {
    if ( (flags & 0x12) != 0 )
      strcpy(s, "fcolor = Out_Color;");
    else
      strcpy(s, "fcolor = 0.0;");
    goto LABEL_42;
  }
  if ( (flags & 0x800) != 0 )
  {
    strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -1.5);");
  }
  else if ( *(char *)(RQCaps + 14) )
  {
    strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0);");
  }
  else
  {
    strcpy(s, "lowp vec4 diffuseColor = texture2D(Diffuse, Out_Tex0, -0.5);");
  }
  strcat(byte_617290, s);
  
  strcpy(s, "fcolor = diffuseColor;");
  strcat(byte_617290, s);
  
  if ( (flags & 0x12) == 0 )
  {
    if ( !v2 )
      goto LABEL_43;
    strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
    goto LABEL_42;
  }
  if ( (flags & 0x10000) == 0 )
  {
    strcpy(s, "fcolor *= Out_Color;");
    strcat(byte_617290, s);
    
    if ( v2 )
    {
      strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
      strcat(byte_617290, s);

      
      if ( (flags & FLAG_ENVMAP) == 0 )
        goto LABEL_45;
LABEL_44:
      strcpy(s, "fcolor.xyz = mix(fcolor.xyz, texture2D(EnvMap, Out_Tex1).xyz, EnvMapCoefficient);");
      strcat(byte_617290, s);
      
      goto LABEL_45;
    }
LABEL_43:
    if ( (flags & FLAG_ENVMAP) == 0 )
      goto LABEL_45;
    goto LABEL_44;
  }
  if ( !v2 )
  {
    snprintf(
      s,
      0x200u,
      "fcolor *= vec4(Out_Color.xyz * texture2D(EnvMap, Out_Tex0.xy * DetailTiling, -0.5).xyz * 2.0, Out_Color.w);");
LABEL_42:
    strcat(byte_617290, s);
    
    goto LABEL_43;
  }
  snprintf(
    s,
    0x200u,
    "float waterDetail = texture2D(EnvMap, Out_WaterDetail, -1.0).x + texture2D(EnvMap, Out_WaterDetail2, -1.0).x;");
  strcat(byte_617290, s);
  
  strcpy(s, "fcolor *= vec4(Out_Color.xyz * waterDetail * 1.1, Out_Color.w);");
  strcat(byte_617290, s);
  
  strcpy(s, "fcolor.a += Out_WaterAlphaBlend;");
  strcat(byte_617290, s);

  strcpy(s, "vec4 reflectColour = texture2D(EnvMap, Out_WaterDetail2);");
  strcat(byte_617290, s);

  strcpy(s, "vec4 refractColour = texture2D(Diffuse, Out_WaterDetail2);");
  strcat(byte_617290, s);

  strcpy(s, "fcolor = mix(reflectColour, refractColour, 0.5);");
  strcat(byte_617290, s);
  
  if ( (flags & 0x40) != 0 )
    goto LABEL_44;
LABEL_45:
  if ( (flags & 0x1000000) != 0 )
  {
    strcpy(s, "vec2 ReflPos = normalize(Out_Refl.xy) * (Out_Refl.z * 0.5 + 0.5);");
    strcat(byte_617290, s);
    
    strcpy(s, "ReflPos = (ReflPos * vec2(0.5,0.5)) + vec2(0.5,0.5);");
    strcat(byte_617290, s);
    
    strcpy(s, "lowp vec4 ReflTexture =  texture2D(EnvMap, ReflPos);");
    strcat(byte_617290, s);
    
    strcpy(s, "float newEnvMapCoef = EnvMapCoefficient + 0.14;");
    strcat(byte_617290, s);
    
    strcpy(s, "fcolor.xyz = mix(fcolor.xyz,ReflTexture.xyz, newEnvMapCoef);");
    strcat(byte_617290, s);
    
    strcpy(s, "fcolor.w += ReflTexture.b * 0.125;");
    strcat(byte_617290, s);
    
  }
  if ( !*(char*)(RQCaps + 8) )
  {
    v3 = (flags & 0x2000) == 0;
    if ( (flags & 0x2000) != 0 )
      v3 = (flags & 0x1000040) == 0;
    if ( !v3 )
    {
      strcpy(s, "fcolor.xyz += Out_Spec;");
      strcat(byte_617290, s);
      
    }
    if ( (flags & 0x400) != 0 )
    {
      strcpy(s, "fcolor.xyz = mix(fcolor.xyz, fogColorNew.xyz, Out_FogAmt);");
      strcat(byte_617290, s);
      
    }
  }
  if ( (flags & 0x4000000) != 0 )
  {
    strcpy(s, "fcolor.xyz += fcolor.xyz * 0.5;");
    strcat(byte_617290, s);
    
  }
  strcpy(s, "gl_FragColor = fcolor;");
  strcat(byte_617290, s);
  
  if ( flags << 31 )
  {
    strcpy(s, "/*ATBEGIN*/");
    strcat(byte_617290, s);
    
    if ( *(int *)(g_libGTASA + 6151472) == 9 && (flags & 0x20) != 0 )
    {
      if ( (flags & 0x800) != 0 )
      {
        strcpy(s, "if (diffuseColor.a < 0.8) { discard; }");
        goto LABEL_70;
      }
      if ( (flags & 0x200) == 0 )
      {
        strcpy(s, "if (diffuseColor.a < 0.2) { discard; }");
LABEL_70:
        strcat(byte_617290, s);
        
        strcpy(s, "/*ATEND*/");
        strcat(byte_617290, s);
        
        goto LABEL_71;
      }
      strcpy(s, "gl_FragColor.a = Out_Color.a;");
      strcat(byte_617290, s);
      v4 = strlen(byte_617290);
      v5 = "if (diffuseColor.a < 0.5) { discard; }";
    }
    else
    {
      if ( (flags & 0x800) != 0 )
      {
        strcpy(s, "if (gl_FragColor.a < 0.8) { discard; }");
        goto LABEL_70;
      }
      if ( (flags & 0x200) == 0 )
      {
        strcpy(s, "if (gl_FragColor.a < 0.2) { discard; }");
        goto LABEL_70;
      }
      strcpy(s, "if (gl_FragColor.a < 0.5) { discard; }");
      strcat(byte_617290, s);
      v4 = strlen(byte_617290);
      v5 = "gl_FragColor.a = Out_Color.a;";
    }
    strcpy(s, v5);
    goto LABEL_70;
  }
LABEL_71:
  if ( (flags & 4) != 0 )
  {
    strcpy(s, "gl_FragColor.a *= AlphaModulate;");
    strcat(byte_617290, s);
    
  }
  strcpy(s, "}");
  strcat(byte_617290, s);
  
  Log("build pixel shader");
  Log(byte_617290);
  Log("end build pixel shader");
}

void (*BuildVertexSource)(unsigned int flags);
void BuildVertexSource_hook(unsigned int flags)
{
  /*const char *v2; // r12
  int v3; // r0
  int v4; // r1
  int v5; // r2
  int v6; // r3
  int v7; // r12
  int v8; // r1
  int v9; // r2
  unsigned int v10; // r11
  int v11; // lr
  int v12; // r0
  int v13; // r1
  int v14; // r2
  int v15; // r3
  int v16; // lr
  int v17; // r1
  int v18; // r2
  int v19; // r3
  const char *v20; // r2
  unsigned int v22; // [sp+8h] [bp+0h]
  unsigned int v23; // [sp+2Ch] [bp+24h]
  char v24[512]; // [sp+34h] [bp+2Ch] BYREF
  int v25[128]; // [sp+234h] [bp+22Ch] BYREF

  strcpy((char *)v25, "#version 100\n");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "precision highp float;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "uniform mat4 ProjMatrix;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "uniform mat4 ViewMatrix;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "uniform mat4 ObjMatrix;");
  strcat(byte_619294, (const char *)v25);
  if ( (flags & 2) != 0 )
  {
    strcpy((char *)v25, "uniform lowp vec3 AmbientLightColor;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "uniform lowp vec4 MaterialEmissit;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "uniform lowp vec4 MaterialAmbienve;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "uniform lowp vec4 MaterialDiffuse;");
    strcat(byte_619294, (const char *)v25);
    v23 = flags & 0x2000;
    if ( (flags & 0x2000) != 0 )
    {
      strcpy((char *)v25, "uniform lowp vec3 DirLightDiffuseColor;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "uniform vec3 DirLightDirection;");
      strcat(byte_619294, (const char *)v25);
      //if ( GetMobileEffectSetting() == 3 && (flags & 0x1180) != 0 )
      if ((flags & 0x1180) != 0 )
      {
        strcpy((char *)v25, "uniform vec3 DirBackLightDirection;");
        strcat(byte_619294, (const char *)v25);
      }
    }
    if ( (flags & 0x4000) != 0 )
    {
      strcpy((char *)v25, "uniform lowp vec3 DirLight2DiffuseColor;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "uniform vec3 DirLight2Direction;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x8000) != 0 )
    {
      strcpy((char *)v25, "uniform lowp vec3 DirLight3DiffuseColor;");
      strcat(byte_619294, (const char *)v25);
      strcpy((char *)v25, "uniform vec3 DirLight3Direction;");
      strcat(byte_619294, (const char *)v25);
    }
  }
  else
  {
    v23 = flags & 0x2000;
  }
  strcpy((char *)v25, "attribute vec3 Position;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "attribute vec3 Normal;");
  strcat(byte_619294, (const char *)v25);
  if ( (flags & 0x20) != 0 )
  {
    if ( (flags & 0x40000) != 0 )
      v2 = "attribute vec4 TexCoord0;";
    else
      v2 = "attribute vec2 TexCoord0;";
    
    strcat(byte_619294, (const char *)v2);
  }
  strcpy((char *)v25, "attribute vec4 GlobalColor;");
  strcat(byte_619294, (const char *)v25);
  if ( (flags & 0x180) != 0 )
  {
    strcpy((char *)v25, "attribute vec4 BoneWeight;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "attribute vec4 BoneIndices;");
    strcat(byte_619294, (const char *)v25);
    snprintf((char *)v25, 0x200u, "uniform highp vec4 Bones[%d];", 3 * *RQMaxBones);
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & 0x20) != 0 )
  {
    strcpy((char *)v25, "varying mediump vec2 Out_Tex0;");
    strcat(byte_619294, (const char *)v25);
  }
  v22 = flags & 0x2000000;
  if ( (flags & 0x2000000) != 0 )
  {
    strcpy((char *)v25, "uniform mat3 NormalMatrix;");
    strcat(byte_619294, (const char *)v25);
  }
  v10 = flags & 0x1000040;
  if ( (flags & 0x1000040) != 0 )
  {
    if ( (flags & 0x40) != 0 )
      v11 = (int)"varying mediump vec2 Out_Tex1;";
    else
      v11 = (int)"varying mediump vec3 Out_Refl;";
    
    strcat(byte_619294, (const char *)v11);
    strcpy((char *)v25, "uniform lowp float EnvMapCoefficient;");
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & 0x18807C0) != 0 )
  {
    strcpy((char *)v25, "uniform vec3 CameraPosition;");
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & 0x400) != 0 )
  {
    strcpy((char *)v25, "varying mediump float Out_FogAmt;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "uniform vec3 FogDistances;");
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & FLAG_WATER) != 0 )
  {
    strcpy((char *)v25, "uniform vec3 WaterSpecs;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "varying mediump vec2 Out_WaterDetail;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "varying mediump vec2 Out_WaterDetail2;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "varying mediump float Out_WaterAlphaBlend;");
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & 0x100000) != 0 )
  {
    strcpy((char *)v25, "attribute vec4 Color2;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "uniform lowp float ColorInterp;");
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & 0x12) != 0 )
  {
    strcpy((char *)v25, "varying lowp vec4 Out_Color;");
    strcat(byte_619294, (const char *)v25);
  }
  if ( v23 && (flags & 0x10001C0) != 0 )
  {
    strcpy((char *)v25, "varying lowp vec3 Out_Spec;");
    strcat(byte_619294, (const char *)v25);
  }
  strcpy((char *)v25, "void main() {");
  strcat(byte_619294, (const char *)v25);
  if ( (flags & 0x180) == 0 )
  {
    strcpy((char *)v25, "vec4 WorldPos = ObjMatrix * vec4(Position,1.0);");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 0x800000) != 0 )
      goto LABEL_34;
LABEL_60:
    strcpy((char *)v25, "vec4 ViewPos = ViewMatrix * WorldPos;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "gl_Position = ProjMatrix * ViewPos;");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 2) != 0 )
      goto LABEL_35;
    goto LABEL_61;
  }
  strcpy((char *)v25, "ivec4 BlendIndexArray = ivec4(BoneIndices);");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "mat4 BoneToLocal;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[0] = Bones[BlendIndexArray.x*3] * BoneWeight.x;x;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[1] = Bones[BlendIndexArray.x*3+1] * BoneWeight.");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[2] = Bones[BlendIndexArray.x*3+2] * BoneWeight.x;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[3] = vec4(0.0,0.0,0.0,1.0);");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[0] += Bones[BlendIndexArray.y*3] * BoneWeight.y;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[1] += Bones[BlendIndexArray.y*3+1] * BoneWeight.y;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[2] += Bones[BlendIndexArray.y*3+2] * BoneWeight.y;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[0] += Bones[BlendIndexArray.z*3] * BoneWeight.z;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[1] += Bones[BlendIndexArray.z*3+1] * BoneWeight.z;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "BoneToLocal[2] += Bones[BlendIndexArray.z*3+2] * BoneWeight.z;");
  strcat(byte_619294, (const char *)v25);
  if ( (flags & 0x100) != 0 )
  {
    strcpy((char *)v25, "BoneToLocal[0] += Bones[BlendIndexArray.w*3] * BoneWeight.w;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[1] += Bones[BlendIndexArray.w*3+1] * BoneWeight.w;");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "BoneToLocal[2] += Bones[BlendIndexArray.w*3+2] * BoneWeight.w;");
    strcat(byte_619294, (const char *)v25);
  }
  strcpy((char *)v25, "vec4 WorldPos = ObjMatrix * (vec4(Position,1.0) * BoneToLocal);");
  strcat(byte_619294, (const char *)v25);
  if ( (flags & 0x800000) == 0 )
    goto LABEL_60;
LABEL_34:
  strcpy((char *)v25, "vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "vec3 ReflPos = normalize(ReflVector);");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
  strcat(byte_619294, (const char *)v25);
  strcpy((char *)v25, "gl_Position = vec4(ReflPos.xy, length(ReflVector) * 0.002, 1.0);");
  strcat(byte_619294, (const char *)v25);
  if ( (flags & 2) != 0 )
  {
LABEL_35:
    if ( (flags & 0x201) == 513 && (flags & 0xE000) != 0 )
    {
      strcpy((char *)v25, "vec3 WorldNormal = normalize(vec3(WorldPos.xy - CameraPosition.xy, 0.0001)) * 0.85;");
      strcat(byte_619294, (const char *)v25);
    }
    else
    {
      if ( (flags & 0x180) != 0 )
        strcpy((char *)v25, "vec3 WorldNormal = mat3(ObjMatrix) * (Normal * mat3(BoneToLocal));");
      else
        strcpy((char *)v25, "vec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;");
      strcat(byte_619294, (const char *)v25);
    }
    goto LABEL_39;
  }
LABEL_61:
  if ( v10 )
  {
    strcpy((char *)v25, "vec3 WorldNormal = vec3(0.0, 0.0, 0.0);");
    strcat(byte_619294, (const char *)v25);
  }
LABEL_39:
  if ( *byte_617258 < (unsigned int)((flags & 0x400) != 0) )
  {
    strcpy(
      (char *)v25,
      "Out_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 0.90);");
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & 0x20) != 0 )
  {
    if ( (flags & 0x40000) != 0 )
    {
      strcpy(v24, "TexCoord0.xy / TexCoord0.w");
    }
    else
    {
      if ( (flags & 0x20000) != 0 )
      {
        strcpy(v24, "TexCoord0 / 512.0");
        if ( v22 )
          goto LABEL_46;
LABEL_90:
        snprintf((char *)v25, 0x200u, "Out_Tex0 = %s;", v24);
        strcat(byte_619294, (const char *)v25);
        goto LABEL_47;
      }
      strcpy(v24, "TexCoord0");
    }
    if ( v22 )
    {
LABEL_46:
      snprintf((char *)v25, 0x200u, "Out_Tex0 = (NormalMatrix * vec3(%s, 1.0)).xy;", v24);
      strcat(byte_619294, (const char *)v25);
      goto LABEL_47;
    }
    goto LABEL_90;
  }
LABEL_47:
  if ( v10 )
  {
    strcpy((char *)v25, "vec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "reflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;");
    strcat(byte_619294, (const char *)v25);
    if ( (flags & 0x1000000) != 0 )
      strcpy((char *)v25, "Out_Refl = reflVector;");
    else
      strcpy((char *)v25, "Out_Tex1 = vec2(length(reflVector.xy), (reflVector.z * 0.5) + 0.25);");
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & 0x100000) != 0 )
  {
    strcpy((char *)v25, "lowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);");
    strcat(byte_619294, (const char *)v25);
    strcpy(v24, "InterpColor");
    if ( (flags & 2) == 0 )
      goto LABEL_50;
  }
  else
  {
    strcpy(v24, "GlobalColor");
    if ( (flags & 2) == 0 )
    {
LABEL_50:
      if ( (flags & 0x12) != 0 )
      {
        snprintf((char *)v25, 0x200u, "Out_Color = %s;", v24);
        strcat(byte_619294, (const char *)v25);
      }
      goto LABEL_52;
    }
  }
  strcpy((char *)v25, "vec3 Out_LightingColor;");
  strcat(byte_619294, (const char *)v25);
  if ( (flags & 8) != 0 )
  {
    if ( (flags & 0x200) != 0 )
      strcpy((char *)v25, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz * 1.5;");
    else
      snprintf((char *)v25, 0x200u, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + %s.xyz;", v24);
    strcat(byte_619294, (const char *)v25);
  }
  else
  {
    strcpy((char *)v25, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + MaterialEmissive.xyz;");
    strcat(byte_619294, (const char *)v25);
  }
  if ( (flags & 0xE000) != 0 )
  {
    if ( v23 )
    {
      if ( (flags & 0x1180) != 0 )
      {
        strcpy(
          (char *)v25,
          "Out_LightingColor += (max(dot(DirLightDirection, WorldNormal), 0.0) + max(dot(DirBackLightDirection, WorldNorm"
          "al), 0.0)) * DirLightDiffuseColor;");
        strcat(byte_619294, (const char *)v25);
      }
      else
      {
        strcpy(
          (char *)v25,
          "Out_LightingColor += max(dot(DirLightDirection, WorldNormal), 0.0) * DirLightDiffuseColor;");
        strcat(byte_619294, (const char *)v25);
      }
    }
    if ( (flags & 0x4000) != 0 )
    {
      strcpy(
        (char *)v25,
        "Out_LightingColor += max(dot(DirLight2Direction, WorldNormal), 0.0) * DirLight2DiffuseColor;");
      strcat(byte_619294, (const char *)v25);
    }
    if ( (flags & 0x8000) != 0 )
    {
      strcpy(
        (char *)v25,
        "Out_LightingColor += max(dot(DirLight3Direction, WorldNormal), 0.0) * DirLight3DiffuseColor;");
      strcat(byte_619294, (const char *)v25);
    }
  }
  if ( (flags & 0x12) != 0 )
  {
    if ( (flags & 0x10) != 0 )
      snprintf(
        (char *)v25,
        0x200u,
        "Out_Color = vec4((Out_LightingColor.xyz + %s.xyz * 1.5) * MaterialDiffuse.xyz, (MaterialAmbient.w) * %s.w);",
        v24,
        v24);
    else
      snprintf(
        (char *)v25,
        0x200u,
        "Out_Color = vec4(Out_LightingColor * MaterialDiffuse.xyz, MaterialAmbient.w * %s.w);",
        v24);
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "Out_Color = clamp(Out_Color, 0.0, 1.0);");
    strcat(byte_619294, (const char *)v25);
  }
LABEL_52:
  if ( *byte_617258 >= (unsigned int)(v23 != 0) )
    goto LABEL_56;
  if ( v10 )
  {
    v20 = "float specAmt = max(pow(dot(reflVector, DirLightDirection), %.1f), 0.0) * EnvMapCoefficient * 2.0;";
LABEL_55:
    snprintf((char *)v25, 0x200u, "%s", v20);
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "Out_Spec = specAmt * DirLightDiffuseColor;");
    strcat(byte_619294, (const char *)v25);
    goto LABEL_56;
  }
  if ( (flags & 0x180) != 0 )
  {
    strcpy((char *)v25, "vec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "reflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;");
    strcat(byte_619294, (const char *)v25);
    v20 = "float specAmt = max(pow(dot(reflVector, DirLightDirection), %.1f), 0.0) * 0.125;";
    goto LABEL_55;
  }
LABEL_56:
  if ( (flags & FLAG_WATER) != 0 )
  {
    strcpy((char *)v25, "Out_WaterDetail = (Out_Tex0 * 4.0) + vec2(WaterSpecs.x * -0.3, WaterSpecs.x * 0.21);");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "Out_WaterDetail2 = (Out_Tex0 * -8.0) + vec2(WaterSpecs.x * 0.12, WaterSpecs.x * -0.05);");
    strcat(byte_619294, (const char *)v25);
    strcpy((char *)v25, "Out_WaterAlphaBlend = distance(WorldPos.xy, CameraPosition.xy) * WaterSpecs.y;");
    strcat(byte_619294, (const char *)v25);
  }
  strcpy((char *)v25, "}");
  return strcat(byte_619294, (const char *)v25);*/

  int v2; // r8
  int v3; // r9
  int v4; // r0
  int v5; // r11
  size_t v6; // r0
  const char *v7; // r2
  const char *v8; // r3
  const char *v9; // r8
  int v10; // r0
  int v12; // [sp+18h] [bp-238h]
  char s[512]; // [sp+30h] [bp-220h] BYREF
  int v14; // [sp+230h] [bp-20h]

  byte_619294[0] = 0;
  snprintf(s, 0x200u, "#version 100\n");
  strcat(byte_619294, s);
  
  snprintf(s, 0x200u, "precision highp float;");
  strcat(byte_619294, s);
  
  snprintf(s, 0x200u, "uniform mat4 ProjMatrix;");
  strcat(byte_619294, s);
  
  snprintf(s, 0x200u, "uniform mat4 ViewMatrix;");
  strcat(byte_619294, s);
  
  snprintf(s, 0x200u, "uniform mat4 ObjMatrix;");
  strcat(byte_619294, s);
  
  if ( (flags & 2) != 0 )
  {
    snprintf(s, 0x200u, "uniform lowp vec3 AmbientLightColor;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "uniform lowp vec4 MaterialEmissive;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "uniform lowp vec4 MaterialAmbient;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "uniform lowp vec4 MaterialDiffuse;");
    strcat(byte_619294, s);
    
    if ( (flags & 0x2000) != 0 )
    {
      snprintf(s, 0x200u, "uniform lowp vec3 DirLightDiffuseColor;");
      strcat(byte_619294, s);
      
      snprintf(s, 0x200u, "uniform vec3 DirLightDirection;");
      strcat(byte_619294, s);
      
      if ( (flags & 0x1180) != 0 && *(int *)(g_libGTASA + 9361520) == 3 )
      {
        snprintf(s, 0x200u, "uniform vec3 DirBackLightDirection;");
        strcat(byte_619294, s);
        
      }
    }
    if ( (flags & 0x4000) != 0 )
    {
      snprintf(s, 0x200u, "uniform lowp vec3 DirLight2DiffuseColor;");
      strcat(byte_619294, s);
      
      snprintf(s, 0x200u, "uniform vec3 DirLight2Direction;");
      strcat(byte_619294, s);
      
    }
    if ( (flags & 0x8000) != 0 )
    {
      snprintf(s, 0x200u, "uniform lowp vec3 DirLight3DiffuseColor;");
      strcat(byte_619294, s);
      
      snprintf(s, 0x200u, "uniform vec3 DirLight3Direction;");
      strcat(byte_619294, s);
      
    }
  }
  snprintf(s, 0x200u, "attribute vec3 Position;");
  strcat(byte_619294, s);
  
  snprintf(s, 0x200u, "attribute vec3 Normal;");
  strcat(byte_619294, s);
  
  if ( (flags & 0x20) != 0 )
  {
    if ( (flags & 0x40000) != 0 )
      snprintf(s, 0x200u, "attribute vec4 TexCoord0;");
    else
      snprintf(s, 0x200u, "attribute vec2 TexCoord0;");
    strcat(byte_619294, s);
    
  }
  snprintf(s, 0x200u, "attribute vec4 GlobalColor;");
  strcat(byte_619294, s);
  
  if ( (flags & 0x180) != 0 )
  {
    snprintf(s, 0x200u, "attribute vec4 BoneWeight;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "attribute vec4 BoneIndices;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "uniform highp vec4 Bones[%d];", 3 * *(int *)(g_libGTASA + 6386248));
    strcat(byte_619294, s);
    
  }
  if ( (flags & 0x20) != 0 )
  {
    snprintf(s, 0x200u, "varying mediump vec2 Out_Tex0;");
    strcat(byte_619294, s);
    
  }
  if ( (flags & 0x2000000) != 0 )
  {
    snprintf(s, 0x200u, "uniform mat3 NormalMatrix;");
    strcat(byte_619294, s);
    
  }
  v2 = flags & 0x1000040;
  if ( (flags & 0x1000040) != 0 )
  {
    snprintf(s, 0x200u, "uniform lowp float EnvMapCoefficient;");
    strcat(byte_619294, s);
    
    if ( (flags & 0x40) != 0 )
      snprintf(s, 0x200u, "varying mediump vec2 Out_Tex1;");
    else
      snprintf(s, 0x200u, "varying mediump vec3 Out_Refl;");
    strcat(byte_619294, s);
    
  }
  if ( (flags & 0x9880640) != 0 )
  {
    snprintf(s, 0x200u, "uniform vec3 CameraPosition;");
    strcat(byte_619294, s);
    
  }
  if ( (flags & 0x400) != 0 )
  {
    snprintf(s, 0x200u, "varying mediump float Out_FogAmt;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "uniform vec3 FogDistances;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "varying mediump vec4 Out_ClipSpace;");
    strcat(byte_619294, s);
    
  }
  if ( (flags & FLAG_WATER) != 0 )
  {
    snprintf(s, 0x200u, "uniform vec3 WaterSpecs;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "varying mediump vec2 Out_WaterDetail;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "varying mediump vec2 Out_WaterDetail2;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "varying mediump float Out_WaterAlphaBlend;");
    strcat(byte_619294, s);
    
  }
  if ( (flags & 0x100000) != 0 )
  {
    snprintf(s, 0x200u, "attribute vec4 Color2;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "uniform lowp float ColorInterp;");
    strcat(byte_619294, s);
    
  }
  if ( (flags & 0x12) != 0 )
  {
    snprintf(s, 0x200u, "varying lowp vec4 Out_Color;");
    strcat(byte_619294, s);
    
  }
  v3 = v2 == 0;
  v4 = 0;
  v5 = flags & 0x1000040;
  if ( (flags & 0x2000) == 0 )
    v4 = 1;
  v12 = v4;
  if ( (flags & 0x2000) != 0 && v2 )
  {
    snprintf(s, 0x200u, "varying lowp vec3 Out_Spec;");
    strcat(byte_619294, s);
    
  }
  snprintf(s, 0x200u, "void main() {");
  strcat(byte_619294, s);
  
  if ( (flags & 0x180) != 0 )
  {
    snprintf(s, 0x200u, "ivec4 BlendIndexArray = ivec4(BoneIndices);");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "mat4 BoneToLocal;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[0] = Bones[BlendIndexArray.x*3] * BoneWeight.x;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[1] = Bones[BlendIndexArray.x*3+1] * BoneWeight.x;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[2] = Bones[BlendIndexArray.x*3+2] * BoneWeight.x;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[3] = vec4(0.0,0.0,0.0,1.0);");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.y*3] * BoneWeight.y;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.y*3+1] * BoneWeight.y;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.y*3+2] * BoneWeight.y;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.z*3] * BoneWeight.z;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.z*3+1] * BoneWeight.z;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.z*3+2] * BoneWeight.z;");
    strcat(byte_619294, s);
    
    if ( (flags & 0x100) != 0 )
    {
      snprintf(s, 0x200u, "BoneToLocal[0] += Bones[BlendIndexArray.w*3] * BoneWeight.w;");
      strcat(byte_619294, s);
      
      snprintf(s, 0x200u, "BoneToLocal[1] += Bones[BlendIndexArray.w*3+1] * BoneWeight.w;");
      strcat(byte_619294, s);
      
      snprintf(s, 0x200u, "BoneToLocal[2] += Bones[BlendIndexArray.w*3+2] * BoneWeight.w;");
      strcat(byte_619294, s);
      
    }
    snprintf(s, 0x200u, "vec4 WorldPos = ObjMatrix * (vec4(Position,1.0) * BoneToLocal);");
  }
  else
  {
    snprintf(s, 0x200u, "vec4 WorldPos = ObjMatrix * vec4(Position,1.0);");
  }
  strcat(byte_619294, s);
  
  if ( (flags & 0x8000000) != 0 )
  {
    snprintf(s, 0x200u, "vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "float coefOffset = 1.0 - clamp(length(ReflVector.xy) / 45.0, 0.0, 1.0);");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "coefOffset = -1000.0 * coefOffset;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "vec3 ReflPos = normalize(ReflVector);");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
    strcat(byte_619294, s);
    v6 = strlen(byte_619294);
    v7 = "gl_Position = vec4(ReflPos.xy, coefOffset + length(ReflVector) * 0.002, 1.0);";
LABEL_52:
    snprintf(s, 0x200u, "%s", v7);
    goto LABEL_53;
  }
  if ( (flags & 0x800000) != 0 )
  {
    snprintf(s, 0x200u, "vec3 ReflVector = WorldPos.xyz - CameraPosition.xyz;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "vec3 ReflPos = normalize(ReflVector);");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "ReflPos.xy = normalize(ReflPos.xy) * (ReflPos.z * 0.5 + 0.5);");
    strcat(byte_619294, s);
    v6 = strlen(byte_619294);
    v7 = "gl_Position = vec4(ReflPos.xy, length(ReflVector) * 0.002, 1.0);";
    goto LABEL_52;
  }
  snprintf(s, 0x200u, "vec4 ViewPos = ViewMatrix * WorldPos;");
  strcat(byte_619294, s);
  
  snprintf(s, 0x200u, "gl_Position = ProjMatrix * ViewPos;");
  strcat(byte_619294, s);
  
  if ( (flags & 0x400) == 0 )
    goto LABEL_54;
  snprintf(s, 0x200u, "Out_ClipSpace = gl_Position;");
LABEL_53:
  strcat(byte_619294, s);
  
LABEL_54:
  if ( (flags & 2) != 0 )
  {
    if ( (flags & 0x201) == 513 && (flags & 0xE000) != 0 )
    {
      snprintf(s, 0x200u, "vec3 WorldNormal = normalize(vec3(WorldPos.xy - CameraPosition.xy, 0.0001)) * 0.85;");
    }
    else if ( (flags & 0x180) != 0 )
    {
      snprintf(s, 0x200u, "vec3 WorldNormal = mat3(ObjMatrix) * (Normal * mat3(BoneToLocal));");
    }
    else
    {
      snprintf(s, 0x200u, "vec3 WorldNormal = (ObjMatrix * vec4(Normal,0.0)).xyz;");
    }
    goto LABEL_63;
  }
  if ( v5 )
  {
    snprintf(s, 0x200u, "vec3 WorldNormal = vec3(0.0, 0.0, 0.0);");
LABEL_63:
    strcat(byte_619294, s);
    
  }
  if ( !*(char *)(RQCaps + 8) && (flags & 0x400) != 0 )
  {
    snprintf(
      s,
      0x200u,
      "Out_FogAmt = clamp((length(WorldPos.xyz - CameraPosition.xyz) - FogDistances.x) * FogDistances.z, 0.0, 1.0);");
    strcat(byte_619294, s);
    
  }
  if ( (flags & 0x20) != 0 )
  {
    if ( (flags & 0x40000) != 0 )
    {
      v8 = "TexCoord0.xy / TexCoord0.w";
    }
    else
    {
      v8 = "TexCoord0";
      if ( (flags & 0x20000) != 0 )
        v8 = "TexCoord0 / 512.0";
    }
    if ( (flags & 0x2000000) != 0 )
      snprintf(s, 0x200u, "Out_Tex0 = (NormalMatrix * vec3(%s, 1.0)).xy;", v8);
    else
      snprintf(s, 0x200u, "Out_Tex0 = %s;", v8);
    strcat(byte_619294, s);
    
  }
  if ( v5 )
  {
    snprintf(s, 0x200u, "vec3 reflVector = normalize(WorldPos.xyz - CameraPosition.xyz);");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "reflVector = reflVector - 2.0 * dot(reflVector, WorldNormal) * WorldNormal;");
    strcat(byte_619294, s);
    
    if ( (flags & 0x1000000) != 0 )
      snprintf(s, 0x200u, "Out_Refl = reflVector;");
    else
      snprintf(s, 0x200u, "Out_Tex1 = vec2(length(reflVector.xy), (reflVector.z * 0.5) + 0.25);");
    strcat(byte_619294, s);
    
  }
  if ( (flags & 0x100000) != 0 )
  {
    snprintf(s, 0x200u, "lowp vec4 InterpColor = mix(GlobalColor, Color2, ColorInterp);");
    strcat(byte_619294, s);
    
    v9 = "InterpColor";
  }
  else
  {
    v9 = "GlobalColor";
  }
  if ( (flags & 2) != 0 )
  {
    snprintf(s, 0x200u, "vec3 Out_LightingColor;");
    strcat(byte_619294, s);
    
    if ( (flags & 8) != 0 )
    {
      if ( (flags & 0x200) != 0 )
        snprintf(s, 0x200u, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz * 1.5;");
      else
        snprintf(s, 0x200u, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + %s.xyz;", v9);
    }
    else
    {
      snprintf(s, 0x200u, "Out_LightingColor = AmbientLightColor * MaterialAmbient.xyz + MaterialEmissive.xyz;");
    }
    strcat(byte_619294, s);
    
    if ( (flags & 0xE000) != 0 )
    {
      if ( (flags & 0x2000) != 0 )
      {
        if ( (flags & 0x1180) != 0 && *(int*)(g_libGTASA + 9361520) == 3 )
          snprintf(
            s,
            0x200u,
            "Out_LightingColor += (max(dot(DirLightDirection, WorldNormal), 0.0) + max(dot(DirBackLightDirection, WorldNo"
            "rmal), 0.0)) * DirLightDiffuseColor;");
        else
          snprintf(
            s,
            0x200u,
            "Out_LightingColor += max(dot(DirLightDirection, WorldNormal), 0.0) * DirLightDiffuseColor;");
        strcat(byte_619294, s);
        
        if ( (flags & 0x4000) == 0 )
        {
LABEL_96:
          if ( (flags & 0x8000) == 0 )
            goto LABEL_98;
          goto LABEL_97;
        }
      }
      else if ( (flags & 0x4000) == 0 )
      {
        goto LABEL_96;
      }
      snprintf(
        s,
        0x200u,
        "Out_LightingColor += max(dot(DirLight2Direction, WorldNormal), 0.0) * DirLight2DiffuseColor;");
      strcat(byte_619294, s);
      
      if ( (flags & 0x8000) != 0 )
      {
LABEL_97:
        snprintf(
          s,
          0x200u,
          "Out_LightingColor += max(dot(DirLight3Direction, WorldNormal), 0.0) * DirLight3DiffuseColor;");
        strcat(byte_619294, s);
        
        goto LABEL_98;
      }
    }
LABEL_98:
    if ( (flags & 0x12) != 0 )
    {
      if ( (flags & 0x10) != 0 )
        snprintf(
          s,
          0x200u,
          "Out_Color = vec4((Out_LightingColor.xyz + %s.xyz * 1.5) * MaterialDiffuse.xyz, (MaterialAmbient.w) * %s.w);",
          v9,
          v9);
      else
        snprintf(s, 0x200u, "Out_Color = vec4(Out_LightingColor * MaterialDiffuse.xyz, MaterialAmbient.w * %s.w);", v9);
      strcat(byte_619294, s);
      
      snprintf(s, 0x200u, "Out_Color = clamp(Out_Color, 0.0, 1.0);");
      strcat(byte_619294, s);
      
    }
    goto LABEL_106;
  }
  if ( (flags & 0x12) != 0 )
  {
    snprintf(s, 0x200u, "Out_Color = %s;", v9);
    strcat(byte_619294, s);
    
  }
LABEL_106:
  v10 = *(uint8_t *)(g_libGTASA + 6386264);
  if ( *(uint8_t *)(g_libGTASA + 6386264) )
    v10 = 1;
  if ( !(v10 | v12 | v3) )
  {
    snprintf(
      s,
      0x200u,
      "float specAmt = max(pow(dot(reflVector, DirLightDirection), %.1f), 0.0) * EnvMapCoefficient * 1.3;");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "Out_Spec = specAmt * DirLightDiffuseColor;");
    strcat(byte_619294, s);
    
  }
  if ( (flags & FLAG_WATER) != 0 )
  {
    snprintf(s, 0x200u, "Out_WaterDetail = (Out_Tex0 * 4.0) + vec2(WaterSpecs.x * -0.3, WaterSpecs.x * 0.21);");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "Out_WaterDetail2 = (Out_Tex0 * -8.0) + vec2(WaterSpecs.x * 0.12, WaterSpecs.x * -0.05);");
    strcat(byte_619294, s);
    
    snprintf(s, 0x200u, "Out_WaterAlphaBlend = distance(WorldPos.xy, CameraPosition.xy) * WaterSpecs.y;");
    strcat(byte_619294, s);
    
  }
  snprintf(s, 0x200u, (const char *)"}");
  strcat(byte_619294, s);
  
  //Log("build vertex shader");
  //Log(byte_619294);
  //Log("end build vertex shader");
}

/*int (*RQShader__BuildSource)(int flags, char **pxlsrc, char **vtxsrc);
int RQShader__BuildSource_hook(int flags, char **pxlsrc, char **vtxsrc) {
    pxlbuf[0] = '\0';
    vtxbuf[0] = '\0';


    BuildPixelSource(flags);

    //BuildVertexSource(flags);
    CPatch::CallFunction<void>(g_libGTASA + 0x1A4AB4 + 1, flags);
  

    *pxlsrc = pxlbuf;
    //*vtxsrc = vtxbuf;

    return 1;
}*/


void SetUpGLHooks()
{
	byte_617258 = (char*)(g_libGTASA + 0x617258);
  byte_619294 = (char*)(g_libGTASA + 0x619294);
  byte_619294[0] = '\0';
	//CPatch::InlineHook(g_libGTASA, 0x1A5EB0, &RQShader__BuildSource_hook, &RQShader__BuildSource);
  byte_617290 = (char*)(g_libGTASA + 0x617290);
	byte_617258 = (char*)(g_libGTASA + 0x617258);

  RQMaxBones = (int*)(g_libGTASA + 0x617248);

  RQCaps = (RQCapabilities *)g_libGTASA + 0x617250;

	CPatch::InlineHook(g_libGTASA, 0x1A40B4, &BuildPixelSource_hook, &BuildPixelSource);
  CPatch::InlineHook(g_libGTASA, 0x1A4AB4, &BuildVertexSource_hook, &BuildVertexSource);
}