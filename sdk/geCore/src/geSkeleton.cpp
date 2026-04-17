#include "geSkeleton.h"

namespace geEngineSDK {

  void
  Skeleton::clear() {
    m_bones.clear();
    m_bindPoseLocal.clear();
    m_boneNameMap.clear();
    m_globalInverseTransform = Matrix4::IDENTITY;
  }

  bool
  Skeleton::validateHierarchy() const {
    for (BoneIndex i = 0; i < m_bones.size(); ++i) {
      const Bone& bone = m_bones[i];

      if (bone.parentIndex != INVALID_BONE_INDEX) {
        if (bone.parentIndex >= m_bones.size()) {
          return false;
        }
      }

      for (BoneIndex child : bone.children) {
        if (child >= m_bones.size()) {
          return false;
        }

        if (m_bones[child].parentIndex != i) {
          return false;
        }
      }
    }

    return m_bindPoseLocal.size() == m_bones.size();
  }

  void
  Skeleton::rebuildBindGlobals() {
    if (m_bones.empty()) {
      return;
    }

    for (BoneIndex i = 0; i < m_bones.size(); ++i) {
      if (m_bones[i].parentIndex == INVALID_BONE_INDEX) {
        m_bones[i].bindGlobal = m_bones[i].bindLocal;
      }
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

  bool
  Skeleton::load(const Path& filePath) {
    GE_UNREFERENCED_PARAMETER(filePath);
    return false;
  }

  void
  Skeleton::unload() {
    clear();
  }

  bool
  Skeleton::isLoaded() const {
    return !m_bones.empty();
  }

  SIZE_T
  Skeleton::getMemoryUsage() const {
    return 0;
  }

  void
  AnimationClip::clear() {
    m_name.clear();
    m_duration = 0.0f;
    m_ticksPerSecond = 25.0f;
    m_boneTracks.clear();
  }

  bool
  AnimationClip::load(const Path& filePath) {
    GE_UNREFERENCED_PARAMETER(filePath);
    return false;
  }

  void
  AnimationClip::unload() {
    clear();
  }

  bool
  AnimationClip::isLoaded() const {
    return !m_boneTracks.empty();
  }

  SIZE_T
  AnimationClip::getMemoryUsage() const {
    return sizeof(*this);
  }

  void
  AnimationPlayer::play(const SPtr<AnimationClip>& clip,
                        const SPtr<Skeleton>& skeleton,
                        bool loop) {
    GE_ASSERT(nullptr != clip);
    GE_ASSERT(nullptr != skeleton);

    m_currentClip = clip;
    m_skeleton = skeleton;
    m_currentTime = 0.0f;
    m_isLooping = loop;

    m_pose.resize(skeleton->getNumBones());

    for (BoneIndex i = 0; i < skeleton->getNumBones(); ++i) {
      m_pose.localTransform(i) = skeleton->m_bindPoseLocal[i];
    }
  }

  void
  AnimationPlayer::stop() {
    m_currentClip = nullptr;
    m_skeleton = nullptr;
    m_currentTime = 0.0f;
    m_isLooping = true;
    m_pose = Pose();
  }

  void
  AnimationPlayer::update(float deltaTime) {
    if (nullptr == m_currentClip || nullptr == m_skeleton) {
      return;
    }

    m_currentTime += deltaTime * m_currentClip->getTicksPerSecond();

    if (m_isLooping) {
      if (m_currentClip->getDuration() > 0.0f) {
        m_currentTime = fmod(m_currentTime, m_currentClip->getDuration());
      }
    }
    else {
      m_currentTime = Math::min(m_currentTime, m_currentClip->getDuration());
    }

    for (BoneIndex i = 0; i < m_skeleton->getNumBones(); ++i) {
      m_pose.localTransform(i) = m_skeleton->m_bindPoseLocal[i];
    }

    for (auto& it : m_currentClip->m_boneTracks) {
      const BoneIndex boneIndex = it.first;
      const BoneTrack& track = it.second;

      Transform& boneTransform = m_pose.localTransform(boneIndex);

      if (!track.positions.empty()) {
        boneTransform.setTranslation(sampleVector(track.positions, m_currentTime));
      }

      if (!track.rotations.empty()) {
        boneTransform.setRotation(sampleQuaternion(track.rotations, m_currentTime));
      }

      if (!track.scales.empty()) {
        boneTransform.setScale3D(sampleVector(track.scales, m_currentTime));
      }
    }
  }

  SkeletonInstance::SkeletonInstance(const SPtr<Skeleton>& skeleton) {
    setSkeleton(skeleton);
  }

  void
  SkeletonInstance::setSkeleton(const SPtr<Skeleton>& skeleton) {
    GE_ASSERT(nullptr != skeleton);

    m_skeleton = skeleton;

    const SIZE_T numBones = skeleton->getNumBones();
    m_localMatrices.resize(numBones, Matrix4::IDENTITY);
    m_globalMatrices.resize(numBones, Matrix4::IDENTITY);
    m_finalMatrices.resize(numBones, Matrix4::IDENTITY);
  }

  void
  SkeletonInstance::applyPose(const Pose& pose) {
    GE_ASSERT(nullptr != m_skeleton);
    GE_ASSERT(pose.size() == m_skeleton->getNumBones());

    const BoneIndex n = cast::st<BoneIndex>(m_skeleton->getNumBones());

    for (BoneIndex i = 0; i < n; ++i) {
      m_localMatrices[i] = pose.localTransform(i).toMatrixWithScale();
    }

    Vector<uint8> visited(n, 0);

    std::function<void(BoneIndex)> buildGlobal = [&](BoneIndex i) {
      if (visited[i]) {
        return;
      }

      visited[i] = 1;

      const Bone& b = m_skeleton->getBone(i);

      if (b.parentIndex != INVALID_BONE_INDEX) {
        buildGlobal(b.parentIndex);
        m_globalMatrices[i] = m_localMatrices[i] * m_globalMatrices[b.parentIndex];
      }
      else {
        m_globalMatrices[i] = m_localMatrices[i];
      }
      };

    for (BoneIndex i = 0; i < n; ++i) {
      buildGlobal(i);
    }
  }

  void
  SkeletonInstance::buildFinalBoneMatricesForMesh(const Vector<Matrix4>& meshOffsets,
      Vector<Matrix4>& outFinal) const {
    GE_ASSERT(nullptr != m_skeleton);
    GE_ASSERT(meshOffsets.size() == m_skeleton->getNumBones());

    const BoneIndex n = cast::st<BoneIndex>(m_skeleton->getNumBones());
    outFinal.resize(n);

    for (BoneIndex i = 0; i < n; ++i) {
      // Con tu convención de vector fila:
      // bindPos * offset * currentGlobal * globalInverse
      Matrix4 M = meshOffsets[i] * m_globalMatrices[i] * m_skeleton->m_globalInverseTransform;
      M.transpose();
      outFinal[i] = M;
    }
  }

  void
  SkeletonInstance::applyPoseForMesh(const Pose& pose,
                                     const Vector<Matrix4>& meshOffsets) {
    applyPose(pose);
    buildFinalBoneMatricesForMesh(meshOffsets, m_finalMatrices);
  }

} // namespace geEngineSDK
