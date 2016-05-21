#include "Model.h"

std::unordered_map<std::string, Texture> Model::mModelTextures;

Model::Model()
{
 
}

Model::~Model()
{

}

bool Model::init(GLuint program, const std::string& fileName)
{

    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll 
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(fileName,
      //  aiProcess_GenNormals             |
      //  aiProcess_GenUVCoords            |
      //  aiProcess_CalcTangentSpace       | 
        aiProcess_FlipUVs                |
        aiProcess_Triangulate            
      //  aiProcess_OptimizeMeshes         |
      //  aiProcess_OptimizeGraph          |
      //  aiProcess_JoinIdenticalVertices  |
      //  aiProcess_SortByPType
        );
    
  
    if(!scene)
    {
        std::cout << "Error opening scene: "<< importer.GetErrorString() << std::endl;
        return false;
    }

    for(size_t i=0;i<scene->mNumMeshes;++i)
    {
        std::vector<Vertex>* vertices = new std::vector<Vertex>();
        std::vector<unsigned int>* indices = new std::vector<unsigned int>();
        std::vector<Texture>* textures = new std::vector<Texture>();
        //std::vector<Texture>& textures = &texturesPtr;
        aiMesh* assimpMesh = scene->mMeshes[i];  
                
        for(size_t j=0;j<assimpMesh->mNumVertices;++j)
        {
            
            aiVector3D& assimpVector = assimpMesh->mVertices[i];
            aiVector3D& assimpNormal = assimpMesh->mNormals[i];

            Vertex vertex;
            vertex.position = glm::vec3(assimpVector.x, assimpVector.y, assimpVector.z);
            vertex.normal = glm::vec3(assimpNormal.x, assimpNormal.y, assimpNormal.z);            
            
            if(assimpMesh->HasTextureCoords(i))
            {
                aiVector3D& assimpTexture = assimpMesh->mTextureCoords[0][i];
                vertex.texCoord = glm::vec2(assimpTexture.x, assimpTexture.y);
            } 
            else 
            {
                vertex.texCoord = glm::vec2(0.0f, 0.0f);
            }

            vertices->push_back(vertex); 
        }

        for(size_t j=0;j<assimpMesh->mNumFaces;++j)
        {
            aiFace& face = assimpMesh->mFaces[j];
            for(size_t k=0;k<face.mNumIndices;++k)
            {
                indices->push_back(face.mIndices[i]);
            }
        }

        aiMaterial* material = scene->mMaterials[assimpMesh->mMaterialIndex];
        addTextures(*textures, material, aiTextureType_DIFFUSE, fileName);
       // std::cout << "n i " << indices.size() << " n v " << vertices.size() << std::endl; 
        Mesh mesh(program,*vertices,*indices, *textures);

        mMeshes.push_back(mesh);
    }

    return true;
}


void Model::update(const glm::mat4& model, float dt)
{
    mModelMatrix = model; 
}

void Model::draw(const glm::mat4& viewProj)
{
    for(size_t i=0;i<mMeshes.size();++i)
    {
        mMeshes[i].draw(mModelMatrix, viewProj);
    }
}

void Model::addTextures(std::vector<Texture>& in, aiMaterial* material, aiTextureType textureType, const std::string& fileName)
{
    for(size_t i=0;i<material->GetTextureCount(textureType);++i)
    {
        aiString file;
        material->GetTexture(textureType, i, &file);
        std::string fileStr(file.C_Str());

        std::unordered_map<std::string, Texture>::iterator it = mModelTextures.find(fileStr);
        if(it != mModelTextures.end())
        {
           in.push_back(it->second);  
        }
        else
        {
            Texture texture;
            texture.textureType = textureType; 
           
            std::string folder = getFolderFromFileName(fileName);

            std::string textureFileName = folder + "/" + file.C_Str();

            std::cout << "tex file name " << textureFileName << " "<< fileName << " " << folder  << std::endl;

            texture.textureHandle = getTextureHandle(textureFileName);
            
            mModelTextures.insert(std::pair<std::string, Texture>(fileStr, texture));
            in.push_back(texture);
        }
    }
}

GLuint Model::getTextureHandle(const std::string& textureFileName)
{
    GLuint handle;
    glGenTextures(1, &handle);
    int w, h;
    unsigned char* img = SOIL_load_image(textureFileName.c_str(), &w, &h, 0, SOIL_LOAD_RGB);
    
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0); 

    SOIL_free_image_data(img); 
    
    return handle;
}

std::string Model::getFolderFromFileName(const std::string& fileName)
{
   size_t last_slash = fileName.find_last_of("\\/");
   return last_slash == std::string::npos ? "" : fileName.substr(0, last_slash);
}

