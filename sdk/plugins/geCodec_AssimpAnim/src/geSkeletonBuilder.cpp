#include "geSkeletonBuilder.h"

namespace geEngineSDK {

  void
  SkeletonBuilder::clear() {
    m_bones.clear();
    m_bindPoseLocal.clear();
    m_boneNameMap.clear();
    m_globalInverseTransform = Matrix4::IDENTITY;
  }

  BoneIndex
  SkeletonBuilder::findBoneByName(const String& name) const {
    auto it = m_boneNameMap.find(name);
    if (it != m_boneNameMap.end()) {
      return it->second;
    }

    return INVALID_BONE_INDEX;
  }

  BoneIndex
  SkeletonBuilder::addBone(const Bone& bone) {
    GE_ASSERT(m_bones.size() < MAX_BONES);

    const BoneIndex idx = cast::st<BoneIndex>(m_bones.size());
    m_bones.push_back(bone);
    m_boneNameMap[bone.name] = idx;
    m_bindPoseLocal.push_back(Transform::IDENTITY);
    return idx;
  }

  void
  SkeletonBuilder::setParent(BoneIndex boneIndex, BoneIndex parentIndex) {
    GE_ASSERT(boneIndex < m_bones.size());

    Bone& bone = m_bones[boneIndex];

    if (bone.parentIndex != INVALID_BONE_INDEX) {
      Bone& oldParent = m_bones[bone.parentIndex];
      for (auto it = oldParent.children.begin(); it != oldParent.children.end(); ++it) {
        if (*it == boneIndex) {
          oldParent.children.erase(it);
          break;
        }
      }
    }

    bone.parentIndex = parentIndex;

    if (parentIndex != INVALID_BONE_INDEX) {
      GE_ASSERT(parentIndex < m_bones.size());
      m_bones[parentIndex].children.push_back(boneIndex);
    }
  }

  void
  SkeletonBuilder::rebuildBindGlobals() {
    if (m_bones.empty()) {
      return;
    }

    Vector<uint8> visited(m_bones.size(), 0);

    function<void(BoneIndex)> buildGlobal = [&](BoneIndex i)
    {
      if (visited[i]) {
        return;
      }

      visited[i] = 1;

      Bone& bone = m_bones[i];

      if (bone.parentIndex != INVALID_BONE_INDEX) {
        buildGlobal(bone.parentIndex);
        bone.bindGlobal = bone.bindLocal * m_bones[bone.parentIndex].bindGlobal;
      }
      else {
        bone.bindGlobal = bone.bindLocal;
      }
    };

    for (BoneIndex i = 0; i < m_bones.size(); ++i) {
      buildGlobal(i);
    }
  }

  SPtr<Skeleton>
  SkeletonBuilder::build() const {
    auto skeleton = ge_shared_ptr_new<Skeleton>();
    skeleton->m_bones = m_bones;
    skeleton->m_bindPoseLocal = m_bindPoseLocal;
    skeleton->m_boneNameMap = m_boneNameMap;
    skeleton->m_globalInverseTransform = m_globalInverseTransform;
    return skeleton;
  }

} // namespace geEngineSDK
