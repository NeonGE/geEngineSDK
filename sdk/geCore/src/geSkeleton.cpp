#include "geSkeleton.h"

namespace geEngineSDK {

  void
  Skeleton::clear() {
    m_bones.clear();
    m_bindPoseLocal.clear();
    m_boneNameMap.clear();
    m_updateOrder.clear();
    m_globalInverseTransform = Matrix4::IDENTITY;
    m_skeletonHash = 0;
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
    m_updateOrder.clear();
    m_updateOrder.reserve(m_bones.size());

    if (m_bones.empty()) {
      m_skeletonHash = 0;
      return;
    }

    Vector<uint8> visited(m_bones.size(), 0);

    function<void(BoneIndex)> pushOrder = [&](BoneIndex i) {
      if (i >= m_bones.size() || visited[i]) {
        return;
      }

      visited[i] = 1;
      m_updateOrder.push_back(i);

      for (BoneIndex child : m_bones[i].children) {
        pushOrder(child);
      }
    };

    for (BoneIndex i = 0; i < m_bones.size(); ++i) {
      if (m_bones[i].parentIndex == INVALID_BONE_INDEX) {
        pushOrder(i);
      }
    }

    // Safety net for malformed/importer-generated skeletons with missing roots.
    for (BoneIndex i = 0; i < m_bones.size(); ++i) {
      pushOrder(i);
    }

    for (BoneIndex i : m_updateOrder) {
      Bone& bone = m_bones[i];

      if (bone.parentIndex != INVALID_BONE_INDEX) {
        bone.bindGlobal = bone.bindLocal * m_bones[bone.parentIndex].bindGlobal;
      }
      else {
        bone.bindGlobal = bone.bindLocal;
      }
    }

    m_skeletonHash = calculateHierarchyHash();
  }

  SIZE_T
  Skeleton::calculateHierarchyHash() const {
    SIZE_T hash = 0;

    ge_hash_combine(hash, cast::st<uint32>(m_bones.size()));

    for (BoneIndex i = 0; i < m_bones.size(); ++i) {
      const Bone& bone = m_bones[i];

      // Compatibility is based on the runtime bone mapping and default local
      // pose, not on mesh-specific SkinBinding inverse bind matrices.
      ge_hash_combine(hash, bone.name);
      ge_hash_combine(hash, bone.parentIndex);
      ge_hash_combine(hash, bone.bindLocal);
    }

    return hash;
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
    return sizeof(*this)
         + sizeof(Bone) * m_bones.size()
         + sizeof(Transform) * m_bindPoseLocal.size()
         + sizeof(BoneIndex) * m_updateOrder.size();
  }

  void
  AnimationClip::resetForSkeleton(const Skeleton& skeleton) {
    const SIZE_T numBones = skeleton.getNumBones();

    m_boneTracks.clear();
    m_hasTrack.clear();

    m_boneTracks.resize(numBones);
    m_hasTrack.resize(numBones, 0);
    m_skeletonHash = skeleton.getSkeletonHash();
  }

  void
  AnimationClip::setTrack(BoneIndex boneIndex, const BoneTrack& track) {
    GE_ASSERT(boneIndex < m_boneTracks.size());
    GE_ASSERT(boneIndex < m_hasTrack.size());

    m_boneTracks[boneIndex] = track;
    m_hasTrack[boneIndex] = 1;
  }

  void
  AnimationClip::setTrack(BoneIndex boneIndex, BoneTrack&& track) {
    GE_ASSERT(boneIndex < m_boneTracks.size());
    GE_ASSERT(boneIndex < m_hasTrack.size());

    m_boneTracks[boneIndex] = std::move(track);
    m_hasTrack[boneIndex] = 1;
  }

  bool
  AnimationClip::isCompatibleWith(const Skeleton& skeleton) const {
    return m_skeletonHash == skeleton.getSkeletonHash()
        && m_boneTracks.size() == skeleton.getNumBones()
        && m_hasTrack.size() == skeleton.getNumBones();
  }

  void
  AnimationClip::clear() {
    m_name.clear();
    m_duration = 0.0f;
    m_ticksPerSecond = 25.0f;
    m_boneTracks.clear();
    m_hasTrack.clear();
    m_skeletonHash = 0;
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
    for (uint8 hasTrack : m_hasTrack) {
      if (hasTrack) {
        return true;
      }
    }

    return false;
  }

  SIZE_T
  AnimationClip::getMemoryUsage() const {
    return sizeof(*this)
         + sizeof(BoneTrack) * m_boneTracks.size()
         + sizeof(uint8) * m_hasTrack.size();
  }

  SPtr<AnimationClip>
  RawAnimationClip::compileForSkeleton(const SPtr<Skeleton>& skeleton) const {
    GE_ASSERT(nullptr != skeleton);

    auto clip = ge_shared_ptr_new<AnimationClip>();
    clip->m_name = m_name;
    clip->m_duration = m_duration;
    clip->m_ticksPerSecond = m_ticksPerSecond;
    clip->resetForSkeleton(*skeleton);

    for (const NamedBoneTrack& namedTrack : m_tracks) {
      const BoneIndex boneIndex = skeleton->getBoneIndexByName(namedTrack.boneName);
      if (boneIndex == INVALID_BONE_INDEX) {
        continue;
      }

      clip->setTrack(boneIndex, namedTrack.track);
    }

    return clip;
  }

