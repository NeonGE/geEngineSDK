#pragma once

#include <gePrerequisitesCore.h>
#include <geResource.h>
#include <geMatrix4.h>
#include <geTransform.h>

namespace geEngineSDK {

  constexpr uint32 MAX_BONES = 512;
  constexpr uint32 MAX_BONE_INFLUENCE = 4;
  constexpr uint16 INVALID_BONE_INDEX = NumLimit::MAX_UINT16;

  using BoneIndex = uint16;
  using AnimationID = uint32;

  struct Bone
  {
    String name;

    BoneIndex parentIndex = INVALID_BONE_INDEX;
    Vector<BoneIndex> children;

    Matrix4 bindLocal = Matrix4::IDENTITY;
    Matrix4 bindGlobal = Matrix4::IDENTITY;

    // Assimp aiBone::mOffsetMatrix
    Matrix4 offset = Matrix4::IDENTITY;
  };

  template<typename T>
  struct KeyFrame
  {
    float time = 0.0f;
    T value;
  };

  template<typename T>
  uint32
  findKeyframe(const Vector<KeyFrame<T>>& keys, float time) {
    if (keys.empty()) {
      return 0;
    }

    for (uint32 i = 0; i + 1 < keys.size(); ++i) {
      if (time < keys[i + 1].time) {
        return i;
      }
    }

    return cast::st<uint32>(keys.size() - 1);
  }

  inline Vector3
  sampleVector(const Vector<KeyFrame<Vector3>>& keys, float time) {
    if (keys.empty()) {
      return Vector3::ZERO;
    }

    if (keys.size() == 1) {
      return keys[0].value;
    }

    uint32 index = findKeyframe(keys, time);
    if (index + 1 >= keys.size()) {
      return keys[index].value;
    }

    uint32 nextIndex = index + 1;
    float deltaTime = keys[nextIndex].time - keys[index].time;

    if (Math::isNearlyZero(deltaTime)) {
      return keys[index].value;
    }

    float factor = (time - keys[index].time) / deltaTime;
    return Math::lerp(keys[index].value, keys[nextIndex].value, factor);
  }

  inline Quaternion
  sampleQuaternion(const Vector<KeyFrame<Quaternion>>& keys, float time) {
    if (keys.empty()) {
      return Quaternion::IDENTITY;
    }

    if (keys.size() == 1) {
      return keys[0].value;
    }

    uint32 index = findKeyframe(keys, time);
    if (index + 1 >= keys.size()) {
      return keys[index].value;
    }

    uint32 nextIndex = index + 1;
    float deltaTime = keys[nextIndex].time - keys[index].time;

    if (Math::isNearlyZero(deltaTime)) {
      return keys[index].value;
    }

    float factor = (time - keys[index].time) / deltaTime;

    Quaternion q = Quaternion::slerp(keys[index].value,
      keys[nextIndex].value,
      factor);
    q.normalize();
    return q;
  }

  struct BoneTrack
  {
    Vector<KeyFrame<Vector3>> positions;
    Vector<KeyFrame<Quaternion>> rotations;
    Vector<KeyFrame<Vector3>> scales;
  };

  class GE_CORE_EXPORT Skeleton : public Resource
  {
   public:
    Skeleton() = default;
    ~Skeleton() override = default;

    const Bone&
    getBone(BoneIndex index) const {
      GE_ASSERT(index < m_bones.size());
      return m_bones[index];
    }

    BoneIndex
    getBoneIndexByName(const String& name) const {
      auto it = m_boneNameMap.find(name);
      if (it != m_boneNameMap.end()) {
        return it->second;
      }

      return INVALID_BONE_INDEX;
    }

    SIZE_T
    getNumBones() const {
      return m_bones.size();
    }

    void
    clear();

    bool
    validateHierarchy() const;

    void
    rebuildBindGlobals();

    bool
    load(const Path& filePath) override;

    void
    unload() override;

    bool
    isLoaded() const override;

    SIZE_T
    getMemoryUsage() const override;

   public:
    Vector<Bone> m_bones;
    Vector<Transform> m_bindPoseLocal;
    UnorderedMap<String, BoneIndex> m_boneNameMap;
    Matrix4 m_globalInverseTransform = Matrix4::IDENTITY;
  };

