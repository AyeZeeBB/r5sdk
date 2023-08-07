// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: cl_rcon.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_cl_5frcon_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_cl_5frcon_2eproto

#include <limits>
#include <string>

#include <thirdparty/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <thirdparty/protobuf/port_undef.inc>
#include <thirdparty/protobuf/io/coded_stream.h>
#include <thirdparty/protobuf/arena.h>
#include <thirdparty/protobuf/arenastring.h>
#include <thirdparty/protobuf/generated_message_util.h>
#include <thirdparty/protobuf/metadata_lite.h>
#include <thirdparty/protobuf/message_lite.h>
#include <thirdparty/protobuf/repeated_field.h>  // IWYU pragma: export
#include <thirdparty/protobuf/extension_set.h>  // IWYU pragma: export
#include <thirdparty/protobuf/generated_enum_util.h>
// @@protoc_insertion_point(includes)
#include <thirdparty/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_cl_5frcon_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_cl_5frcon_2eproto {
  static const uint32_t offsets[];
};
namespace cl_rcon {
class request;
struct requestDefaultTypeInternal;
extern requestDefaultTypeInternal _request_default_instance_;
}  // namespace cl_rcon
PROTOBUF_NAMESPACE_OPEN
template<> ::cl_rcon::request* Arena::CreateMaybeMessage<::cl_rcon::request>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace cl_rcon {

enum request_t : int {
  SERVERDATA_REQUEST_EXECCOMMAND = 0,
  SERVERDATA_REQUEST_AUTH = 1,
  SERVERDATA_REQUEST_SEND_CONSOLE_LOG = 2,
  request_t_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  request_t_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool request_t_IsValid(int value);
constexpr request_t request_t_MIN = SERVERDATA_REQUEST_EXECCOMMAND;
constexpr request_t request_t_MAX = SERVERDATA_REQUEST_SEND_CONSOLE_LOG;
constexpr int request_t_ARRAYSIZE = request_t_MAX + 1;

const std::string& request_t_Name(request_t value);
template<typename T>
inline const std::string& request_t_Name(T enum_t_value) {
  static_assert(::std::is_same<T, request_t>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function request_t_Name.");
  return request_t_Name(static_cast<request_t>(enum_t_value));
}
bool request_t_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, request_t* value);
// ===================================================================

class request final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:cl_rcon.request) */ {
 public:
  inline request() : request(nullptr) {}
  ~request() override;
  explicit PROTOBUF_CONSTEXPR request(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  request(const request& from);
  request(request&& from) noexcept
    : request() {
    *this = ::std::move(from);
  }

  inline request& operator=(const request& from) {
    CopyFrom(from);
    return *this;
  }
  inline request& operator=(request&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const request& default_instance() {
    return *internal_default_instance();
  }
  static inline const request* internal_default_instance() {
    return reinterpret_cast<const request*>(
               &_request_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(request& a, request& b) {
    a.Swap(&b);
  }
  inline void Swap(request* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(request* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  request* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<request>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const request& from);
  void MergeFrom(const request& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(request* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "cl_rcon.request";
  }
  protected:
  explicit request(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kRequestMsgFieldNumber = 4,
    kRequestValFieldNumber = 5,
    kMessageIDFieldNumber = 1,
    kMessageTypeFieldNumber = 2,
    kRequestTypeFieldNumber = 3,
  };
  // optional string requestMsg = 4;
  bool has_requestmsg() const;
  private:
  bool _internal_has_requestmsg() const;
  public:
  void clear_requestmsg();
  const std::string& requestmsg() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_requestmsg(ArgT0&& arg0, ArgT... args);
  std::string* mutable_requestmsg();
  PROTOBUF_NODISCARD std::string* release_requestmsg();
  void set_allocated_requestmsg(std::string* requestmsg);
  private:
  const std::string& _internal_requestmsg() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_requestmsg(const std::string& value);
  std::string* _internal_mutable_requestmsg();
  public:

  // optional string requestVal = 5;
  bool has_requestval() const;
  private:
  bool _internal_has_requestval() const;
  public:
  void clear_requestval();
  const std::string& requestval() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_requestval(ArgT0&& arg0, ArgT... args);
  std::string* mutable_requestval();
  PROTOBUF_NODISCARD std::string* release_requestval();
  void set_allocated_requestval(std::string* requestval);
  private:
  const std::string& _internal_requestval() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_requestval(const std::string& value);
  std::string* _internal_mutable_requestval();
  public:

  // optional int32 messageID = 1;
  bool has_messageid() const;
  private:
  bool _internal_has_messageid() const;
  public:
  void clear_messageid();
  int32_t messageid() const;
  void set_messageid(int32_t value);
  private:
  int32_t _internal_messageid() const;
  void _internal_set_messageid(int32_t value);
  public:

  // optional int32 messageType = 2;
  bool has_messagetype() const;
  private:
  bool _internal_has_messagetype() const;
  public:
  void clear_messagetype();
  int32_t messagetype() const;
  void set_messagetype(int32_t value);
  private:
  int32_t _internal_messagetype() const;
  void _internal_set_messagetype(int32_t value);
  public:

  // optional .cl_rcon.request_t requestType = 3;
  bool has_requesttype() const;
  private:
  bool _internal_has_requesttype() const;
  public:
  void clear_requesttype();
  ::cl_rcon::request_t requesttype() const;
  void set_requesttype(::cl_rcon::request_t value);
  private:
  ::cl_rcon::request_t _internal_requesttype() const;
  void _internal_set_requesttype(::cl_rcon::request_t value);
  public:

  // @@protoc_insertion_point(class_scope:cl_rcon.request)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr requestmsg_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr requestval_;
    int32_t messageid_;
    int32_t messagetype_;
    int requesttype_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_cl_5frcon_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// request

// optional int32 messageID = 1;
inline bool request::_internal_has_messageid() const {
  bool value = (_impl_._has_bits_[0] & 0x00000004u) != 0;
  return value;
}
inline bool request::has_messageid() const {
  return _internal_has_messageid();
}
inline void request::clear_messageid() {
  _impl_.messageid_ = 0;
  _impl_._has_bits_[0] &= ~0x00000004u;
}
inline int32_t request::_internal_messageid() const {
  return _impl_.messageid_;
}
inline int32_t request::messageid() const {
  // @@protoc_insertion_point(field_get:cl_rcon.request.messageID)
  return _internal_messageid();
}
inline void request::_internal_set_messageid(int32_t value) {
  _impl_._has_bits_[0] |= 0x00000004u;
  _impl_.messageid_ = value;
}
inline void request::set_messageid(int32_t value) {
  _internal_set_messageid(value);
  // @@protoc_insertion_point(field_set:cl_rcon.request.messageID)
}

// optional int32 messageType = 2;
inline bool request::_internal_has_messagetype() const {
  bool value = (_impl_._has_bits_[0] & 0x00000008u) != 0;
  return value;
}
inline bool request::has_messagetype() const {
  return _internal_has_messagetype();
}
inline void request::clear_messagetype() {
  _impl_.messagetype_ = 0;
  _impl_._has_bits_[0] &= ~0x00000008u;
}
inline int32_t request::_internal_messagetype() const {
  return _impl_.messagetype_;
}
inline int32_t request::messagetype() const {
  // @@protoc_insertion_point(field_get:cl_rcon.request.messageType)
  return _internal_messagetype();
}
inline void request::_internal_set_messagetype(int32_t value) {
  _impl_._has_bits_[0] |= 0x00000008u;
  _impl_.messagetype_ = value;
}
inline void request::set_messagetype(int32_t value) {
  _internal_set_messagetype(value);
  // @@protoc_insertion_point(field_set:cl_rcon.request.messageType)
}

// optional .cl_rcon.request_t requestType = 3;
inline bool request::_internal_has_requesttype() const {
  bool value = (_impl_._has_bits_[0] & 0x00000010u) != 0;
  return value;
}
inline bool request::has_requesttype() const {
  return _internal_has_requesttype();
}
inline void request::clear_requesttype() {
  _impl_.requesttype_ = 0;
  _impl_._has_bits_[0] &= ~0x00000010u;
}
inline ::cl_rcon::request_t request::_internal_requesttype() const {
  return static_cast< ::cl_rcon::request_t >(_impl_.requesttype_);
}
inline ::cl_rcon::request_t request::requesttype() const {
  // @@protoc_insertion_point(field_get:cl_rcon.request.requestType)
  return _internal_requesttype();
}
inline void request::_internal_set_requesttype(::cl_rcon::request_t value) {
  _impl_._has_bits_[0] |= 0x00000010u;
  _impl_.requesttype_ = value;
}
inline void request::set_requesttype(::cl_rcon::request_t value) {
  _internal_set_requesttype(value);
  // @@protoc_insertion_point(field_set:cl_rcon.request.requestType)
}

// optional string requestMsg = 4;
inline bool request::_internal_has_requestmsg() const {
  bool value = (_impl_._has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool request::has_requestmsg() const {
  return _internal_has_requestmsg();
}
inline void request::clear_requestmsg() {
  _impl_.requestmsg_.ClearToEmpty();
  _impl_._has_bits_[0] &= ~0x00000001u;
}
inline const std::string& request::requestmsg() const {
  // @@protoc_insertion_point(field_get:cl_rcon.request.requestMsg)
  return _internal_requestmsg();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void request::set_requestmsg(ArgT0&& arg0, ArgT... args) {
 _impl_._has_bits_[0] |= 0x00000001u;
 _impl_.requestmsg_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:cl_rcon.request.requestMsg)
}
inline std::string* request::mutable_requestmsg() {
  std::string* _s = _internal_mutable_requestmsg();
  // @@protoc_insertion_point(field_mutable:cl_rcon.request.requestMsg)
  return _s;
}
inline const std::string& request::_internal_requestmsg() const {
  return _impl_.requestmsg_.Get();
}
inline void request::_internal_set_requestmsg(const std::string& value) {
  _impl_._has_bits_[0] |= 0x00000001u;
  _impl_.requestmsg_.Set(value, GetArenaForAllocation());
}
inline std::string* request::_internal_mutable_requestmsg() {
  _impl_._has_bits_[0] |= 0x00000001u;
  return _impl_.requestmsg_.Mutable(GetArenaForAllocation());
}
inline std::string* request::release_requestmsg() {
  // @@protoc_insertion_point(field_release:cl_rcon.request.requestMsg)
  if (!_internal_has_requestmsg()) {
    return nullptr;
  }
  _impl_._has_bits_[0] &= ~0x00000001u;
  auto* p = _impl_.requestmsg_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.requestmsg_.IsDefault()) {
    _impl_.requestmsg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void request::set_allocated_requestmsg(std::string* requestmsg) {
  if (requestmsg != nullptr) {
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }
  _impl_.requestmsg_.SetAllocated(requestmsg, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.requestmsg_.IsDefault()) {
    _impl_.requestmsg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:cl_rcon.request.requestMsg)
}

// optional string requestVal = 5;
inline bool request::_internal_has_requestval() const {
  bool value = (_impl_._has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool request::has_requestval() const {
  return _internal_has_requestval();
}
inline void request::clear_requestval() {
  _impl_.requestval_.ClearToEmpty();
  _impl_._has_bits_[0] &= ~0x00000002u;
}
inline const std::string& request::requestval() const {
  // @@protoc_insertion_point(field_get:cl_rcon.request.requestVal)
  return _internal_requestval();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void request::set_requestval(ArgT0&& arg0, ArgT... args) {
 _impl_._has_bits_[0] |= 0x00000002u;
 _impl_.requestval_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:cl_rcon.request.requestVal)
}
inline std::string* request::mutable_requestval() {
  std::string* _s = _internal_mutable_requestval();
  // @@protoc_insertion_point(field_mutable:cl_rcon.request.requestVal)
  return _s;
}
inline const std::string& request::_internal_requestval() const {
  return _impl_.requestval_.Get();
}
inline void request::_internal_set_requestval(const std::string& value) {
  _impl_._has_bits_[0] |= 0x00000002u;
  _impl_.requestval_.Set(value, GetArenaForAllocation());
}
inline std::string* request::_internal_mutable_requestval() {
  _impl_._has_bits_[0] |= 0x00000002u;
  return _impl_.requestval_.Mutable(GetArenaForAllocation());
}
inline std::string* request::release_requestval() {
  // @@protoc_insertion_point(field_release:cl_rcon.request.requestVal)
  if (!_internal_has_requestval()) {
    return nullptr;
  }
  _impl_._has_bits_[0] &= ~0x00000002u;
  auto* p = _impl_.requestval_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.requestval_.IsDefault()) {
    _impl_.requestval_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void request::set_allocated_requestval(std::string* requestval) {
  if (requestval != nullptr) {
    _impl_._has_bits_[0] |= 0x00000002u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000002u;
  }
  _impl_.requestval_.SetAllocated(requestval, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.requestval_.IsDefault()) {
    _impl_.requestval_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:cl_rcon.request.requestVal)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace cl_rcon

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::cl_rcon::request_t> : ::std::true_type {};

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <thirdparty/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_cl_5frcon_2eproto