  void
  RawAnimationClip::clear() {
    m_name.clear();
    m_duration = 0.0f;
    m_ticksPerSecond = 25.0f;
    m_tracks.clear();
  }

  bool
  RawAnimationClip::load(const Path& filePath) {
    GE_UNREFERENCED_PARAMETER(filePath);
    return false;
  }

  void
  RawAnimationClip::unload() {
    clear();
  }

  bool
  RawAnimationClip::isLoaded() const {
    return !m_tracks.empty();
  }

  SIZE_T
  RawAnimationClip::getMemoryUsage() const {
    return sizeof(*this) + sizeof(NamedBoneTrack) * m_tracks.size();
  }

  void
  AnimationPlayer::play(const SPtr<AnimationClip>& clip,
                        const SPtr<Skeleton>& skeleton,
                        bool loop) {
    GE_ASSERT(nullptr != clip);
    GE_ASSERT(nullptr != skeleton);
    GE_ASSERT(clip->isCompatibleWith(*skeleton));

    if (nullptr == clip || nullptr == skeleton || !clip->isCompatibleWith(*skeleton)) {
      stop();
      return;
    }

    m_currentClip = clip;
    m_skeleton = skeleton;
    m_currentTime = 0.0f;
    m_isLooping = loop;
    m_isPlaying = true;

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
    m_isPlaying = false;
    m_pose = Pose();
  }

  void
  AnimationPlayer::pause(bool bPause) {
    m_isPlaying = !bPause;
  }

  void
  AnimationPlayer::update(float deltaTime) {
    if (nullptr == m_currentClip || nullptr == m_skeleton) {
      return;
    }

    if (!m_isPlaying) {
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

    const BoneIndex numBones = cast::st<BoneIndex>(m_skeleton->getNumBones());

    for (BoneIndex i = 0; i < numBones; ++i) {
      m_pose.localTransform(i) = m_skeleton->m_bindPoseLocal[i];
    }

    for (BoneIndex i = 0; i < numBones; ++i) {
      const BoneTrack* track = m_currentClip->getTrack(i);
      if (nullptr == track) {
        continue;
      }

      Transform& boneTransform = m_pose.localTransform(i);

      if (!track->positions.empty()) {
        boneTransform.setTranslation(sampleVector(track->positions, m_currentTime));
      }

      if (!track->rotations.empty()) {
        boneTransform.setRotation(sampleQuaternion(track->rotations, m_currentTime));
      }

      if (!track->scales.empty()) {
        boneTransform.setScale3D(sampleVector(track->scales, m_currentTime));
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

    const Vector<BoneIndex>& updateOrder = m_skeleton->m_updateOrder;

    if (!updateOrder.empty()) {
      for (BoneIndex boneIndex : updateOrder) {
        const Bone& bone = m_skeleton->getBone(boneIndex);

        if (bone.parentIndex != INVALID_BONE_INDEX) {
          m_globalMatrices[boneIndex] =
            m_localMatrices[boneIndex] * m_globalMatrices[bone.parentIndex];
        }
        else {
          m_globalMatrices[boneIndex] = m_localMatrices[boneIndex];
        }
      }

      return;
    }

    // Fallback for skeletons created before m_updateOrder existed.
    Vector<uint8> visited(n, 0);

    function<void(BoneIndex)> buildGlobal = [&](BoneIndex i) {
      if (visited[i]) {
        return;
      }

      visited[i] = 1;

      const Bone& bone = m_skeleton->getBone(i);

      if (bone.parentIndex != INVALID_BONE_INDEX) {
        buildGlobal(bone.parentIndex);
        m_globalMatrices[i] = m_localMatrices[i] * m_globalMatrices[bone.parentIndex];
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
                                                  Vector<Matrix4>& outFinal,
                                                  bool transposeForGPU) const {
    GE_ASSERT(nullptr != m_skeleton);
    GE_ASSERT(meshOffsets.size() == m_skeleton->getNumBones());

    const BoneIndex n = cast::st<BoneIndex>(m_skeleton->getNumBones());
    outFinal.resize(n);

    for (BoneIndex i = 0; i < n; ++i) {
      // Row-vector convention:
      // vertexBind * inverseBind(mesh space) * animatedGlobal * globalInverse
      Matrix4 M = meshOffsets[i] *
                  m_globalMatrices[i] *
                  m_skeleton->m_globalInverseTransform;

      // Keep true for the current DX11 path if your shader constant upload
      // expects transposed matrices. Pass false when the renderer handles this
      // per-backend during upload.
      if (transposeForGPU) {
        M.transpose();
      }

      outFinal[i] = M;
    }
  }

  void
  SkeletonInstance::applyPoseForMesh(const Pose& pose,
                                     const Vector<Matrix4>& meshOffsets,
                                     bool transposeForGPU) {
    applyPose(pose);
    buildFinalBoneMatricesForMesh(meshOffsets, m_finalMatrices, transposeForGPU);
  }

} // namespace geEngineSDK
