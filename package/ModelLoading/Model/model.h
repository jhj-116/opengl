#ifndef MODEL_H
#define MODEL_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <package/ModelLoading/Mesh/mesh.h>
#include <shader/shader_m.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFileModelT(const char* path, const string& directory, bool gamma=false)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    //翻转图片
    //stbi_set_flip_vertically_on_load(true);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        //Assimp 将模型加载到 Assimp 的数据结构中，称为Scene
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        /*
        通过设置，我们告诉 Assimp，如果模型不（完全）由三角形组成，它应该首先将模型的所有原始形状转换为三角形。
        在处理过程中，必要时会在 y 轴上翻转纹理坐标（您可能还记得 Textures 一章中，
        OpenGL 中的大多数图像都是围绕 y 轴反转的;这个后处理选项为我们解决了这个问题）。
        其他一些有用的选项是：aiProcess_TriangulateaiProcess_FlipUVs

        aiProcess_GenNormals：如果模型不包含法线向量，则为每个顶点创建法线向量。(网格顶点法线都是面法线加权)
        aiProcess_SplitLargeMeshes：将大型网格分割为较小的子网格，如果您的渲染允许最大顶点数并且只能处理较小的网格，这将非常有用。
        aiProcess_OptimizeMeshes：通过尝试将多个网格合并为一个更大的网格来执行相反的操作，从而减少优化所需的绘图调用。
        */

        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;//如果有错误，则导入程序的 GetErrorString 函数检索到的错误并返回
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
        /*
        根据Assimp 的结构，每个节点都包含一组网格索引，其中每个索引都指向位于场景对象中的特定网格。
        因此，我们想要检索这些网格索引，检索每个网格，处理每个网格，然后对节点的每个子节点再次执行所有这些操作
        那这里为什么不直接使用aiMesh* mesh = scene->mMeshes[scene->mNumMeshes]获得场景中所有Mesh呢？

        细心的读者可能已经注意到，我们可以不处理任何节点，而只需直接遍历场景的所有网格
        (scene 的 mMeshes 保存所有的网格，而子节点和孙子节点则保存对部分网格的引用)，而无需对索引执行所有这些复杂操作。
        我们这样做的原因是，使用此类节点的最初想法是它定义了网格之间的父子关系。通过递归迭代这些关系，我们可以将某些网格定义为其他网格的父级。
        此类系统的一个示例用例是，当您想要平移汽车网格并确保其所有子项（如引擎网格、方向盘网格及其轮胎网格）也平移时;
        使用父子关系很容易创建这样的系统。
        但是，目前我们没有使用这样的系统，但通常建议在您想要对网格数据进行额外控制时坚持使用此方法。
        
        */
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)//获取mesh(网格)的顶点属性(顶点坐标，纹理坐标，法线)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        //Assimp 的接口将每个网格定义为具有一组面，其中每个面代表一个基元，在我们的例子中（由于选项）始终是三角形。
        // 面包含我们需要为其基元绘制的顶点的索引。
        // 因此，如果我们遍历所有 faces 并将所有 face 的索引存储在 vector 中，我们就全部设置好了
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);//保存三角面的顶点坐标索引
        }
        // process materials
        std::cout << scene->mNumMaterials << "," << mesh->mMaterialIndex << endl;
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];//每一个网格对应一个材质。应该是assimp在加载模型的并将
        //模型数据解析导入assimp结构数据体时，对一个复杂的模型(如果一个对象需要多个材质，通常会将其拆分为多个网格。
        // 例如，一个复杂模型（如角色）可能会被拆分成多个网格，每个网格对应一个材质。
        // 这样，每个网格都可以独立使用其特定的材质)会进行一定的划分处理。
    

        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");//纹理法线贴图
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");//纹理高度贴图
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);//获取材质的制定类型的贴图(比如漫反射的纹理贴图或镜面反射的纹理贴图等，一个材质有多个纹理贴图)
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            //这里是下面是根据texure的路径利用stbi_image库加载texture纹理。如果mesh的纹理已经加载就会被放入到textures_loaded容器中，
            //当我们遍历textures_loaded容器中发现mat->GetTexture获取的纹理路径在容器中了，把标志符skip=true,
            // 就不需要执行下面stbi_image库加载texture纹理(因为这个纹理之前就被加载了，干嘛还在加载一次，不浪费时间内存嘛)
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFileModelT(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};



#endif