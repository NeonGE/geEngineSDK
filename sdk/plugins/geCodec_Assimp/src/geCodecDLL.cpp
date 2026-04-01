/*****************************************************************************/
/**
 * @file    geCodecDLL.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/18
 * @brief   Plugin entry point for the Codec.
 *
 * Plugin entry point for the Codec.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/

#include <gePrerequisitesCore.h>
#include <geICodec.h>
#include <geRenderAPI.h>
#include <geMountManager.h>

#include "geTimer.h"
#include "geModelBuilder.h"

using namespace geEngineSDK;

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Vector3
toVector3(const aiVector3D& v) {
  return Vector3(v.x, v.y, v.z);
}

Vector2
toVector2(const aiVector3D& v) {
  return Vector2(v.x, v.y);
}

Vector4
toVector4(const aiVector3D& v, float w) {
  return Vector4(v.x, v.y, v.z, w);
}

static Matrix4
aiMatrixToMatrix4(const aiMatrix4x4& m) {
  return Matrix4(m.a1, m.a2, m.a3, m.a4,
                 m.b1, m.b2, m.b3, m.b4,
                 m.c1, m.c2, m.c3, m.c4,
                 m.d1, m.d2, m.d3, m.d4);
}

static String CODEC_NAME = "Assimp Mesh Loader Codec";
static String CODEC_DESC = "This codec implements the Assimp Library "
                           "to load 3D models";

static Vector<String> CODEC_EXTENSIONS_IMPORT = {
  ".glb", ".fbx", ".obj", ".3ds"
};

static Vector<String> CODEC_EXTENSIONS_EXPORT = {
  
};

extern "C"
{
  GE_PLUGIN_EXPORT CODEC_TYPE::E
  CodecType(void) {
    return CODEC_TYPE::MODEL;
  }

  GE_PLUGIN_EXPORT void
  CodecVersion(uint32& major, uint32& minor, uint32& patch) {
    major = 6;
    minor = 0;
    patch = 4;
  }

  GE_PLUGIN_EXPORT const String&
  CodecName(void) {
    return CODEC_NAME;
  }

  GE_PLUGIN_EXPORT const String&
  CodecDescription(void) {
    return CODEC_DESC;
  }

  GE_PLUGIN_EXPORT const Vector<String>&
  CodecExtensions(void) {
    return CODEC_EXTENSIONS_IMPORT;
  }

  GE_PLUGIN_EXPORT bool
  CodecCanImport(const Path& filePath) {
    // Check if the file extension is one of the supported formats
    String ext = filePath.getExtension();
    for (const auto& supportedExt : CODEC_EXTENSIONS_IMPORT) {
      if (StringUtil::match(ext, supportedExt, false)) {
        return true;
      }
    }

    return false;
  }

  GE_PLUGIN_EXPORT bool
  CodecCanExport(const Path& filePath) {
    // Check if the file extension is one of the supported formats to export
    String ext = filePath.getExtension();
    for (const auto& supportedExt : CODEC_EXTENSIONS_EXPORT) {
      if (StringUtil::match(ext, supportedExt, false)) {
        return true;
      }
    }

    return false;
  }

  void
  processAssimpStaticMesh(ModelBuilder& builder,
                        uint32 nodeIndex,
                        const aiMesh* mesh) {
    GE_ASSERT(nullptr != mesh);
    GE_ASSERT(!mesh->HasBones());
    GE_ASSERT(nodeIndex < builder.m_nodes.size());

    using VES = VERTEX_ELEMENT_SEMANTIC::E;
    using VET = VERTEX_ELEMENT_TYPE::E;

    if (!mesh->HasPositions() || mesh->mNumVertices == 0) {
      return;
    }

    if (!mesh->HasFaces() || mesh->mNumFaces == 0) {
      return;
    }

    const bool hasPoints = (mesh->mPrimitiveTypes & aiPrimitiveType_POINT) != 0;
    const bool hasLines = (mesh->mPrimitiveTypes & aiPrimitiveType_LINE) != 0;
    const bool hasTriangles = (mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) != 0;
    const bool hasPolygons = (mesh->mPrimitiveTypes & aiPrimitiveType_POLYGON) != 0;

    const uint32 primitiveTypeCount = (hasPoints ? 1u : 0u) +
                                      (hasLines ? 1u : 0u) +
                                      (hasTriangles ? 1u : 0u) +
                                      (hasPolygons ? 1u : 0u);

    //For now we only support meshes with a single primitive topology.
    if (primitiveTypeCount != 1) {
      return;
    }

    PRIMITIVE_TOPOLOGY::E topology = PRIMITIVE_TOPOLOGY::TRIANGLELIST;
    if (hasPoints) {
      topology = PRIMITIVE_TOPOLOGY::POINTLIST;
    }
    else if (hasLines) {
      topology = PRIMITIVE_TOPOLOGY::LINELIST;
    }
    else if (hasTriangles) {
      topology = PRIMITIVE_TOPOLOGY::TRIANGLELIST;
    }
    else {
      //Polygon faces need a separate path or prior triangulation.
      return;
    }

    uint32 dataOffset = 0;
    Vector<VertexElement> vertexElements;

    uint32 posOffset = NumLimit::MAX_UINT32;
    uint32 norOffset = NumLimit::MAX_UINT32;
    uint32 tanOffset = NumLimit::MAX_UINT32;
    uint32 uvOffset0 = NumLimit::MAX_UINT32;
    uint32 uvOffset1 = NumLimit::MAX_UINT32;
    uint32 colOffset0 = NumLimit::MAX_UINT32;

    if (mesh->HasPositions()) {
      posOffset = dataOffset;
      vertexElements.push_back(VertexElement(0, dataOffset, VET::FLOAT3, VES::POSITION));
      dataOffset += vertexElements.back().getSize();
    }

    if (mesh->HasNormals()) {
      norOffset = dataOffset;
      vertexElements.push_back(VertexElement(0, dataOffset, VET::FLOAT3, VES::NORMAL));
      dataOffset += vertexElements.back().getSize();
    }

    if (mesh->HasTangentsAndBitangents()) {
      tanOffset = dataOffset;
      vertexElements.push_back(VertexElement(0, dataOffset, VET::FLOAT4, VES::TANGENT));
      dataOffset += vertexElements.back().getSize();
    }

    if (mesh->HasTextureCoords(0)) {
      uvOffset0 = dataOffset;
      vertexElements.push_back(VertexElement(0, dataOffset, VET::FLOAT2, VES::TEXCOORD, 0));
      dataOffset += vertexElements.back().getSize();
    }

    if (mesh->HasTextureCoords(1)) {
      uvOffset1 = dataOffset;
      vertexElements.push_back(VertexElement(0, dataOffset, VET::FLOAT2, VES::TEXCOORD, 1));
      dataOffset += vertexElements.back().getSize();
    }

    if (mesh->HasVertexColors(0)) {
      colOffset0 = dataOffset;
      vertexElements.push_back(VertexElement(0, dataOffset, VET::COLOR, VES::COLOR));
      dataOffset += vertexElements.back().getSize();
    }

    auto& renderAPI = RenderAPI::instance();
    auto vertexDecl = renderAPI.createVertexDeclaration(vertexElements);
    uint32 vertexSize = vertexDecl->getProperties().getVertexSize(0);

    Vector<uint8> vertices;
    Vector<uint32> indices;

    vertices.resize(mesh->mNumVertices * vertexSize, 0);

    uint32 totalIndices = 0;
    for (uint32 i = 0; i < mesh->mNumFaces; ++i) {
      totalIndices += mesh->mFaces[i].mNumIndices;
    }
    indices.reserve(totalIndices);

    AABox bounds = AABox::EMPTY;

    for (uint32 i = 0; i < mesh->mNumVertices; ++i) {
      const SIZE_T vertexBase = i * vertexSize;

      if (posOffset != NumLimit::MAX_UINT32) {
        const Vector3 position = toVector3(mesh->mVertices[i]);
        memcpy(&vertices[vertexBase + posOffset], &position, sizeof(Vector3));
        bounds += position;
      }

      if (norOffset != NumLimit::MAX_UINT32) {
        const Vector3 normal = toVector3(mesh->mNormals[i]);
        memcpy(&vertices[vertexBase + norOffset], &normal, sizeof(Vector3));
      }

      if (tanOffset != NumLimit::MAX_UINT32) {
        //TODO: Compute handedness in .w using normal/tangent/bitangent if needed.
        const Vector4 tangent = toVector4(mesh->mTangents[i], 1.0f);
        memcpy(&vertices[vertexBase + tanOffset], &tangent, sizeof(Vector4));
      }

      if (uvOffset0 != NumLimit::MAX_UINT32) {
        const Vector2 uv0 = toVector2(mesh->mTextureCoords[0][i]);
        memcpy(&vertices[vertexBase + uvOffset0], &uv0, sizeof(Vector2));
      }

      if (uvOffset1 != NumLimit::MAX_UINT32) {
        const Vector2 uv1 = toVector2(mesh->mTextureCoords[1][i]);
        memcpy(&vertices[vertexBase + uvOffset1], &uv1, sizeof(Vector2));
      }

      if (colOffset0 != NumLimit::MAX_UINT32) {
        const aiColor4D& c = mesh->mColors[0][i];
        const Color color = LinearColor(c.r, c.g, c.b, c.a).toColor(false);
        memcpy(&vertices[vertexBase + colOffset0], &color, sizeof(Color));
      }
    }

    for (uint32 i = 0; i < mesh->mNumFaces; ++i) {
      const aiFace& face = mesh->mFaces[i];
      switch (topology) {
      case PRIMITIVE_TOPOLOGY::POINTLIST:
        GE_ASSERT(face.mNumIndices == 1);
        break;
      case PRIMITIVE_TOPOLOGY::LINELIST:
        GE_ASSERT(face.mNumIndices == 2);
        break;
      case PRIMITIVE_TOPOLOGY::TRIANGLELIST:
        GE_ASSERT(face.mNumIndices == 3);
        break;
      default:
        GE_ASSERT(false);
        return;
      }

      for (uint32 j = 0; j < face.mNumIndices; ++j) {
        indices.push_back(face.mIndices[j]);
      }
    }

    String meshName = mesh->mName.length > 0 ? String(mesh->mName.C_Str()) : String("Mesh");
    
    const uint32 meshGroupIndex = builder.findOrCreateStaticMeshGroup("StaticMeshGroup",
                                                                      vertexDecl,
                                                                      topology);

    builder.appendStaticSubMesh(meshGroupIndex,
                                meshName,
                                nodeIndex,
                                mesh->mMaterialIndex,
                                vertices,
                                indices,
                                bounds,
                                mesh->mNumFaces);
  }

  uint32
  processAssimpNodeRecursive(ModelBuilder& builder,
                             const aiScene* scene,
                             const aiNode* node,
                             int32 parentIndex) {
    GE_ASSERT(nullptr != scene);
    GE_ASSERT(nullptr != node);

    ModelNode modelNode(node->mName.C_Str());
    modelNode.m_localTransform = aiMatrixToMatrix4(node->mTransformation);

    const uint32 nodeIndex = builder.addNode(modelNode);
    builder.setNodeParent(nodeIndex, parentIndex);

    for (uint32 i = 0; i < node->mNumMeshes; ++i) {
      const uint32 meshIndex = node->mMeshes[i];
      GE_ASSERT(meshIndex < scene->mNumMeshes);

      const aiMesh* mesh = scene->mMeshes[meshIndex];
      GE_ASSERT(nullptr != mesh);

      if (!mesh->HasBones()) {
        processAssimpStaticMesh(builder, nodeIndex, mesh);
      }
      else {
        //TODO: Skinned path
      }
    }

    for (uint32 i = 0; i < node->mNumChildren; ++i) {
      processAssimpNodeRecursive(builder,
                                 scene,
                                 node->mChildren[i],
                                 cast::st<int32>(nodeIndex));
    }

    return nodeIndex;
  }

  bool
  importAssimpSceneToModelBuilder(const aiScene* scene,
                                  ModelBuilder& builder) {
    if (nullptr == scene || nullptr == scene->mRootNode) {
      return false;
    }

    builder.clear();

    processAssimpNodeRecursive(builder,
                               scene,
                               scene->mRootNode,
                               -1);
    builder.updateBounds();
    return true;
  }

  GE_PLUGIN_EXPORT void
  CodecImport(const Path& filePath, bool useCacheIfAvailable, SPtr<Resource>& outRes) {
    GE_UNREFERENCED_PARAMETER(useCacheIfAvailable);

    if (!CodecCanImport(filePath)) {
      GE_LOG(kError,
             Generic,
             String("Cannot import file: {0}. Unsupported format."), filePath);
      return;
    }

    //auto& renderAPI = RenderAPI::instance();
    auto& mountman = MountManager::instance();

    auto pFileData = mountman.open(filePath);
    Vector<uint8>fileData;
    pFileData->getAllData(fileData);

    /*************************************************************************/
    //Extract the extension without the starting dot '.'
    String extension = filePath.getExtension().substr(1);
    StringUtil::toLowerCase(extension);
   
    Assimp::Importer importer;

    uint32 flags = aiProcessPreset_TargetRealtime_MaxQuality
                 | aiProcess_ConvertToLeftHanded;
    //flags |= aiProcess_PreTransformVertices;
    flags &= ~aiProcess_CalcTangentSpace;
    flags &= ~aiProcess_RemoveRedundantMaterials;
    flags &= ~aiProcess_JoinIdenticalVertices;
    
    HighResTimer profilingTimer;
    const aiScene* pScene = importer.ReadFileFromMemory(fileData.data(),
                                                        fileData.size(),
                                                        flags,
                                                        extension.c_str());
    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
      std::cout << "ERROR::ASSIMP: Scene is incomplete or root node is missing: "
                << importer.GetErrorString() << std::endl;
      return;
    }
    auto loadingTime = profilingTimer.getMilliseconds();

    ModelBuilder builder;

    importAssimpSceneToModelBuilder(pScene, builder);
    outRes = builder.build();

    /*************************************************************************/
  }

  GE_PLUGIN_EXPORT bool
  CodecExport(const SPtr<Resource>& /*resource*/, const Path& /*filePath*/) {
    return false;
  }
}
