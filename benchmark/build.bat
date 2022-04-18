@echo off
REM SET VariantDefine=-DAPI_CB64
SET VariantDefine=-DAPI_OPENSSL_EVP

SET VariantIncludes=-Iopenssl/include
SET VariantLibs=openssl/lib/libcrypto.lib
cl -nologo -Zi -Zo -Oy- -O2 -W4 -I../ %VariantDefine% %VariantIncludes% encode_ascending.cpp %VariantLibs% /link /DEBUG
cl -nologo -Zi -Zo -Oy- -O2 -W4 -I../ %VariantDefine% %VariantIncludes% roundtrip_ascending.cpp %VariantLibs% /link /DEBUG
cl -nologo -Zi -Zo -Oy- -O2 -W4 -I../ %VariantDefine% %VariantIncludes% roundtrip_verylarge.cpp %VariantLibs% /link /DEBUG
