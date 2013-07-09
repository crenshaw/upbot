-- eDSL Test!
-- This is a language pragma stating that we would like to use the FFI
{-# LANGUAGE ForeignFunctionInterface #-}

import Foreign.C.String

foreign import ccall "clib.h printRandomString"
    c_printRandomString :: CString -> IO CString

main = do
    randomString <- newCString "Oogly Boogly Womp"
    c_printRandomString randomString
