#include "heightmap.h"

const size_t HeightMap::MAX_LAYERS = 8;

HeightMap::HeightMap(): mNumIndices(0), mWidth(0), mHeight(0)
{

}

HeightMap::~HeightMap()
{

}

void HeightMap::init(
            const std::string& heightMapPath, 
            const std::vector<Layer>& layers,
            const std::vector<BlendedTexture>* blendedTextures,
            unsigned int flags /* = HeightMapFlag_GEN_NORMALS | HeightMapFlag_USE_ABS_PATH */)
{
    bool useAbsPath = (flags & HeightMapFlag_USE_ABS_PATH) == HeightMapFlag_USE_ABS_PATH;
    bool genNormals = (flags & HeightMapFlag_GEN_NORMALS) == HeightMapFlag_GEN_NORMALS;
    bool genTangentsAndBitangents = false;

    if(genNormals)
    {
        genTangentsAndBitangents = (flags & HeightMapFlag_GEN_TANGENTS_AND_BINORMALS) == HeightMapFlag_GEN_TANGENTS_AND_BINORMALS; 
    }

    initBuffers(heightMapPath, useAbsPath, genNormals, genTangentsAndBitangents);
    initLayers(heightMapPath, layers, useAbsPath); 

    if(blendedTextures)
    {
        initBlendedTextures(*blendedTextures, useAbsPath); 
    }
}


void HeightMap::initBuffers(
            const std::string& heightMapPath,
            bool useAbsPath, 
            bool genNormals /* = true */,
            bool genTangentsAndBinormals /* = false */)
{
    std::string path = useAbsPath ? FileSystem::getInstance().getAbsPath(heightMapPath) : heightMapPath;
    
    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec3> tangents, binormals;
    std::vector<GLuint> indices;

    initVertices(vertices, heightMapPath);
    initIndices(indices);

    mNumIndices = indices.size();

    size_t numAttrs = 1;
        
    if(genNormals)
    {
        numAttrs += 1;
        initNormals(normals, vertices);
        
        if(genTangentsAndBinormals)
        {
            numAttrs += 2;
            initTangensAndBinormals(tangents, binormals, vertices, normals);
        }

        std::vector<glm::vec3> result;
        result.reserve(numAttrs * vertices.size());
        
        for(size_t i=0;i<vertices.size();++i)
        {
            result.push_back(vertices[i]);
            result.push_back(normals[i]);

            if(genTangentsAndBinormals)
            {
                result.push_back(tangents[i]);
                result.push_back(binormals[i]);
            }
        }

        vertices = result;
    } 

    GLsizei stride = numAttrs * sizeof(glm::vec3);

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    if(genNormals)
    {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        if(genTangentsAndBinormals)
        {
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(6*sizeof(float)));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(9*sizeof(float)));
            glEnableVertexAttribArray(3);
        }
    }
   
    glBindVertexArray(0);
} 


void HeightMap::initVertices(
            std::vector<glm::vec3>& vertices, 
            const std::string& heightMapPath
        )
{
    ImageData id;
    TextureManager& tm = TextureManager::getInstance();
    tm.outImageData(heightMapPath.c_str(), id);

    mWidth = id.w;
    mHeight = id.h;

    float scale = 1.0f/255.0f;

    for(int i=0;i<id.w;++i)
    {
        for(int j=0;j<id.h;++j)
        {
            // x [-0.5, 0.5], y [0, 1], z [-0.5, 0.5]
            float x = (float) i / id.w - 0.5f;
            float z = (float) j / id.h - 0.5f;
            float y = scale * ((float) id.img[i * id.w + j]);

            vertices.push_back(glm::vec3(x, y, z));

         //   LOG("xyz:" << x << " " << y << " " << z);
        } 
    }

    tm.freeImage(id);
}


void HeightMap::initIndices(std::vector<GLuint>& indices)
{
    size_t w = mWidth;
    size_t h = mHeight;
    
    for(size_t i=0;i<w-1;++i)
    {
        for(size_t j=0;j<h;++j)
        {
            indices.push_back( i      * w + j);
            indices.push_back((i + 1) * w + j);
        }
        
        indices.push_back((i + 1) * w + h - 1);
        indices.push_back((i + 1) * w);
    } 
 
    size_t i = w - 1;

    for(size_t j=0;j<h;++j)
    {
        indices.push_back( i      * w + j);
        indices.push_back((i + 1) * w + j);
    }
}