  class GE_CORE_EXPORT AnimationClip : public Resource
  {
   public:
    AnimationClip() = default;
    ~AnimationClip() override = default;

    float
    getDuration() const {
      return m_duration;
    }

    float
    getTicksPerSecond() const {
      return m_ticksPerSecond;
    }

    const BoneTrack*
    getTrack(BoneIndex boneIndex) const {
      auto it = m_boneTracks.find(boneIndex);
      if (it != m_boneTracks.end()) {
        return &it->second;
      }
      return nullptr;
    }

    void
    clear();

    bool
    load(const Path& filePath) override;

    void
    unload() override;

    bool
    isLoaded() const override;

    SIZE_T
    getMemoryUsage() const override;

   public:
    String m_name;
    float m_duration = 0.0f;
    float m_ticksPerSecond = 25.0f;
    Map<BoneIndex, BoneTrack> m_boneTracks;
  };

  class AnimationClipCollection : public Resource
  {
   public:
    AnimationClipCollection() = default;
    ~AnimationClipCollection() override = default;

    bool
    load(const Path& filePath) override {
      GE_UNREFERENCED_PARAMETER(filePath);
      return false;
    }

    void
    unload() override {}

    bool
    isLoaded() const override {
      return !m_clips.empty();
    }

    SIZE_T
    getMemoryUsage() const override {
      SIZE_T accumulated = 0;
      for (auto& clip : m_clips) {
        if (clip) {
          accumulated += clip->getMemoryUsage();
        }
      }

      return accumulated;
    }

   public:
    Vector<SPtr<AnimationClip>> m_clips;
  };

  class GE_CORE_EXPORT Pose
  {
   public:
    void
    resize(SIZE_T numBones) {
      m_localTransforms.resize(numBones);
    }

    Transform&
    localTransform(BoneIndex index) {
      GE_ASSERT(index < m_localTransforms.size());
      return m_localTransforms[index];
    }

    const Transform&
    localTransform(BoneIndex index) const {
      GE_ASSERT(index < m_localTransforms.size());
      return m_localTransforms[index];
    }

    SIZE_T
    size() const {
      return m_localTransforms.size();
    }

   private:
    Vector<Transform> m_localTransforms;
  };

  class GE_CORE_EXPORT AnimationPlayer
  {
   public:
    void
    play(const SPtr<AnimationClip>& clip,
         const SPtr<Skeleton>& skeleton,
         bool loop = true);

    void
    stop();

    void
    update(float deltaTime);

    const Pose&
    getCurrentPose() const {
      return m_pose;
    }

   private:
    SPtr<AnimationClip> m_currentClip;
    SPtr<Skeleton> m_skeleton;
    float m_currentTime = 0.0f;
    bool m_isLooping = true;
    Pose m_pose;
  };

  class GE_CORE_EXPORT SkeletonInstance
  {
   public:
    SkeletonInstance() = default;
    explicit SkeletonInstance(const SPtr<Skeleton>& skeleton);

    void
    setSkeleton(const SPtr<Skeleton>& skeleton);

    const SPtr<Skeleton>&
    getSkeleton() const {
      return m_skeleton;
    }

    void
    applyPose(const Pose& pose);

    void
    buildFinalBoneMatricesForMesh(const Vector<Matrix4>& meshOffsets,
                                  Vector<Matrix4>& outFinal) const;

    void
    applyPoseForMesh(const Pose& pose,
                     const Vector<Matrix4>& meshOffsets);

    const Vector<Matrix4>&
    getLocalMatrices() const {
      return m_localMatrices;
    }

    const Vector<Matrix4>&
    getGlobalMatrices() const {
      return m_globalMatrices;
    }

    const Vector<Matrix4>&
    getFinalBoneMatrices() const {
      return m_finalMatrices;
    }

   private:
    SPtr<Skeleton> m_skeleton;
    Vector<Matrix4> m_localMatrices;
    Vector<Matrix4> m_globalMatrices;
    Vector<Matrix4> m_finalMatrices;
  };

} // namespace geEngineSDK
