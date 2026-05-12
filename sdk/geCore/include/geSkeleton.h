#pragma once

#include <gePrerequisitesCore.h>
#include <geResource.h>
#include <geMatrix4.h>
#include <geTransform.h>

#include <algorithm>
#include <utility>

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

    // Assimp aiBone::mOffsetMatrix.
    // Runtime skinning should prefer SkinBinding::m_boneOffsets because the
    // inverse bind matrix can be mesh/submesh-specific.
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

    if (keys.size() == 1) {
      return 0;
    }

    // Returns the last key whose time is <= requested time. This is O(log n)
    // instead of scanning every key every frame.
    auto it = std::upper_bound(keys.begin(),
                               keys.end(),
                               time,
                               [](float t, const KeyFrame<T>& key) {
                                 return t < key.time;
                               });

    if (it == keys.begin()) {
      return 0;
    }

    return cast::st<uint32>((it - keys.begin()) - 1);
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

  struct NamedBoneTrack
  {
    String boneName;
    BoneTrack track;
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

    SIZE_T
    getSkeletonHash() const {
      return m_skeletonHash;
    }

    void
    clear();

    bool
    validateHierarchy() const;

    void
    rebuildBindGlobals();

    SIZE_T
    calculateHierarchyHash() const;

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

    // Parent-before-child update order. This lets SkeletonInstance update
    // globals with a flat loop instead of recursive visited checks.
    Vector<BoneIndex> m_updateOrder;

    Matrix4 m_globalInverseTransform = Matrix4::IDENTITY;
    SIZE_T m_skeletonHash = 0;
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

    SIZE_T
    getSkeletonHash() const {
      return m_skeletonHash;
    }

    const BoneTrack*
    getTrack(BoneIndex boneIndex) const {
      if (boneIndex >= m_boneTracks.size() || boneIndex >= m_hasTrack.size()) {
        return nullptr;
      }

      return m_hasTrack[boneIndex] ? &m_boneTracks[boneIndex] : nullptr;
    }

    void
    resetForSkeleton(const Skeleton& skeleton);

    void
    setTrack(BoneIndex boneIndex, const BoneTrack& track);

    void
    setTrack(BoneIndex boneIndex, BoneTrack&& track);

    GE_NODISCARD bool
    isCompatibleWith(const Skeleton& skeleton) const;

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

    // Dense arrays indexed directly by BoneIndex. Much cheaper than Map in
    // AnimationPlayer::update and safer once compiled against a Skeleton.
    Vector<BoneTrack> m_boneTracks;
    Vector<uint8> m_hasTrack;
    SIZE_T m_skeletonHash = 0;
  };

  class GE_CORE_EXPORT RawAnimationClip : public Resource
  {
   public:
    RawAnimationClip() = default;
    ~RawAnimationClip() override = default;

    SPtr<AnimationClip>
    compileForSkeleton(const SPtr<Skeleton>& skeleton) const;

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

    // Stored by node/bone name as imported. This is the safe representation for
    // animation files loaded without knowing the final model Skeleton yet.
    Vector<NamedBoneTrack> m_tracks;
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
    pause(bool bPause = true);

    void
    update(float deltaTime);

    const Pose&
    getCurrentPose() const {
      return m_pose;
    }

    GE_NODISCARD bool
    isPlaying() const {
      return m_isPlaying;
    }

   private:
    SPtr<AnimationClip> m_currentClip;
    SPtr<Skeleton> m_skeleton;
    float m_currentTime = 0.0f;
    bool m_isLooping = true;
    bool m_isPlaying = false;
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
                                  Vector<Matrix4>& outFinal,
                                  bool transposeForGPU = true) const;

    void
    applyPoseForMesh(const Pose& pose,
                     const Vector<Matrix4>& meshOffsets,
                     bool transposeForGPU = true);

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
