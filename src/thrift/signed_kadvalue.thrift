namespace cpp kad

struct SignedValue {
  1: string value,
  2: string value_signature ,
}

struct SignedRequest {
  1: string signer_id ,
  2: string public_key ,
  3: string signed_public_key ,
  4: string signed_request ,
}
