#pragma once

#include "geSkeleton.h"

namespace geEngineSDK {

  class SkeletonBuilder
  {
   public:
    void
    clear();

    BoneIndex
    findBoneByName(const String& name) const;

    BoneIndex
    addBone(const Bone& bone);

    void
    setParent(BoneIndex boneIndex, BoneIndex parentIndex);

    void
    rebuildBindGlobals();

    SPtr<Skeleton>
    build() const;

   public:
    Vector<Bone> m_bones;
    Vector<Transform> m_bindPoseLocal;
    UnorderedMap<String, BoneIndex> m_boneNameMap;
    Matrix4 m_globalInverseTransform = Matrix4::IDENTITY;
  };

} // namespace geEngineSDK
