/*
 * Copyright (c) 2012 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * NaCl Service Runtime.  Secure RNG implementation.
 */

//#include <string.h>

#include "native_client/src/include/build_config.h"
#if NACL_SGX == 1
#include "native_client/src/trusted/stdlib/api.h"
#else
#include <string.h>
#endif

#include <unistd.h>

#include "native_client/src/shared/platform/nacl_check.h"
#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/nacl_secure_random.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"


static struct NaClSecureRngIfVtbl const kNaClSecureRngVtbl;

void NaClSecureRngModuleInit(void) {

}

void NaClSecureRngModuleFini(void) {
}

int NaClSecureRngCtor(struct NaClSecureRng *self) {
  self->nvalid = 0;
  self->base.vtbl = &kNaClSecureRngVtbl;
  return 1;
}

int NaClSecureRngTestingCtor(struct NaClSecureRng *self,
                             uint8_t              *seed_material,
                             size_t               seed_bytes) {
  UNREFERENCED_PARAMETER(self);
  UNREFERENCED_PARAMETER(seed_material);
  UNREFERENCED_PARAMETER(seed_bytes);
  return 0;
}

static void NaClSecureRngDtor(struct NaClSecureRngIf *vself) {
  struct NaClSecureRng *self = (struct NaClSecureRng *) vself;
  memset(self->buf, 0, sizeof self->buf);
  vself->vtbl = NULL;
  return;
}

int RandomBitsRead (void * buffer, int size)
{
    int i = 0;

    for ( ; i < size ; i += 4) {
        uint32_t rand = rdrand();

        if (i + 4 <= size) {
            *(uint32_t *)(buffer + i) = rand;
        } else {
            switch (size - i) {
                case 3:
                    *(uint16_t *)(buffer + i) = rand & 0xffff;
                    i += 2;
                    rand >>= 16;
                case 1:
                    *(uint8_t *)(buffer + i) = rand & 0xff;
                    i++;
                    break;
                case 2:
                    *(uint16_t *)(buffer + i) = rand & 0xffff;
                    i += 2;
                    break;
            }
            break;
        }
    }
    return i;
}
static void NaClSecureRngFilbuf(struct NaClSecureRng *self) {
  self->nvalid = RandomBitsRead (self->buf, sizeof self->buf);
  if (self->nvalid <= 0) {
    NaClLog(LOG_FATAL, "NaClSecureRngFilbuf failed, read returned %d\n",
            self->nvalid);
  }
}

static uint8_t NaClSecureRngGenByte(struct NaClSecureRngIf *vself) {
  struct NaClSecureRng *self = (struct NaClSecureRng *) vself;

  if (0 > self->nvalid) {
    NaClLog(LOG_FATAL,
            "NaClSecureRngGenByte: illegal buffer state, nvalid = %d\n",
            self->nvalid);
  }
  if (0 == self->nvalid) {
    NaClSecureRngFilbuf(self);
  }
  /* 0 < self->nvalid <= sizeof self->buf */
  return self->buf[--self->nvalid];
}

static struct NaClSecureRngIfVtbl const kNaClSecureRngVtbl = {
  NaClSecureRngDtor,
  NaClSecureRngGenByte,
  NaClSecureRngDefaultGenUint32,
  NaClSecureRngDefaultGenBytes,
  NaClSecureRngDefaultUniform,
};
