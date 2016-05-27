#include "Model.h"

Model::Model(const std::string& fileName, 
      const std::string& texturesDir,          
      bool replaceTexturePath/* = false */,  
      int aiProcessArgs/* = 0 */, 
      bool gamma/* = false */
     ): Model(fileName, texturesDir, replaceTexturePath, aiProcessArgs, gamma)
{
}

Model::Model(
    const std::string& fileName, 
    const char* texturesDir/* = NULL */,          
    bool replaceTexturePath/* = false */,  
    int aiProcessArgs/* = 0 */, 
    bool gamma/* = false */
    ) : mFileName(fileName), mTexturesDir(texturesDir ? texturesDir : FileSystem::getInstance().getPathFromFileName(fileName) )
    , mAiProcessArgs(aiProcessArgs), gammaCorrection(gamma), mReplaceTexturePath(replaceTexturePath)
{
    LOG("MODEL_CREATE");  
}

Model::~Model()
{
    unload();
}

bool Model::init()
{   

    LOG("MODEL INIT");
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(mFileName, 
            mAiProcessArgs ? mAiProcessArgs :
              aiProcess_Triangulate 
         //   | aiProcess_FlipUVs 
            | aiProcess_CalcTangentSpace
            );

    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }

    if(!scene->HasMeshes())
    {
        LOG("ERROR: NO MESHES FOUND");
        return false;
    }
    
    size_t numVertices, numIndices;
    countMeshData(scene, numVertices, numIndices);

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    
    vertices.reserve(numVertices);
    indices.reserve(numIndices);
    mMaterials.resize(scene->mNumMaterials);

    processNode(scene, scene->mRootNode, vertices, indices);
    
    initBuffers(vertices, indices);

    return true;
}

void Model::draw(ModelTechnique& tech)
{
    glBindVertexArray(mVAO);

    for(GLuint i = 0; i < mMeshes.size(); i++)
    {
        mMeshes[i].draw(mMaterials, tech);
    }
    glBindVertexArray(0);
}


Mesh Model::processMesh(const aiScene* scene, aiMesh* mesh, 
         std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
    LOG("MODEL PROCESS MESH");

    size_t baseVertex = vertices.size();
    size_t baseIndex  = indices.size();


    for(GLuint i = 0; i < mesh->mNumVertices; i++)
    {

        Vertex vertex;

        if(mesh->HasPositions()) 
        {
            setVector(mesh->mVertices[i], vertex.Position);
        }

        if(mesh->HasNormals()) 
        {
            setVector(mesh->mNormals[i], vertex.Normal);
        }

        if(mesh->HasTangentsAndBitangents()) 
        {
            setVector(mesh->mTangents[i], vertex.Tangent);
            setVector(mesh->mBitangents[i], vertex.Bitangent);
        }

        if(mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        
        vertices.push_back(vertex);
       
    }

    //process indices
    for(GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace& face = mesh->mFaces[i];

        for(GLuint j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
       
    }

    if(mesh->mMaterialIndex >= 0)
    {

        
        std::vector<Texture>& textures = mMaterials[mesh->mMaterialIndex];


        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiTextureType texTypes[] = {
            aiTextureType_DIFFUSE,
            aiTextureType_SPECULAR,
            aiTextureType_HEIGHT,
            aiTextureType_AMBIENT
        };

        size_t n = sizeof(texTypes)/sizeof(texTypes[0]);
        for(size_t i=0;i<n;++i)
        {
            loadMaterialTextures(textures, material, texTypes[i]);
        }
        
    }
    
    size_t numIndices = indices.size() - baseIndex; 
    


    Mesh ret(
            numIndices,
            mesh->mMaterialIndex,
            baseVertex,
            baseIndex);

    return ret;
}


void Model::countMeshData(const aiScene* scene, size_t& outNumVertices, size_t& outNumIndices)
{
    outNumVertices = 0;
    outNumIndices  = 0;

    if(!scene)
    {
        LOG("ERROR:: SCENE IS NULL");
        return;
    }

    for(size_t i=0;i<scene->mNumMeshes;++i)
    {
        aiMesh* m = scene->mMeshes[i];
        outNumVertices += m->mNumVertices;   
        outNumIndices += m->mNumFaces;
    }
    outNumIndices *= 3;
    LOG("OUT VERT:" << outNumVertices << " OUT IND:" <<outNumIndices);
}

void Model::unload()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}



void Model::processNode(const aiScene* scene, aiNode* node,
        std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
    for(size_t i=0;i<node->mNumMeshes;++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        mMeshes.push_back(processMesh(scene, mesh, vertices, indices));
    }


    for(size_t i=0;i<node->mNumChildren;++i)
    {
        processNode(scene, node->mChildren[i], vertices, indices);
    }
}


void Model::loadMaterialTextures(std::vector<Texture>& textures, aiMaterial* mat, aiTextureType type)
{
    TextureManager& textureManager = TextureManager::getInstance();
    FileSystem& fs = FileSystem::getInstance();

    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString relativeTexFilePath;
        mat->GetTexture(type, i, &relativeTexFilePath);
        Texture texture;
        LOG("REL P:" << relativeTexFilePath.C_Str());
        LOG("NMAT:" << mat->GetTextureCount(type));
        texture.id = textureManager.load(mTexturesDir.c_str(),  mReplaceTexturePath ? 
                                                                fs.getReplacedTexturePathFileName(relativeTexFilePath.C_Str(), "")
                                                                : relativeTexFilePath.C_Str() ); 
        texture.type = type;
        textures.push_back(texture);
    } 
}

void Model::initBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    initVertices(vertices);
    initIndices(indices);

    glBindVertexArray(0);

}

void Model::initVertices(const std::vector<Vertex>& vertices)
{
    glGenBuffers(1, &mVBO);
    
    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));
}

void Model::initIndices(const std::vector<GLuint>& indices)
{
    glGenBuffers(1, &mEBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Model::setVector(const aiVector3D& src, glm::vec3& dest)
{
    dest.x = src.x;
    dest.y = src.y; 
    dest.z = src.z;
}





