#ifndef ALM_MESH_TECHNIQUE
#define ALM_MESH_TECHNIQUE

#include "Technique.h"

class MeshTechnique : public Technique
{
public:
    MeshTechnique();

protected:
    

    static const char * const mTextureTypeNames;
    static const aiTextureType mTextureTypes;
    static const size_t mNumTextureTypes;

};

inline const char * const MeshTechnique::mTextureTypeNames[] = {
    "", "u_TextureDiffuse", "u_TextureSpecular", "u_TextureAmbient",
    "u_TextureEmissive", "u_TextureHeight", "u_TextureNormals", 
    "u_TextureShininess", "u_TextureOpacity", "u_TextureDisplacement",
    "u_TextureLightMap", "u_TextureReflection", "" 
};

inline const size_t MeshTechnique::mNumTextureTypes = sizeof(Technique::mNumTextureTypeNames)/sizeof(Technique::mNumTextureTypeNames[0]);

inline const char* MeshTechnique::getTextureName(aiTextureType t) const
{
    return mTextureTypes[t];
}



#endif
