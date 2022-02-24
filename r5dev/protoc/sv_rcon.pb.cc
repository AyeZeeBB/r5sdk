// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sv_rcon.proto

#include "sv_rcon.pb.h"

#include <algorithm>

#include <thirdparty/protobuf/io/coded_stream.h>
#include <thirdparty/protobuf/extension_set.h>
#include <thirdparty/protobuf/wire_format_lite.h>
#include <thirdparty/protobuf/io/zero_copy_stream_impl_lite.h>
// @@protoc_insertion_point(includes)
#include <thirdparty/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace sv_rcon {
constexpr response::response(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : responsebuf_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , responseval_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , responseid_(0)
  , responsetype_(0)
{}
struct responseDefaultTypeInternal {
  constexpr responseDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~responseDefaultTypeInternal() {}
  union {
    response _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT responseDefaultTypeInternal _response_default_instance_;
}  // namespace sv_rcon
namespace sv_rcon {
bool response_t_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}

static ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<std::string> response_t_strings[6] = {};

static const char response_t_names[] =
  "SERVERDATA_RESPONSE_AUTH"
  "SERVERDATA_RESPONSE_CONSOLE_LOG"
  "SERVERDATA_RESPONSE_REMOTEBUG"
  "SERVERDATA_RESPONSE_STRING"
  "SERVERDATA_RESPONSE_UPDATE"
  "SERVERDATA_RESPONSE_VALUE";

static const ::PROTOBUF_NAMESPACE_ID::internal::EnumEntry response_t_entries[] = {
  { {response_t_names + 0, 24}, 2 },
  { {response_t_names + 24, 31}, 3 },
  { {response_t_names + 55, 29}, 5 },
  { {response_t_names + 84, 26}, 4 },
  { {response_t_names + 110, 26}, 1 },
  { {response_t_names + 136, 25}, 0 },
};

static const int response_t_entries_by_number[] = {
  5, // 0 -> SERVERDATA_RESPONSE_VALUE
  4, // 1 -> SERVERDATA_RESPONSE_UPDATE
  0, // 2 -> SERVERDATA_RESPONSE_AUTH
  1, // 3 -> SERVERDATA_RESPONSE_CONSOLE_LOG
  3, // 4 -> SERVERDATA_RESPONSE_STRING
  2, // 5 -> SERVERDATA_RESPONSE_REMOTEBUG
};

const std::string& response_t_Name(
    response_t value) {
  static const bool dummy =
      ::PROTOBUF_NAMESPACE_ID::internal::InitializeEnumStrings(
          response_t_entries,
          response_t_entries_by_number,
          6, response_t_strings);
  (void) dummy;
  int idx = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumName(
      response_t_entries,
      response_t_entries_by_number,
      6, value);
  return idx == -1 ? ::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString() :
                     response_t_strings[idx].get();
}
bool response_t_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, response_t* value) {
  int int_value;
  bool success = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumValue(
      response_t_entries, 6, name, &int_value);
  if (success) {
    *value = static_cast<response_t>(int_value);
  }
  return success;
}

// ===================================================================

class response::_Internal {
 public:
  using HasBits = decltype(std::declval<response>()._has_bits_);
  static void set_has_responseid(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_responsetype(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_responsebuf(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_responseval(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

response::response(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:sv_rcon.response)
}
response::response(const response& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  responsebuf_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    responsebuf_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), "", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (from._internal_has_responsebuf()) {
    responsebuf_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_responsebuf(), 
      GetArenaForAllocation());
  }
  responseval_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    responseval_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), "", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (from._internal_has_responseval()) {
    responseval_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_responseval(), 
      GetArenaForAllocation());
  }
  ::memcpy(&responseid_, &from.responseid_,
    static_cast<size_t>(reinterpret_cast<char*>(&responsetype_) -
    reinterpret_cast<char*>(&responseid_)) + sizeof(responsetype_));
  // @@protoc_insertion_point(copy_constructor:sv_rcon.response)
}

inline void response::SharedCtor() {
responsebuf_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  responsebuf_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), "", GetArenaForAllocation());
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
responseval_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  responseval_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), "", GetArenaForAllocation());
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&responseid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&responsetype_) -
    reinterpret_cast<char*>(&responseid_)) + sizeof(responsetype_));
}

