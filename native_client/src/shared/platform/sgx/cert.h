#define my_cert "/home/hmlee/iitp_cert.pem"
#define my_key "/home/hmlee/iitp_key.pem"

uint8_t ca_cert[] = {0x30, 0x82, 0x5, 0xd8, 0x30, 0x82, 0x3, 0xc0, 0xa0, 0x3, 0x2, 0x1, 0x2, 0x2, 0x10, 0x4c, 0xaa, 0xf9, 0xca, 0xdb, 0x63, 0x6f, 0xe0, 0x1f, 0xf7, 0x4e, 0xd8, 0x5b, 0x3, 0x86, 0x9d, 0x30, 0xd, 0x6, 0x9, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0xd, 0x1, 0x1, 0xc, 0x5, 0x0, 0x30, 0x81, 0x85, 0x31, 0xb, 0x30, 0x9, 0x6, 0x3, 0x55, 0x4, 0x6, 0x13, 0x2, 0x47, 0x42, 0x31, 0x1b, 0x30, 0x19, 0x6, 0x3, 0x55, 0x4, 0x8, 0x13, 0x12, 0x47, 0x72, 0x65, 0x61, 0x74, 0x65, 0x72, 0x20, 0x4d, 0x61, 0x6e, 0x63, 0x68, 0x65, 0x73, 0x74, 0x65, 0x72, 0x31, 0x10, 0x30, 0xe, 0x6, 0x3, 0x55, 0x4, 0x7, 0x13, 0x7, 0x53, 0x61, 0x6c, 0x66, 0x6f, 0x72, 0x64, 0x31, 0x1a, 0x30, 0x18, 0x6, 0x3, 0x55, 0x4, 0xa, 0x13, 0x11, 0x43, 0x4f, 0x4d, 0x4f, 0x44, 0x4f, 0x20, 0x43, 0x41, 0x20, 0x4c, 0x69, 0x6d, 0x69, 0x74, 0x65, 0x64, 0x31, 0x2b, 0x30, 0x29, 0x6, 0x3, 0x55, 0x4, 0x3, 0x13, 0x22, 0x43, 0x4f, 0x4d, 0x4f, 0x44, 0x4f, 0x20, 0x52, 0x53, 0x41, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x30, 0x1e, 0x17, 0xd, 0x31, 0x30, 0x30, 0x31, 0x31, 0x39, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x17, 0xd, 0x33, 0x38, 0x30, 0x31, 0x31, 0x38, 0x32, 0x33, 0x35, 0x39, 0x35, 0x39, 0x5a, 0x30, 0x81, 0x85, 0x31, 0xb, 0x30, 0x9, 0x6, 0x3, 0x55, 0x4, 0x6, 0x13, 0x2, 0x47, 0x42, 0x31, 0x1b, 0x30, 0x19, 0x6, 0x3, 0x55, 0x4, 0x8, 0x13, 0x12, 0x47, 0x72, 0x65, 0x61, 0x74, 0x65, 0x72, 0x20, 0x4d, 0x61, 0x6e, 0x63, 0x68, 0x65, 0x73, 0x74, 0x65, 0x72, 0x31, 0x10, 0x30, 0xe, 0x6, 0x3, 0x55, 0x4, 0x7, 0x13, 0x7, 0x53, 0x61, 0x6c, 0x66, 0x6f, 0x72, 0x64, 0x31, 0x1a, 0x30, 0x18, 0x6, 0x3, 0x55, 0x4, 0xa, 0x13, 0x11, 0x43, 0x4f, 0x4d, 0x4f, 0x44, 0x4f, 0x20, 0x43, 0x41, 0x20, 0x4c, 0x69, 0x6d, 0x69, 0x74, 0x65, 0x64, 0x31, 0x2b, 0x30, 0x29, 0x6, 0x3, 0x55, 0x4, 0x3, 0x13, 0x22, 0x43, 0x4f, 0x4d, 0x4f, 0x44, 0x4f, 0x20, 0x52, 0x53, 0x41, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x30, 0x82, 0x2, 0x22, 0x30, 0xd, 0x6, 0x9, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0xd, 0x1, 0x1, 0x1, 0x5, 0x0, 0x3, 0x82, 0x2, 0xf, 0x0, 0x30, 0x82, 0x2, 0xa, 0x2, 0x82, 0x2, 0x1, 0x0, 0x91, 0xe8, 0x54, 0x92, 0xd2, 0xa, 0x56, 0xb1, 0xac, 0xd, 0x24, 0xdd, 0xc5, 0xcf, 0x44, 0x67, 0x74, 0x99, 0x2b, 0x37, 0xa3, 0x7d, 0x23, 0x70, 0x0, 0x71, 0xbc, 0x53, 0xdf, 0xc4, 0xfa, 0x2a, 0x12, 0x8f, 0x4b, 0x7f, 0x10, 0x56, 0xbd, 0x9f, 0x70, 0x72, 0xb7, 0x61, 0x7f, 0xc9, 0x4b, 0xf, 0x17, 0xa7, 0x3d, 0xe3, 0xb0, 0x4, 0x61, 0xee, 0xff, 0x11, 0x97, 0xc7, 0xf4, 0x86, 0x3e, 0xa, 0xfa, 0x3e, 0x5c, 0xf9, 0x93, 0xe6, 0x34, 0x7a, 0xd9, 0x14, 0x6b, 0xe7, 0x9c, 0xb3, 0x85, 0xa0, 0x82, 0x7a, 0x76, 0xaf, 0x71, 0x90, 0xd7, 0xec, 0xfd, 0xd, 0xfa, 0x9c, 0x6c, 0xfa, 0xdf, 0xb0, 0x82, 0xf4, 0x14, 0x7e, 0xf9, 0xbe, 0xc4, 0xa6, 0x2f, 0x4f, 0x7f, 0x99, 0x7f, 0xb5, 0xfc, 0x67, 0x43, 0x72, 0xbd, 0xc, 0x0, 0xd6, 0x89, 0xeb, 0x6b, 0x2c, 0xd3, 0xed, 0x8f, 0x98, 0x1c, 0x14, 0xab, 0x7e, 0xe5, 0xe3, 0x6e, 0xfc, 0xd8, 0xa8, 0xe4, 0x92, 0x24, 0xda, 0x43, 0x6b, 0x62, 0xb8, 0x55, 0xfd, 0xea, 0xc1, 0xbc, 0x6c, 0xb6, 0x8b, 0xf3, 0xe, 0x8d, 0x9a, 0xe4, 0x9b, 0x6c, 0x69, 0x99, 0xf8, 0x78, 0x48, 0x30, 0x45, 0xd5, 0xad, 0xe1, 0xd, 0x3c, 0x45, 0x60, 0xfc, 0x32, 0x96, 0x51, 0x27, 0xbc, 0x67, 0xc3, 0xca, 0x2e, 0xb6, 0x6b, 0xea, 0x46, 0xc7, 0xc7, 0x20, 0xa0, 0xb1, 0x1f, 0x65, 0xde, 0x48, 0x8, 0xba, 0xa4, 0x4e, 0xa9, 0xf2, 0x83, 0x46, 0x37, 0x84, 0xeb, 0xe8, 0xcc, 0x81, 0x48, 0x43, 0x67, 0x4e, 0x72, 0x2a, 0x9b, 0x5c, 0xbd, 0x4c, 0x1b, 0x28, 0x8a, 0x5c, 0x22, 0x7b, 0xb4, 0xab, 0x98, 0xd9, 0xee, 0xe0, 0x51, 0x83, 0xc3, 0x9, 0x46, 0x4e, 0x6d, 0x3e, 0x99, 0xfa, 0x95, 0x17, 0xda, 0x7c, 0x33, 0x57, 0x41, 0x3c, 0x8d, 0x51, 0xed, 0xb, 0xb6, 0x5c, 0xaf, 0x2c, 0x63, 0x1a, 0xdf, 0x57, 0xc8, 0x3f, 0xbc, 0xe9, 0x5d, 0xc4, 0x9b, 0xaf, 0x45, 0x99, 0xe2, 0xa3, 0x5a, 0x24, 0xb4, 0xba, 0xa9, 0x56, 0x3d, 0xcf, 0x6f, 0xaa, 0xff, 0x49, 0x58, 0xbe, 0xf0, 0xa8, 0xff, 0xf4, 0xb8, 0xad, 0xe9, 0x37, 0xfb, 0xba, 0xb8, 0xf4, 0xb, 0x3a, 0xf9, 0xe8, 0x43, 0x42, 0x1e, 0x89, 0xd8, 0x84, 0xcb, 0x13, 0xf1, 0xd9, 0xbb, 0xe1, 0x89, 0x60, 0xb8, 0x8c, 0x28, 0x56, 0xac, 0x14, 0x1d, 0x9c, 0xa, 0xe7, 0x71, 0xeb, 0xcf, 0xe, 0xdd, 0x3d, 0xa9, 0x96, 0xa1, 0x48, 0xbd, 0x3c, 0xf7, 0xaf, 0xb5, 0xd, 0x22, 0x4c, 0xc0, 0x11, 0x81, 0xec, 0x56, 0x3b, 0xf6, 0xd3, 0xa2, 0xe2, 0x5b, 0xb7, 0xb2, 0x4, 0x22, 0x52, 0x95, 0x80, 0x93, 0x69, 0xe8, 0x8e, 0x4c, 0x65, 0xf1, 0x91, 0x3, 0x2d, 0x70, 0x74, 0x2, 0xea, 0x8b, 0x67, 0x15, 0x29, 0x69, 0x52, 0x2, 0xbb, 0xd7, 0xdf, 0x50, 0x6a, 0x55, 0x46, 0xbf, 0xa0, 0xa3, 0x28, 0x61, 0x7f, 0x70, 0xd0, 0xc3, 0xa2, 0xaa, 0x2c, 0x21, 0xaa, 0x47, 0xce, 0x28, 0x9c, 0x6, 0x45, 0x76, 0xbf, 0x82, 0x18, 0x27, 0xb4, 0xd5, 0xae, 0xb4, 0xcb, 0x50, 0xe6, 0x6b, 0xf4, 0x4c, 0x86, 0x71, 0x30, 0xe9, 0xa6, 0xdf, 0x16, 0x86, 0xe0, 0xd8, 0xff, 0x40, 0xdd, 0xfb, 0xd0, 0x42, 0x88, 0x7f, 0xa3, 0x33, 0x3a, 0x2e, 0x5c, 0x1e, 0x41, 0x11, 0x81, 0x63, 0xce, 0x18, 0x71, 0x6b, 0x2b, 0xec, 0xa6, 0x8a, 0xb7, 0x31, 0x5c, 0x3a, 0x6a, 0x47, 0xe0, 0xc3, 0x79, 0x59, 0xd6, 0x20, 0x1a, 0xaf, 0xf2, 0x6a, 0x98, 0xaa, 0x72, 0xbc, 0x57, 0x4a, 0xd2, 0x4b, 0x9d, 0xbb, 0x10, 0xfc, 0xb0, 0x4c, 0x41, 0xe5, 0xed, 0x1d, 0x3d, 0x5e, 0x28, 0x9d, 0x9c, 0xcc, 0xbf, 0xb3, 0x51, 0xda, 0xa7, 0x47, 0xe5, 0x84, 0x53, 0x2, 0x3, 0x1, 0x0, 0x1, 0xa3, 0x42, 0x30, 0x40, 0x30, 0x1d, 0x6, 0x3, 0x55, 0x1d, 0xe, 0x4, 0x16, 0x4, 0x14, 0xbb, 0xaf, 0x7e, 0x2, 0x3d, 0xfa, 0xa6, 0xf1, 0x3c, 0x84, 0x8e, 0xad, 0xee, 0x38, 0x98, 0xec, 0xd9, 0x32, 0x32, 0xd4, 0x30, 0xe, 0x6, 0x3, 0x55, 0x1d, 0xf, 0x1, 0x1, 0xff, 0x4, 0x4, 0x3, 0x2, 0x1, 0x6, 0x30, 0xf, 0x6, 0x3, 0x55, 0x1d, 0x13, 0x1, 0x1, 0xff, 0x4, 0x5, 0x30, 0x3, 0x1, 0x1, 0xff, 0x30, 0xd, 0x6, 0x9, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0xd, 0x1, 0x1, 0xc, 0x5, 0x0, 0x3, 0x82, 0x2, 0x1, 0x0, 0xa, 0xf1, 0xd5, 0x46, 0x84, 0xb7, 0xae, 0x51, 0xbb, 0x6c, 0xb2, 0x4d, 0x41, 0x14, 0x0, 0x93, 0x4c, 0x9c, 0xcb, 0xe5, 0xc0, 0x54, 0xcf, 0xa0, 0x25, 0x8e, 0x2, 0xf9, 0xfd, 0xb0, 0xa2, 0xd, 0xf5, 0x20, 0x98, 0x3c, 0x13, 0x2d, 0xac, 0x56, 0xa2, 0xb0, 0xd6, 0x7e, 0x11, 0x92, 0xe9, 0x2e, 0xba, 0x9e, 0x2e, 0x9a, 0x72, 0xb1, 0xbd, 0x19, 0x44, 0x6c, 0x61, 0x35, 0xa2, 0x9a, 0xb4, 0x16, 0x12, 0x69, 0x5a, 0x8c, 0xe1, 0xd7, 0x3e, 0xa4, 0x1a, 0xe8, 0x2f, 0x3, 0xf4, 0xae, 0x61, 0x1d, 0x10, 0x1b, 0x2a, 0xa4, 0x8b, 0x7a, 0xc5, 0xfe, 0x5, 0xa6, 0xe1, 0xc0, 0xd6, 0xc8, 0xfe, 0x9e, 0xae, 0x8f, 0x2b, 0xba, 0x3d, 0x99, 0xf8, 0xd8, 0x73, 0x9, 0x58, 0x46, 0x6e, 0xa6, 0x9c, 0xf4, 0xd7, 0x27, 0xd3, 0x95, 0xda, 0x37, 0x83, 0x72, 0x1c, 0xd3, 0x73, 0xe0, 0xa2, 0x47, 0x99, 0x3, 0x38, 0x5d, 0xd5, 0x49, 0x79, 0x0, 0x29, 0x1c, 0xc7, 0xec, 0x9b, 0x20, 0x1c, 0x7, 0x24, 0x69, 0x57, 0x78, 0xb2, 0x39, 0xfc, 0x3a, 0x84, 0xa0, 0xb5, 0x9c, 0x7c, 0x8d, 0xbf, 0x2e, 0x93, 0x62, 0x27, 0xb7, 0x39, 0xda, 0x17, 0x18, 0xae, 0xbd, 0x3c, 0x9, 0x68, 0xff, 0x84, 0x9b, 0x3c, 0xd5, 0xd6, 0xb, 0x3, 0xe3, 0x57, 0x9e, 0x14, 0xf7, 0xd1, 0xeb, 0x4f, 0xc8, 0xbd, 0x87, 0x23, 0xb7, 0xb6, 0x49, 0x43, 0x79, 0x85, 0x5c, 0xba, 0xeb, 0x92, 0xb, 0xa1, 0xc6, 0xe8, 0x68, 0xa8, 0x4c, 0x16, 0xb1, 0x1a, 0x99, 0xa, 0xe8, 0x53, 0x2c, 0x92, 0xbb, 0xa1, 0x9, 0x18, 0x75, 0xc, 0x65, 0xa8, 0x7b, 0xcb, 0x23, 0xb7, 0x1a, 0xc2, 0x28, 0x85, 0xc3, 0x1b, 0xff, 0xd0, 0x2b, 0x62, 0xef, 0xa4, 0x7b, 0x9, 0x91, 0x98, 0x67, 0x8c, 0x14, 0x1, 0xcd, 0x68, 0x6, 0x6a, 0x63, 0x21, 0x75, 0x3, 0x80, 0x88, 0x8a, 0x6e, 0x81, 0xc6, 0x85, 0xf2, 0xa9, 0xa4, 0x2d, 0xe7, 0xf4, 0xa5, 0x24, 0x10, 0x47, 0x83, 0xca, 0xcd, 0xf4, 0x8d, 0x79, 0x58, 0xb1, 0x6, 0x9b, 0xe7, 0x1a, 0x2a, 0xd9, 0x9d, 0x1, 0xd7, 0x94, 0x7d, 0xed, 0x3, 0x4a, 0xca, 0xf0, 0xdb, 0xe8, 0xa9, 0x1, 0x3e, 0xf5, 0x56, 0x99, 0xc9, 0x1e, 0x8e, 0x49, 0x3d, 0xbb, 0xe5, 0x9, 0xb9, 0xe0, 0x4f, 0x49, 0x92, 0x3d, 0x16, 0x82, 0x40, 0xcc, 0xcc, 0x59, 0xc6, 0xe6, 0x3a, 0xed, 0x12, 0x2e, 0x69, 0x3c, 0x6c, 0x95, 0xb1, 0xfd, 0xaa, 0x1d, 0x7b, 0x7f, 0x86, 0xbe, 0x1e, 0xe, 0x32, 0x46, 0xfb, 0xfb, 0x13, 0x8f, 0x75, 0x7f, 0x4c, 0x8b, 0x4b, 0x46, 0x63, 0xfe, 0x0, 0x34, 0x40, 0x70, 0xc1, 0xc3, 0xb9, 0xa1, 0xdd, 0xa6, 0x70, 0xe2, 0x4, 0xb3, 0x41, 0xbc, 0xe9, 0x80, 0x91, 0xea, 0x64, 0x9c, 0x7a, 0xe1, 0x22, 0x3, 0xa9, 0x9c, 0x6e, 0x6f, 0xe, 0x65, 0x4f, 0x6c, 0x87, 0x87, 0x5e, 0xf3, 0x6e, 0xa0, 0xf9, 0x75, 0xa5, 0x9b, 0x40, 0xe8, 0x53, 0xb2, 0x27, 0x9d, 0x4a, 0xb9, 0xc0, 0x77, 0x21, 0x8d, 0xff, 0x87, 0xf2, 0xde, 0xbc, 0x8c, 0xef, 0x17, 0xdf, 0xb7, 0x49, 0xb, 0xd1, 0xf2, 0x6e, 0x30, 0xb, 0x1a, 0xe, 0x4e, 0x76, 0xed, 0x11, 0xfc, 0xf5, 0xe9, 0x56, 0xb2, 0x7d, 0xbf, 0xc7, 0x6d, 0xa, 0x93, 0x8c, 0xa5, 0xd0, 0xc0, 0xb6, 0x1d, 0xbe, 0x3a, 0x4e, 0x94, 0xa2, 0xd7, 0x6e, 0x6c, 0xb, 0xc2, 0x8a, 0x7c, 0xfa, 0x20, 0xf3, 0xc4, 0xe4, 0xe5, 0xcd, 0xd, 0xa8, 0xcb, 0x91, 0x92, 0xb1, 0x7c, 0x85, 0xec, 0xb5, 0x14, 0x69, 0x66, 0xe, 0x82, 0xe7, 0xcd, 0xce, 0xc8, 0x2d, 0xa6, 0x51, 0x7f, 0x21, 0xc1, 0x35, 0x53, 0x85, 0x6, 0x4a, 0x5d, 0x9f, 0xad, 0xbb, 0x1b, 0x5f, 0x74};

