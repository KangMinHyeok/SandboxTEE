# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: aesm.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)




DESCRIPTOR = _descriptor.FileDescriptor(
  name='aesm.proto',
  package='',
  serialized_pb='\n\naesm.proto\"U\n\x0eGetTokenReqRaw\x12\x11\n\tsignature\x18\x01 \x02(\x0c\x12\x0b\n\x03key\x18\x02 \x02(\x0c\x12\x12\n\nattributes\x18\x03 \x02(\x0c\x12\x0f\n\x07timeout\x18\t \x02(\r\"+\n\x0bGetTokenReq\x12\x1c\n\x03req\x18\x03 \x02(\x0b\x32\x0f.GetTokenReqRaw\".\n\x0eGetTokenRetRaw\x12\r\n\x05\x65rror\x18\x01 \x02(\x05\x12\r\n\x05token\x18\x02 \x01(\x0c\"+\n\x0bGetTokenRet\x12\x1c\n\x03ret\x18\x03 \x02(\x0b\x32\x0f.GetTokenRetRaw')




_GETTOKENREQRAW = _descriptor.Descriptor(
  name='GetTokenReqRaw',
  full_name='GetTokenReqRaw',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='signature', full_name='GetTokenReqRaw.signature', index=0,
      number=1, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='key', full_name='GetTokenReqRaw.key', index=1,
      number=2, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='attributes', full_name='GetTokenReqRaw.attributes', index=2,
      number=3, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='timeout', full_name='GetTokenReqRaw.timeout', index=3,
      number=9, type=13, cpp_type=3, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=14,
  serialized_end=99,
)


_GETTOKENREQ = _descriptor.Descriptor(
  name='GetTokenReq',
  full_name='GetTokenReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='req', full_name='GetTokenReq.req', index=0,
      number=3, type=11, cpp_type=10, label=2,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=101,
  serialized_end=144,
)


