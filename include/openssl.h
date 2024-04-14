#pragma once

#define OSSL_DEPRECATED(_m)

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>

PRE{
  if(OPENSSL_init_ssl(0, NULL) == 0){
    PR_abort();
  }
  if(OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS | OPENSSL_INIT_ADD_ALL_DIGESTS, NULL) == 0){
    PR_abort();
  }
}