/*
const unsigned char ca_cert[] = {
"-----BEGIN CERTIFICATE-----\n\
MIIEEzCCAvugAwIBAgIUVNWbRdPxVh73V8/oLuIpybfnbTMwDQYJKoZIhvcNAQEL\n\
BQAwgbExCzAJBgNVBAYTAktSMQ4wDAYDVQQIDAVTZW91bDEOMAwGA1UEBwwFU2Vv\n\
dWwxIjAgBgNVBAoMGVNlb3VsIE5hdGlvbmFsIFVuaXZlcnNpdHkxIjAgBgNVBAsM\n\
GVNlY3VyaXR5IE9wdGltaXphdGlvbiBMYWIxIjAgBgkqhkiG9w0BCQEWE2tybGVl\n\
QHNvci5zbnUuYWMua3IxFjAUBgNVBAMMDUF1dGggQWdlbnQgQ0EwHhcNMjEwODEw\n\
MTI0MzQ0WhcNMzEwODA4MTI0MzQ0WjCBsTELMAkGA1UEBhMCS1IxDjAMBgNVBAgM\n\
BVNlb3VsMQ4wDAYDVQQHDAVTZW91bDEiMCAGA1UECgwZU2VvdWwgTmF0aW9uYWwg\n\
VW5pdmVyc2l0eTEiMCAGA1UECwwZU2VjdXJpdHkgT3B0aW1pemF0aW9uIExhYjEi\n\
MCAGCSqGSIb3DQEJARYTa3JsZWVAc29yLnNudS5hYy5rcjEWMBQGA1UEAwwNQXV0\n\
aCBBZ2VudCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK02jrW8\n\
304ftqEgC3aqlQDSb5v0+uIJihIY2CAGrono1PBHdNq0cFrdaxee52Ck3MrLcNkx\n\
zdjLvPIyAV/uAXdyIUbreGCBF/w+5nzJYEOZi5aXwwHV04ayvSfya1wrgmr1F9qI\n\
XwOFpq2hia4E8nl47ZIdurj+7pthQiEaHwDkRDzdBm/Cq6MjMLxFn69Gjf0gu6vO\n\
ClSgyWK60Bmz5Xm2rjO+WcmE2Pe0hwnx5xYHxbwpOi8a62H5h/cVP7s9T/a/DJsD\n\
/6hapc4qx0BurZVi8voJCuLeKdxMrbA2CJ0SRMq6nR/s6PHXqNMKNLjnoYm4OZ7k\n\
4CNvxY39j8XfXtUCAwEAAaMhMB8wDAYDVR0TBAUwAwEB/zAPBgNVHREECDAGhwST\n\
LvRsMA0GCSqGSIb3DQEBCwUAA4IBAQCqMoi5YamMZYTVHsXtqduyUT+D9PIXhwU5\n\
eGFVFp/7Ytg8ow33FHg9jL3VDoCTT2prJ3qXQgtaQVbDdcBN/4O4zFWYTsKlnEee\n\
Ru9eHlpcFZcIRfrg5alLjvZBdQNRMSb/Kfpogb3yEuqUq4Nk52rVeYU7mSjtrd1e\n\
R7Zgjxi91thn+NZdi8CcfSByUB241G3knMby6aSfvF1VGwjxDrMZMzIi+0jX13oV\n\
D2bCiZE7fUmzP9R0RUJvpOGMIP9ZDCSYv+7iRIKP1zHDiZOjyqSmhQhDKdtWZE9u\n\
jioLDtHvwuRvrxAkYnAKNkwvlqmE4UwNMAFWQR4iyT5yjKQqgVMf\n\
-----END CERTIFICATE-----\n\n"};
*/