response::~response() {
  // @@protoc_insertion_point(destructor:sv_rcon.response)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

inline void response::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  responsebuf_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  responseval_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void response::ArenaDtor(void* object) {
  response* _this = reinterpret_cast< response* >(object);
  (void)_this;
}
void response::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void response::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void response::Clear() {
// @@protoc_insertion_point(message_clear_start:sv_rcon.response)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      responsebuf_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      responseval_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x0000000cu) {
    ::memset(&responseid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&responsetype_) -
        reinterpret_cast<char*>(&responseid_)) + sizeof(responsetype_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* response::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // optional int32 responseID = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _Internal::set_has_responseid(&has_bits);
          responseid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional .sv_rcon.response_t responseType = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_responsetype(static_cast<::sv_rcon::response_t>(val));
        } else
          goto handle_unusual;
        continue;
      // optional string responseBuf = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          auto str = _internal_mutable_responsebuf();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, nullptr));
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional string responseVal = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 34)) {
          auto str = _internal_mutable_responseval();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, nullptr));
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<std::string>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* response::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:sv_rcon.response)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // optional int32 responseID = 1;
  if (_internal_has_responseid()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_responseid(), target);
  }

  // optional .sv_rcon.response_t responseType = 2;
  if (_internal_has_responsetype()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      2, this->_internal_responsetype(), target);
  }

  // optional string responseBuf = 3;
  if (_internal_has_responsebuf()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_responsebuf().data(), static_cast<int>(this->_internal_responsebuf().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "sv_rcon.response.responseBuf");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_responsebuf(), target);
  }

  // optional string responseVal = 4;
  if (_internal_has_responseval()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_responseval().data(), static_cast<int>(this->_internal_responseval().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "sv_rcon.response.responseVal");
    target = stream->WriteStringMaybeAliased(
        4, this->_internal_responseval(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:sv_rcon.response)
  return target;
}

size_t response::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:sv_rcon.response)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000000fu) {
    // optional string responseBuf = 3;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_responsebuf());
    }

    // optional string responseVal = 4;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_responseval());
    }

    // optional int32 responseID = 1;
    if (cached_has_bits & 0x00000004u) {
      total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32SizePlusOne(this->_internal_responseid());
    }

    // optional .sv_rcon.response_t responseType = 2;
    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_responsetype());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void response::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const response*>(
      &from));
}

void response::MergeFrom(const response& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:sv_rcon.response)
  GOOGLE_DCHECK_NE(&from, this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x0000000fu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_responsebuf(from._internal_responsebuf());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_responseval(from._internal_responseval());
    }
    if (cached_has_bits & 0x00000004u) {
      responseid_ = from.responseid_;
    }
    if (cached_has_bits & 0x00000008u) {
      responsetype_ = from.responsetype_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
}

void response::CopyFrom(const response& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:sv_rcon.response)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool response::IsInitialized() const {
  return true;
}

void response::InternalSwap(response* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &responsebuf_, lhs_arena,
      &other->responsebuf_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &responseval_, lhs_arena,
      &other->responseval_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(response, responsetype_)
      + sizeof(response::responsetype_)
      - PROTOBUF_FIELD_OFFSET(response, responseid_)>(
          reinterpret_cast<char*>(&responseid_),
          reinterpret_cast<char*>(&other->responseid_));
}

std::string response::GetTypeName() const {
  return "sv_rcon.response";
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace sv_rcon
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::sv_rcon::response* Arena::CreateMaybeMessage< ::sv_rcon::response >(Arena* arena) {
  return Arena::CreateMessageInternal< ::sv_rcon::response >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <thirdparty/protobuf/port_undef.inc>