void HeightMap::initNormals(std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& vertices)
{  
    int w = (int) mWidth;
    int h = (int) mHeight;

    for(int i=0;i<w;++i)
    {
        for(int j=0;j<h;++j)
        {
            bool left   = j > 0;
            bool top    = i > 0;
            bool right  = j < h - 1;
            bool bottom = i < w - 1;

            const glm::vec3 *const sides[] = {
                left           ? &vertices[ i      * w + j - 1] : NULL, 
                top            ? &vertices[(i - 1) * w + j    ] : NULL, 
                top && right   ? &vertices[(i - 1) * w + j + 1] : NULL, 
                right          ? &vertices[ i      * w + j + 1] : NULL,  
                bottom         ? &vertices[(i + 1) * w + j    ] : NULL, 
                bottom && left ? &vertices[(i + 1) * w + j - 1] : NULL,
                left           ? &vertices[ i      * w + j - 1] : NULL 
            };

            glm::vec3 normal;
            const glm::vec3& a = vertices[i * w + j];
           
            for(int k=0;k<6;++k)
            {
           //     LOG("i:" << i << " j:" << j << " k:" << k);


                if(sides[k] && sides[k + 1])
                {
                    glm::vec3 ab = *sides[k] - a;
                    glm::vec3 ac = *sides[k + 1] - a;
                    normal += glm::cross(ac, ab);

                 //   LOG("normal:" << glm::to_string(normal));
                } 
            }

            normal = glm::normalize(normal);
            normals.push_back(normal);
        }
    }
}

void HeightMap::initTangensAndBinormals(
                std::vector<glm::vec3>& tangents, 
                std::vector<glm::vec3>& binormals, 
                const std::vector<glm::vec3>& vertices,
                const std::vector<glm::vec3>& normals 
            )
{
    for(size_t i=0;i<vertices.size();++i)
    {
        glm::vec3 tangent = vertices[i] - glm::proj(vertices[i], normals[i]);
        glm::vec3 binormal = glm::cross(normals[i], tangent);

        tangent = glm::normalize(tangent);
        binormal = glm::normalize(binormal);

        tangents.push_back(tangent);
        binormals.push_back(binormal);
    }           
}



void HeightMap::initLayers(
            const std::string& heightMapPath,
            const std::vector<Layer>& layers, 
            bool useAbsPath
        )
{
    GLuint buffer;
    glGenFramebuffers(1, &buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer);

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG("ERROR:: HEIGHTMAP FRAMEBUFFER NOT COMPLETE " << status);
        exit(1);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
   // glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    GLuint attachment[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachment);  
    
    FileSystem& fs = FileSystem::getInstance();
    TextureManager& tm = TextureManager::getInstance();
    
    glm::mat4 identity;
    Technique layerTech("shaders/heightmap_layers.vert", "shaders/heightmap_layers.frag");
    layerTech.use();
    layerTech.setHandleWorldViewProj();
    layerTech.setUniformWorldViewProj(identity);
  

    GLuint heightMapId = tm.load(fs.getAbsPath(heightMapPath)); 
    GLuint gHeightMap = glGetUniformLocation(layerTech.mUniforms.program, "u_Sampler");
    GLuint gNumLayers = glGetUniformLocation(layerTech.mUniforms.program, "u_NumLayers");

    glActiveTexture(GL_TEXTURE0);  
    glUniform1i(gHeightMap, 0);
    glBindTexture(GL_TEXTURE_2D, heightMapId);
    glUniform1i(gNumLayers, layers.size()); 
    
    for(unsigned int i=0;i<layers.size();++i)
    {        
        char str[32];
        sprintf(str, "u_Layers[%u]", i);
        GLuint gSampler = glGetUniformLocation(layerTech.mUniforms.program, str);
        sprintf(str, "u_Steps[%u]", i);
        GLuint gStep = glGetUniformLocation(layerTech.mUniforms.program, str);

        glActiveTexture(GL_TEXTURE1 + i);   
        glUniform1i(gSampler, (i + 1));
        glBindTexture(GL_TEXTURE_2D, tm.load(fs.getAbsPath(layers[i].path)));   
        glUniform1f(gStep, layers[i].step);
    }
    
    Quad q;
    q.initBuffers();
  
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);

    q.draw(); 

    for(size_t i=0;i<layers.size() + 1;++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void HeightMap::initBlendedTextures(const std::vector<BlendedTexture>& blendedTextures, bool useAbsPath)
{
   // FileSystem& fs = FileSystem::getInstance();
    //blend texture and blendedtexture of blendedTextures, 

}



void HeightMap::draw()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLE_STRIP, mNumIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HeightMap::unload()
{

}


