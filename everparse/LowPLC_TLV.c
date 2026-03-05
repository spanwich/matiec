

#include "LowPLC_TLV.h"

static inline uint64_t ValidateLoadInt(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateBoolNot(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 2ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 2ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateBoolAnd(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateBoolOr(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateBoolXor(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCopyBool(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 2ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 2ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateIntAdd(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateIntSub(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateIntMul(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCopyInt(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 2ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 2ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCmpEq(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCmpNe(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCmpLt(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCmpGt(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCmpLe(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCmpGe(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateTonCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 7ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 7ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateTofCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 7ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 7ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateTpCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 7ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 7ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCtuCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 6ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 6ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCtdCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 6ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 6ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateCtudCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 9ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 9ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateSrCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 4ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 4ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateRsCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 4ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 4ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateRtrigCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t ValidateFtrigCall(uint64_t InputLength, uint64_t StartPosition)
{
  BOOLEAN hasBytes = 3ULL <= (InputLength - StartPosition);
  if (hasBytes)
  {
    return StartPosition + 3ULL;
  }
  return EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA, StartPosition);
}

static inline uint64_t
ValidateLeafPayload(
  uint8_t Tag,
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLen,
  uint64_t StartPosition
)
{
  if (Tag == 1U)
  {
    /* Validating field load_true */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterLeafPayload;
    if (hasBytes)
    {
      positionAfterLeafPayload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterLeafPayload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "load_true",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 2U)
  {
    /* Validating field load_false */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterLeafPayload;
    if (hasBytes)
    {
      positionAfterLeafPayload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterLeafPayload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "load_false",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 3U)
  {
    /* Validating field load_int */
    uint64_t positionAfterLeafPayload = ValidateLoadInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "load_int",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 16U)
  {
    /* Validating field bool_not */
    uint64_t positionAfterLeafPayload = ValidateBoolNot(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "bool_not",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 17U)
  {
    /* Validating field bool_and */
    uint64_t positionAfterLeafPayload = ValidateBoolAnd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "bool_and",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 18U)
  {
    /* Validating field bool_or */
    uint64_t positionAfterLeafPayload = ValidateBoolOr(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "bool_or",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 19U)
  {
    /* Validating field bool_xor */
    uint64_t positionAfterLeafPayload = ValidateBoolXor(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "bool_xor",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 20U)
  {
    /* Validating field copy_bool */
    uint64_t positionAfterLeafPayload = ValidateCopyBool(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "copy_bool",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 32U)
  {
    /* Validating field int_add */
    uint64_t positionAfterLeafPayload = ValidateIntAdd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "int_add",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 33U)
  {
    /* Validating field int_sub */
    uint64_t positionAfterLeafPayload = ValidateIntSub(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "int_sub",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 34U)
  {
    /* Validating field int_mul */
    uint64_t positionAfterLeafPayload = ValidateIntMul(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "int_mul",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 35U)
  {
    /* Validating field copy_int */
    uint64_t positionAfterLeafPayload = ValidateCopyInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "copy_int",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 48U)
  {
    /* Validating field cmp_eq */
    uint64_t positionAfterLeafPayload = ValidateCmpEq(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "cmp_eq",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 49U)
  {
    /* Validating field cmp_ne */
    uint64_t positionAfterLeafPayload = ValidateCmpNe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "cmp_ne",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 50U)
  {
    /* Validating field cmp_lt */
    uint64_t positionAfterLeafPayload = ValidateCmpLt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "cmp_lt",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 51U)
  {
    /* Validating field cmp_gt */
    uint64_t positionAfterLeafPayload = ValidateCmpGt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "cmp_gt",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 52U)
  {
    /* Validating field cmp_le */
    uint64_t positionAfterLeafPayload = ValidateCmpLe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "cmp_le",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 53U)
  {
    /* Validating field cmp_ge */
    uint64_t positionAfterLeafPayload = ValidateCmpGe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "cmp_ge",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 80U)
  {
    /* Validating field ton_call */
    uint64_t positionAfterLeafPayload = ValidateTonCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "ton_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 81U)
  {
    /* Validating field tof_call */
    uint64_t positionAfterLeafPayload = ValidateTofCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "tof_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 82U)
  {
    /* Validating field tp_call */
    uint64_t positionAfterLeafPayload = ValidateTpCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "tp_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 96U)
  {
    /* Validating field ctu_call */
    uint64_t positionAfterLeafPayload = ValidateCtuCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "ctu_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 97U)
  {
    /* Validating field ctd_call */
    uint64_t positionAfterLeafPayload = ValidateCtdCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "ctd_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 98U)
  {
    /* Validating field ctud_call */
    uint64_t positionAfterLeafPayload = ValidateCtudCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "ctud_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 112U)
  {
    /* Validating field sr_call */
    uint64_t positionAfterLeafPayload = ValidateSrCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "sr_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 113U)
  {
    /* Validating field rs_call */
    uint64_t positionAfterLeafPayload = ValidateRsCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "rs_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 114U)
  {
    /* Validating field rtrig_call */
    uint64_t positionAfterLeafPayload = ValidateRtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "rtrig_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  if (Tag == 115U)
  {
    /* Validating field ftrig_call */
    uint64_t positionAfterLeafPayload = ValidateFtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterLeafPayload))
    {
      return positionAfterLeafPayload;
    }
    ErrorHandlerFn("_LeafPayload",
      "ftrig_call",
      EverParseErrorReasonOfResult(positionAfterLeafPayload),
      EverParseGetValidatorErrorKind(positionAfterLeafPayload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterLeafPayload;
  }
  uint64_t
  positionAfterLeafPayload =
    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_IMPOSSIBLE,
      StartPosition);
  if (EverParseIsSuccess(positionAfterLeafPayload))
  {
    return positionAfterLeafPayload;
  }
  ErrorHandlerFn("_LeafPayload",
    "_x_0",
    EverParseErrorReasonOfResult(positionAfterLeafPayload),
    EverParseGetValidatorErrorKind(positionAfterLeafPayload),
    Ctxt,
    Input,
    StartPosition);
  return positionAfterLeafPayload;
}

static inline uint64_t
ValidateStmtD6(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAftertag;
  if (hasBytes0)
  {
    positionAftertag = StartPosition + 1ULL;
  }
  else
  {
    positionAftertag =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t positionAfterStmtD6;
  if (EverParseIsError(positionAftertag))
  {
    positionAfterStmtD6 = positionAftertag;
  }
  else
  {
    uint8_t tag = Input[(uint32_t)StartPosition];
    BOOLEAN tagConstraintIsOk = 1U <= tag && tag <= 115U;
    uint64_t positionAftertag1 = EverParseCheckConstraintOk(tagConstraintIsOk, positionAftertag);
    if (EverParseIsError(positionAftertag1))
    {
      positionAfterStmtD6 = positionAftertag1;
    }
    else
    {
      /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
      BOOLEAN hasBytes = 2ULL <= (InputLength - positionAftertag1);
      uint64_t positionAfterStmtD60;
      if (hasBytes)
      {
        positionAfterStmtD60 = positionAftertag1 + 2ULL;
      }
      else
      {
        positionAfterStmtD60 =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAftertag1);
      }
      uint64_t positionAfterlen;
      if (EverParseIsSuccess(positionAfterStmtD60))
      {
        positionAfterlen = positionAfterStmtD60;
      }
      else
      {
        ErrorHandlerFn("_StmtD6",
          "len",
          EverParseErrorReasonOfResult(positionAfterStmtD60),
          EverParseGetValidatorErrorKind(positionAfterStmtD60),
          Ctxt,
          Input,
          positionAftertag1);
        positionAfterlen = positionAfterStmtD60;
      }
      if (EverParseIsError(positionAfterlen))
      {
        positionAfterStmtD6 = positionAfterlen;
      }
      else
      {
        uint16_t r = Load16Le(Input + (uint32_t)positionAftertag1);
        uint16_t len = (uint16_t)(uint32_t)r;
        /* Validating field payload */
        BOOLEAN hasEnoughBytes = (uint64_t)(uint32_t)len <= (InputLength - positionAfterlen);
        uint64_t positionAfterStmtD61;
        if (!hasEnoughBytes)
        {
          positionAfterStmtD61 =
            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
              positionAfterlen);
        }
        else
        {
          uint8_t *truncatedInput = Input;
          uint64_t truncatedInputLength = positionAfterlen + (uint64_t)(uint32_t)len;
          uint64_t result = positionAfterlen;
          while (TRUE)
          {
            uint64_t position = result;
            BOOLEAN ite;
            if (!(1ULL <= (truncatedInputLength - position)))
            {
              ite = TRUE;
            }
            else
            {
              uint64_t
              positionAfterStmtD62 =
                ValidateLeafPayload(tag,
                  Ctxt,
                  ErrorHandlerFn,
                  truncatedInput,
                  truncatedInputLength,
                  position);
              uint64_t result1;
              if (EverParseIsSuccess(positionAfterStmtD62))
              {
                result1 = positionAfterStmtD62;
              }
              else
              {
                ErrorHandlerFn("_StmtD6",
                  "payload.element",
                  EverParseErrorReasonOfResult(positionAfterStmtD62),
                  EverParseGetValidatorErrorKind(positionAfterStmtD62),
                  Ctxt,
                  truncatedInput,
                  position);
                result1 = positionAfterStmtD62;
              }
              result = result1;
              ite = EverParseIsError(result1);
            }
            if (ite)
            {
              break;
            }
          }
          uint64_t res = result;
          positionAfterStmtD61 = res;
        }
        if (EverParseIsSuccess(positionAfterStmtD61))
        {
          positionAfterStmtD6 = positionAfterStmtD61;
        }
        else
        {
          ErrorHandlerFn("_StmtD6",
            "payload",
            EverParseErrorReasonOfResult(positionAfterStmtD61),
            EverParseGetValidatorErrorKind(positionAfterStmtD61),
            Ctxt,
            Input,
            positionAfterlen);
          positionAfterStmtD6 = positionAfterStmtD61;
        }
      }
    }
  }
  if (EverParseIsSuccess(positionAfterStmtD6))
  {
    return positionAfterStmtD6;
  }
  ErrorHandlerFn("_StmtD6",
    "tag",
    EverParseErrorReasonOfResult(positionAfterStmtD6),
    EverParseGetValidatorErrorKind(positionAfterStmtD6),
    Ctxt,
    Input,
    StartPosition);
  return positionAfterStmtD6;
}

static inline uint64_t
ValidateIfBlockD5(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field cond */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterIfBlockD5;
  if (hasBytes0)
  {
    positionAfterIfBlockD5 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterIfBlockD5 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterIfBlockD5))
  {
    res = positionAfterIfBlockD5;
  }
  else
  {
    ErrorHandlerFn("_IfBlockD5",
      "cond",
      EverParseErrorReasonOfResult(positionAfterIfBlockD5),
      EverParseGetValidatorErrorKind(positionAfterIfBlockD5),
      Ctxt,
      Input,
      StartPosition);
    res = positionAfterIfBlockD5;
  }
  uint64_t positionAftercond = res;
  if (EverParseIsError(positionAftercond))
  {
    return positionAftercond;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes1 = 1ULL <= (InputLength - positionAftercond);
  uint64_t positionAfterthenCount;
  if (hasBytes1)
  {
    positionAfterthenCount = positionAftercond + 1ULL;
  }
  else
  {
    positionAfterthenCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftercond);
  }
  uint64_t positionAfterIfBlockD50;
  if (EverParseIsError(positionAfterthenCount))
  {
    positionAfterIfBlockD50 = positionAfterthenCount;
  }
  else
  {
    uint8_t thenCount = Input[(uint32_t)positionAftercond];
    BOOLEAN thenCountConstraintIsOk = thenCount <= 64U;
    uint64_t
    positionAfterthenCount1 =
      EverParseCheckConstraintOk(thenCountConstraintIsOk,
        positionAfterthenCount);
    if (EverParseIsError(positionAfterthenCount1))
    {
      positionAfterIfBlockD50 = positionAfterthenCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT8, i.e., 1 byte */
      BOOLEAN hasBytes2 = 1ULL <= (InputLength - positionAfterthenCount1);
      uint64_t positionAfterelseCount;
      if (hasBytes2)
      {
        positionAfterelseCount = positionAfterthenCount1 + 1ULL;
      }
      else
      {
        positionAfterelseCount =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterthenCount1);
      }
      uint64_t positionAfterIfBlockD51;
      if (EverParseIsError(positionAfterelseCount))
      {
        positionAfterIfBlockD51 = positionAfterelseCount;
      }
      else
      {
        uint8_t elseCount = Input[(uint32_t)positionAfterthenCount1];
        BOOLEAN elseCountConstraintIsOk = elseCount <= 64U;
        uint64_t
        positionAfterelseCount1 =
          EverParseCheckConstraintOk(elseCountConstraintIsOk,
            positionAfterelseCount);
        if (EverParseIsError(positionAfterelseCount1))
        {
          positionAfterIfBlockD51 = positionAfterelseCount1;
        }
        else
        {
          /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
          BOOLEAN hasBytes3 = 2ULL <= (InputLength - positionAfterelseCount1);
          uint64_t positionAfterIfBlockD52;
          if (hasBytes3)
          {
            positionAfterIfBlockD52 = positionAfterelseCount1 + 2ULL;
          }
          else
          {
            positionAfterIfBlockD52 =
              EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                positionAfterelseCount1);
          }
          uint64_t positionAfterthenLen;
          if (EverParseIsSuccess(positionAfterIfBlockD52))
          {
            positionAfterthenLen = positionAfterIfBlockD52;
          }
          else
          {
            ErrorHandlerFn("_IfBlockD5",
              "then_len",
              EverParseErrorReasonOfResult(positionAfterIfBlockD52),
              EverParseGetValidatorErrorKind(positionAfterIfBlockD52),
              Ctxt,
              Input,
              positionAfterelseCount1);
            positionAfterthenLen = positionAfterIfBlockD52;
          }
          if (EverParseIsError(positionAfterthenLen))
          {
            positionAfterIfBlockD51 = positionAfterthenLen;
          }
          else
          {
            uint16_t r0 = Load16Le(Input + (uint32_t)positionAfterelseCount1);
            uint16_t thenLen = (uint16_t)(uint32_t)r0;
            /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
            BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterthenLen);
            uint64_t positionAfterIfBlockD53;
            if (hasBytes)
            {
              positionAfterIfBlockD53 = positionAfterthenLen + 2ULL;
            }
            else
            {
              positionAfterIfBlockD53 =
                EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                  positionAfterthenLen);
            }
            uint64_t positionAfterelseLen;
            if (EverParseIsSuccess(positionAfterIfBlockD53))
            {
              positionAfterelseLen = positionAfterIfBlockD53;
            }
            else
            {
              ErrorHandlerFn("_IfBlockD5",
                "else_len",
                EverParseErrorReasonOfResult(positionAfterIfBlockD53),
                EverParseGetValidatorErrorKind(positionAfterIfBlockD53),
                Ctxt,
                Input,
                positionAfterthenLen);
              positionAfterelseLen = positionAfterIfBlockD53;
            }
            if (EverParseIsError(positionAfterelseLen))
            {
              positionAfterIfBlockD51 = positionAfterelseLen;
            }
            else
            {
              uint16_t r = Load16Le(Input + (uint32_t)positionAfterthenLen);
              uint16_t elseLen = (uint16_t)(uint32_t)r;
              /* Validating field then_body */
              BOOLEAN
              hasEnoughBytes0 = (uint64_t)(uint32_t)thenLen <= (InputLength - positionAfterelseLen);
              uint64_t positionAfterIfBlockD54;
              if (!hasEnoughBytes0)
              {
                positionAfterIfBlockD54 =
                  EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                    positionAfterelseLen);
              }
              else
              {
                uint8_t *truncatedInput = Input;
                uint64_t truncatedInputLength = positionAfterelseLen + (uint64_t)(uint32_t)thenLen;
                uint64_t result = positionAfterelseLen;
                while (TRUE)
                {
                  uint64_t position = result;
                  BOOLEAN ite;
                  if (!(1ULL <= (truncatedInputLength - position)))
                  {
                    ite = TRUE;
                  }
                  else
                  {
                    uint64_t
                    positionAfterIfBlockD55 =
                      ValidateStmtD6(Ctxt,
                        ErrorHandlerFn,
                        truncatedInput,
                        truncatedInputLength,
                        position);
                    uint64_t result1;
                    if (EverParseIsSuccess(positionAfterIfBlockD55))
                    {
                      result1 = positionAfterIfBlockD55;
                    }
                    else
                    {
                      ErrorHandlerFn("_IfBlockD5",
                        "then_body.element",
                        EverParseErrorReasonOfResult(positionAfterIfBlockD55),
                        EverParseGetValidatorErrorKind(positionAfterIfBlockD55),
                        Ctxt,
                        truncatedInput,
                        position);
                      result1 = positionAfterIfBlockD55;
                    }
                    result = result1;
                    ite = EverParseIsError(result1);
                  }
                  if (ite)
                  {
                    break;
                  }
                }
                uint64_t res0 = result;
                positionAfterIfBlockD54 = res0;
              }
              uint64_t positionAfterthenBody;
              if (EverParseIsSuccess(positionAfterIfBlockD54))
              {
                positionAfterthenBody = positionAfterIfBlockD54;
              }
              else
              {
                ErrorHandlerFn("_IfBlockD5",
                  "then_body",
                  EverParseErrorReasonOfResult(positionAfterIfBlockD54),
                  EverParseGetValidatorErrorKind(positionAfterIfBlockD54),
                  Ctxt,
                  Input,
                  positionAfterelseLen);
                positionAfterthenBody = positionAfterIfBlockD54;
              }
              if (EverParseIsError(positionAfterthenBody))
              {
                positionAfterIfBlockD51 = positionAfterthenBody;
              }
              else
              {
                /* Validating field else_body */
                BOOLEAN
                hasEnoughBytes =
                  (uint64_t)(uint32_t)elseLen <= (InputLength - positionAfterthenBody);
                uint64_t positionAfterIfBlockD55;
                if (!hasEnoughBytes)
                {
                  positionAfterIfBlockD55 =
                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                      positionAfterthenBody);
                }
                else
                {
                  uint8_t *truncatedInput = Input;
                  uint64_t
                  truncatedInputLength = positionAfterthenBody + (uint64_t)(uint32_t)elseLen;
                  uint64_t result = positionAfterthenBody;
                  while (TRUE)
                  {
                    uint64_t position = result;
                    BOOLEAN ite;
                    if (!(1ULL <= (truncatedInputLength - position)))
                    {
                      ite = TRUE;
                    }
                    else
                    {
                      uint64_t
                      positionAfterIfBlockD56 =
                        ValidateStmtD6(Ctxt,
                          ErrorHandlerFn,
                          truncatedInput,
                          truncatedInputLength,
                          position);
                      uint64_t result1;
                      if (EverParseIsSuccess(positionAfterIfBlockD56))
                      {
                        result1 = positionAfterIfBlockD56;
                      }
                      else
                      {
                        ErrorHandlerFn("_IfBlockD5",
                          "else_body.element",
                          EverParseErrorReasonOfResult(positionAfterIfBlockD56),
                          EverParseGetValidatorErrorKind(positionAfterIfBlockD56),
                          Ctxt,
                          truncatedInput,
                          position);
                        result1 = positionAfterIfBlockD56;
                      }
                      result = result1;
                      ite = EverParseIsError(result1);
                    }
                    if (ite)
                    {
                      break;
                    }
                  }
                  uint64_t res0 = result;
                  positionAfterIfBlockD55 = res0;
                }
                if (EverParseIsSuccess(positionAfterIfBlockD55))
                {
                  positionAfterIfBlockD51 = positionAfterIfBlockD55;
                }
                else
                {
                  ErrorHandlerFn("_IfBlockD5",
                    "else_body",
                    EverParseErrorReasonOfResult(positionAfterIfBlockD55),
                    EverParseGetValidatorErrorKind(positionAfterIfBlockD55),
                    Ctxt,
                    Input,
                    positionAfterthenBody);
                  positionAfterIfBlockD51 = positionAfterIfBlockD55;
                }
              }
            }
          }
        }
      }
      if (EverParseIsSuccess(positionAfterIfBlockD51))
      {
        positionAfterIfBlockD50 = positionAfterIfBlockD51;
      }
      else
      {
        ErrorHandlerFn("_IfBlockD5",
          "else_count",
          EverParseErrorReasonOfResult(positionAfterIfBlockD51),
          EverParseGetValidatorErrorKind(positionAfterIfBlockD51),
          Ctxt,
          Input,
          positionAfterthenCount1);
        positionAfterIfBlockD50 = positionAfterIfBlockD51;
      }
    }
  }
  if (EverParseIsSuccess(positionAfterIfBlockD50))
  {
    return positionAfterIfBlockD50;
  }
  ErrorHandlerFn("_IfBlockD5",
    "then_count",
    EverParseErrorReasonOfResult(positionAfterIfBlockD50),
    EverParseGetValidatorErrorKind(positionAfterIfBlockD50),
    Ctxt,
    Input,
    positionAftercond);
  return positionAfterIfBlockD50;
}

static inline uint64_t
ValidateForBlockD5(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field var_idx */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterForBlockD5;
  if (hasBytes0)
  {
    positionAfterForBlockD5 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterForBlockD5 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res0;
  if (EverParseIsSuccess(positionAfterForBlockD5))
  {
    res0 = positionAfterForBlockD5;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD5",
      "var_idx",
      EverParseErrorReasonOfResult(positionAfterForBlockD5),
      EverParseGetValidatorErrorKind(positionAfterForBlockD5),
      Ctxt,
      Input,
      StartPosition);
    res0 = positionAfterForBlockD5;
  }
  uint64_t positionAftervarIdx = res0;
  if (EverParseIsError(positionAftervarIdx))
  {
    return positionAftervarIdx;
  }
  /* Validating field from_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes1 = 2ULL <= (InputLength - positionAftervarIdx);
  uint64_t positionAfterForBlockD50;
  if (hasBytes1)
  {
    positionAfterForBlockD50 = positionAftervarIdx + 2ULL;
  }
  else
  {
    positionAfterForBlockD50 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftervarIdx);
  }
  uint64_t res1;
  if (EverParseIsSuccess(positionAfterForBlockD50))
  {
    res1 = positionAfterForBlockD50;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD5",
      "from_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD50),
      EverParseGetValidatorErrorKind(positionAfterForBlockD50),
      Ctxt,
      Input,
      positionAftervarIdx);
    res1 = positionAfterForBlockD50;
  }
  uint64_t positionAfterfromVal = res1;
  if (EverParseIsError(positionAfterfromVal))
  {
    return positionAfterfromVal;
  }
  /* Validating field to_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes2 = 2ULL <= (InputLength - positionAfterfromVal);
  uint64_t positionAfterForBlockD51;
  if (hasBytes2)
  {
    positionAfterForBlockD51 = positionAfterfromVal + 2ULL;
  }
  else
  {
    positionAfterForBlockD51 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterfromVal);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterForBlockD51))
  {
    res = positionAfterForBlockD51;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD5",
      "to_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD51),
      EverParseGetValidatorErrorKind(positionAfterForBlockD51),
      Ctxt,
      Input,
      positionAfterfromVal);
    res = positionAfterForBlockD51;
  }
  uint64_t positionAftertoVal = res;
  if (EverParseIsError(positionAftertoVal))
  {
    return positionAftertoVal;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes3 = 1ULL <= (InputLength - positionAftertoVal);
  uint64_t positionAfterbodyCount;
  if (hasBytes3)
  {
    positionAfterbodyCount = positionAftertoVal + 1ULL;
  }
  else
  {
    positionAfterbodyCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertoVal);
  }
  uint64_t positionAfterForBlockD52;
  if (EverParseIsError(positionAfterbodyCount))
  {
    positionAfterForBlockD52 = positionAfterbodyCount;
  }
  else
  {
    uint8_t bodyCount = Input[(uint32_t)positionAftertoVal];
    BOOLEAN bodyCountConstraintIsOk = bodyCount <= 64U;
    uint64_t
    positionAfterbodyCount1 =
      EverParseCheckConstraintOk(bodyCountConstraintIsOk,
        positionAfterbodyCount);
    if (EverParseIsError(positionAfterbodyCount1))
    {
      positionAfterForBlockD52 = positionAfterbodyCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
      BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterbodyCount1);
      uint64_t positionAfterForBlockD53;
      if (hasBytes)
      {
        positionAfterForBlockD53 = positionAfterbodyCount1 + 2ULL;
      }
      else
      {
        positionAfterForBlockD53 =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterbodyCount1);
      }
      uint64_t positionAfterbodyLen;
      if (EverParseIsSuccess(positionAfterForBlockD53))
      {
        positionAfterbodyLen = positionAfterForBlockD53;
      }
      else
      {
        ErrorHandlerFn("_ForBlockD5",
          "body_len",
          EverParseErrorReasonOfResult(positionAfterForBlockD53),
          EverParseGetValidatorErrorKind(positionAfterForBlockD53),
          Ctxt,
          Input,
          positionAfterbodyCount1);
        positionAfterbodyLen = positionAfterForBlockD53;
      }
      if (EverParseIsError(positionAfterbodyLen))
      {
        positionAfterForBlockD52 = positionAfterbodyLen;
      }
      else
      {
        uint16_t r = Load16Le(Input + (uint32_t)positionAfterbodyCount1);
        uint16_t bodyLen = (uint16_t)(uint32_t)r;
        /* Validating field body */
        BOOLEAN
        hasEnoughBytes = (uint64_t)(uint32_t)bodyLen <= (InputLength - positionAfterbodyLen);
        uint64_t positionAfterForBlockD54;
        if (!hasEnoughBytes)
        {
          positionAfterForBlockD54 =
            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
              positionAfterbodyLen);
        }
        else
        {
          uint8_t *truncatedInput = Input;
          uint64_t truncatedInputLength = positionAfterbodyLen + (uint64_t)(uint32_t)bodyLen;
          uint64_t result = positionAfterbodyLen;
          while (TRUE)
          {
            uint64_t position = result;
            BOOLEAN ite;
            if (!(1ULL <= (truncatedInputLength - position)))
            {
              ite = TRUE;
            }
            else
            {
              uint64_t
              positionAfterForBlockD55 =
                ValidateStmtD6(Ctxt,
                  ErrorHandlerFn,
                  truncatedInput,
                  truncatedInputLength,
                  position);
              uint64_t result1;
              if (EverParseIsSuccess(positionAfterForBlockD55))
              {
                result1 = positionAfterForBlockD55;
              }
              else
              {
                ErrorHandlerFn("_ForBlockD5",
                  "body.element",
                  EverParseErrorReasonOfResult(positionAfterForBlockD55),
                  EverParseGetValidatorErrorKind(positionAfterForBlockD55),
                  Ctxt,
                  truncatedInput,
                  position);
                result1 = positionAfterForBlockD55;
              }
              result = result1;
              ite = EverParseIsError(result1);
            }
            if (ite)
            {
              break;
            }
          }
          uint64_t res2 = result;
          positionAfterForBlockD54 = res2;
        }
        if (EverParseIsSuccess(positionAfterForBlockD54))
        {
          positionAfterForBlockD52 = positionAfterForBlockD54;
        }
        else
        {
          ErrorHandlerFn("_ForBlockD5",
            "body",
            EverParseErrorReasonOfResult(positionAfterForBlockD54),
            EverParseGetValidatorErrorKind(positionAfterForBlockD54),
            Ctxt,
            Input,
            positionAfterbodyLen);
          positionAfterForBlockD52 = positionAfterForBlockD54;
        }
      }
    }
  }
  if (EverParseIsSuccess(positionAfterForBlockD52))
  {
    return positionAfterForBlockD52;
  }
  ErrorHandlerFn("_ForBlockD5",
    "body_count",
    EverParseErrorReasonOfResult(positionAfterForBlockD52),
    EverParseGetValidatorErrorKind(positionAfterForBlockD52),
    Ctxt,
    Input,
    positionAftertoVal);
  return positionAfterForBlockD52;
}

static inline uint64_t
ValidateStmtD5payload(
  uint8_t Tag,
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLen,
  uint64_t StartPosition
)
{
  if (Tag == 1U)
  {
    /* Validating field load_true */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD5payload;
    if (hasBytes)
    {
      positionAfterStmtD5payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD5payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "load_true",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 2U)
  {
    /* Validating field load_false */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD5payload;
    if (hasBytes)
    {
      positionAfterStmtD5payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD5payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "load_false",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 3U)
  {
    /* Validating field load_int */
    uint64_t positionAfterStmtD5payload = ValidateLoadInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "load_int",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 16U)
  {
    /* Validating field bool_not */
    uint64_t positionAfterStmtD5payload = ValidateBoolNot(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "bool_not",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 17U)
  {
    /* Validating field bool_and */
    uint64_t positionAfterStmtD5payload = ValidateBoolAnd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "bool_and",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 18U)
  {
    /* Validating field bool_or */
    uint64_t positionAfterStmtD5payload = ValidateBoolOr(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "bool_or",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 19U)
  {
    /* Validating field bool_xor */
    uint64_t positionAfterStmtD5payload = ValidateBoolXor(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "bool_xor",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 20U)
  {
    /* Validating field copy_bool */
    uint64_t positionAfterStmtD5payload = ValidateCopyBool(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "copy_bool",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 32U)
  {
    /* Validating field int_add */
    uint64_t positionAfterStmtD5payload = ValidateIntAdd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "int_add",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 33U)
  {
    /* Validating field int_sub */
    uint64_t positionAfterStmtD5payload = ValidateIntSub(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "int_sub",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 34U)
  {
    /* Validating field int_mul */
    uint64_t positionAfterStmtD5payload = ValidateIntMul(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "int_mul",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 35U)
  {
    /* Validating field copy_int */
    uint64_t positionAfterStmtD5payload = ValidateCopyInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "copy_int",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 48U)
  {
    /* Validating field cmp_eq */
    uint64_t positionAfterStmtD5payload = ValidateCmpEq(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "cmp_eq",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 49U)
  {
    /* Validating field cmp_ne */
    uint64_t positionAfterStmtD5payload = ValidateCmpNe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "cmp_ne",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 50U)
  {
    /* Validating field cmp_lt */
    uint64_t positionAfterStmtD5payload = ValidateCmpLt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "cmp_lt",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 51U)
  {
    /* Validating field cmp_gt */
    uint64_t positionAfterStmtD5payload = ValidateCmpGt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "cmp_gt",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 52U)
  {
    /* Validating field cmp_le */
    uint64_t positionAfterStmtD5payload = ValidateCmpLe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "cmp_le",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 53U)
  {
    /* Validating field cmp_ge */
    uint64_t positionAfterStmtD5payload = ValidateCmpGe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "cmp_ge",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 64U)
  {
    /* Validating field if_block */
    uint64_t
    positionAfterStmtD5payload =
      ValidateIfBlockD5(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "if_block",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 65U)
  {
    /* Validating field for_block */
    uint64_t
    positionAfterStmtD5payload =
      ValidateForBlockD5(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "for_block",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 80U)
  {
    /* Validating field ton_call */
    uint64_t positionAfterStmtD5payload = ValidateTonCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "ton_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 81U)
  {
    /* Validating field tof_call */
    uint64_t positionAfterStmtD5payload = ValidateTofCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "tof_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 82U)
  {
    /* Validating field tp_call */
    uint64_t positionAfterStmtD5payload = ValidateTpCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "tp_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 96U)
  {
    /* Validating field ctu_call */
    uint64_t positionAfterStmtD5payload = ValidateCtuCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "ctu_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 97U)
  {
    /* Validating field ctd_call */
    uint64_t positionAfterStmtD5payload = ValidateCtdCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "ctd_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 98U)
  {
    /* Validating field ctud_call */
    uint64_t positionAfterStmtD5payload = ValidateCtudCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "ctud_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 112U)
  {
    /* Validating field sr_call */
    uint64_t positionAfterStmtD5payload = ValidateSrCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "sr_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 113U)
  {
    /* Validating field rs_call */
    uint64_t positionAfterStmtD5payload = ValidateRsCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "rs_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 114U)
  {
    /* Validating field rtrig_call */
    uint64_t positionAfterStmtD5payload = ValidateRtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "rtrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  if (Tag == 115U)
  {
    /* Validating field ftrig_call */
    uint64_t positionAfterStmtD5payload = ValidateFtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD5payload))
    {
      return positionAfterStmtD5payload;
    }
    ErrorHandlerFn("_StmtD5Payload",
      "ftrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD5payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD5payload;
  }
  uint64_t
  positionAfterStmtD5payload =
    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_IMPOSSIBLE,
      StartPosition);
  if (EverParseIsSuccess(positionAfterStmtD5payload))
  {
    return positionAfterStmtD5payload;
  }
  ErrorHandlerFn("_StmtD5Payload",
    "_x_29",
    EverParseErrorReasonOfResult(positionAfterStmtD5payload),
    EverParseGetValidatorErrorKind(positionAfterStmtD5payload),
    Ctxt,
    Input,
    StartPosition);
  return positionAfterStmtD5payload;
}

static inline uint64_t
ValidateStmtD5(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterStmtD5;
  if (hasBytes0)
  {
    positionAfterStmtD5 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterStmtD5 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t positionAftertag;
  if (EverParseIsSuccess(positionAfterStmtD5))
  {
    positionAftertag = positionAfterStmtD5;
  }
  else
  {
    ErrorHandlerFn("_StmtD5",
      "tag",
      EverParseErrorReasonOfResult(positionAfterStmtD5),
      EverParseGetValidatorErrorKind(positionAfterStmtD5),
      Ctxt,
      Input,
      StartPosition);
    positionAftertag = positionAfterStmtD5;
  }
  if (EverParseIsError(positionAftertag))
  {
    return positionAftertag;
  }
  uint8_t tag = Input[(uint32_t)StartPosition];
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes = 2ULL <= (InputLength - positionAftertag);
  uint64_t positionAfterStmtD50;
  if (hasBytes)
  {
    positionAfterStmtD50 = positionAftertag + 2ULL;
  }
  else
  {
    positionAfterStmtD50 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertag);
  }
  uint64_t positionAfterlen;
  if (EverParseIsSuccess(positionAfterStmtD50))
  {
    positionAfterlen = positionAfterStmtD50;
  }
  else
  {
    ErrorHandlerFn("_StmtD5",
      "len",
      EverParseErrorReasonOfResult(positionAfterStmtD50),
      EverParseGetValidatorErrorKind(positionAfterStmtD50),
      Ctxt,
      Input,
      positionAftertag);
    positionAfterlen = positionAfterStmtD50;
  }
  if (EverParseIsError(positionAfterlen))
  {
    return positionAfterlen;
  }
  uint16_t r = Load16Le(Input + (uint32_t)positionAftertag);
  uint16_t len = (uint16_t)(uint32_t)r;
  /* Validating field payload */
  BOOLEAN hasEnoughBytes = (uint64_t)(uint32_t)len <= (InputLength - positionAfterlen);
  uint64_t positionAfterStmtD51;
  if (!hasEnoughBytes)
  {
    positionAfterStmtD51 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterlen);
  }
  else
  {
    uint8_t *truncatedInput = Input;
    uint64_t truncatedInputLength = positionAfterlen + (uint64_t)(uint32_t)len;
    uint64_t result = positionAfterlen;
    while (TRUE)
    {
      uint64_t position = result;
      BOOLEAN ite;
      if (!(1ULL <= (truncatedInputLength - position)))
      {
        ite = TRUE;
      }
      else
      {
        uint64_t
        positionAfterStmtD52 =
          ValidateStmtD5payload(tag,
            Ctxt,
            ErrorHandlerFn,
            truncatedInput,
            truncatedInputLength,
            position);
        uint64_t result1;
        if (EverParseIsSuccess(positionAfterStmtD52))
        {
          result1 = positionAfterStmtD52;
        }
        else
        {
          ErrorHandlerFn("_StmtD5",
            "payload.element",
            EverParseErrorReasonOfResult(positionAfterStmtD52),
            EverParseGetValidatorErrorKind(positionAfterStmtD52),
            Ctxt,
            truncatedInput,
            position);
          result1 = positionAfterStmtD52;
        }
        result = result1;
        ite = EverParseIsError(result1);
      }
      if (ite)
      {
        break;
      }
    }
    uint64_t res = result;
    positionAfterStmtD51 = res;
  }
  if (EverParseIsSuccess(positionAfterStmtD51))
  {
    return positionAfterStmtD51;
  }
  ErrorHandlerFn("_StmtD5",
    "payload",
    EverParseErrorReasonOfResult(positionAfterStmtD51),
    EverParseGetValidatorErrorKind(positionAfterStmtD51),
    Ctxt,
    Input,
    positionAfterlen);
  return positionAfterStmtD51;
}

static inline uint64_t
ValidateIfBlockD4(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field cond */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterIfBlockD4;
  if (hasBytes0)
  {
    positionAfterIfBlockD4 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterIfBlockD4 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterIfBlockD4))
  {
    res = positionAfterIfBlockD4;
  }
  else
  {
    ErrorHandlerFn("_IfBlockD4",
      "cond",
      EverParseErrorReasonOfResult(positionAfterIfBlockD4),
      EverParseGetValidatorErrorKind(positionAfterIfBlockD4),
      Ctxt,
      Input,
      StartPosition);
    res = positionAfterIfBlockD4;
  }
  uint64_t positionAftercond = res;
  if (EverParseIsError(positionAftercond))
  {
    return positionAftercond;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes1 = 1ULL <= (InputLength - positionAftercond);
  uint64_t positionAfterthenCount;
  if (hasBytes1)
  {
    positionAfterthenCount = positionAftercond + 1ULL;
  }
  else
  {
    positionAfterthenCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftercond);
  }
  uint64_t positionAfterIfBlockD40;
  if (EverParseIsError(positionAfterthenCount))
  {
    positionAfterIfBlockD40 = positionAfterthenCount;
  }
  else
  {
    uint8_t thenCount = Input[(uint32_t)positionAftercond];
    BOOLEAN thenCountConstraintIsOk = thenCount <= 64U;
    uint64_t
    positionAfterthenCount1 =
      EverParseCheckConstraintOk(thenCountConstraintIsOk,
        positionAfterthenCount);
    if (EverParseIsError(positionAfterthenCount1))
    {
      positionAfterIfBlockD40 = positionAfterthenCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT8, i.e., 1 byte */
      BOOLEAN hasBytes2 = 1ULL <= (InputLength - positionAfterthenCount1);
      uint64_t positionAfterelseCount;
      if (hasBytes2)
      {
        positionAfterelseCount = positionAfterthenCount1 + 1ULL;
      }
      else
      {
        positionAfterelseCount =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterthenCount1);
      }
      uint64_t positionAfterIfBlockD41;
      if (EverParseIsError(positionAfterelseCount))
      {
        positionAfterIfBlockD41 = positionAfterelseCount;
      }
      else
      {
        uint8_t elseCount = Input[(uint32_t)positionAfterthenCount1];
        BOOLEAN elseCountConstraintIsOk = elseCount <= 64U;
        uint64_t
        positionAfterelseCount1 =
          EverParseCheckConstraintOk(elseCountConstraintIsOk,
            positionAfterelseCount);
        if (EverParseIsError(positionAfterelseCount1))
        {
          positionAfterIfBlockD41 = positionAfterelseCount1;
        }
        else
        {
          /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
          BOOLEAN hasBytes3 = 2ULL <= (InputLength - positionAfterelseCount1);
          uint64_t positionAfterIfBlockD42;
          if (hasBytes3)
          {
            positionAfterIfBlockD42 = positionAfterelseCount1 + 2ULL;
          }
          else
          {
            positionAfterIfBlockD42 =
              EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                positionAfterelseCount1);
          }
          uint64_t positionAfterthenLen;
          if (EverParseIsSuccess(positionAfterIfBlockD42))
          {
            positionAfterthenLen = positionAfterIfBlockD42;
          }
          else
          {
            ErrorHandlerFn("_IfBlockD4",
              "then_len",
              EverParseErrorReasonOfResult(positionAfterIfBlockD42),
              EverParseGetValidatorErrorKind(positionAfterIfBlockD42),
              Ctxt,
              Input,
              positionAfterelseCount1);
            positionAfterthenLen = positionAfterIfBlockD42;
          }
          if (EverParseIsError(positionAfterthenLen))
          {
            positionAfterIfBlockD41 = positionAfterthenLen;
          }
          else
          {
            uint16_t r0 = Load16Le(Input + (uint32_t)positionAfterelseCount1);
            uint16_t thenLen = (uint16_t)(uint32_t)r0;
            /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
            BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterthenLen);
            uint64_t positionAfterIfBlockD43;
            if (hasBytes)
            {
              positionAfterIfBlockD43 = positionAfterthenLen + 2ULL;
            }
            else
            {
              positionAfterIfBlockD43 =
                EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                  positionAfterthenLen);
            }
            uint64_t positionAfterelseLen;
            if (EverParseIsSuccess(positionAfterIfBlockD43))
            {
              positionAfterelseLen = positionAfterIfBlockD43;
            }
            else
            {
              ErrorHandlerFn("_IfBlockD4",
                "else_len",
                EverParseErrorReasonOfResult(positionAfterIfBlockD43),
                EverParseGetValidatorErrorKind(positionAfterIfBlockD43),
                Ctxt,
                Input,
                positionAfterthenLen);
              positionAfterelseLen = positionAfterIfBlockD43;
            }
            if (EverParseIsError(positionAfterelseLen))
            {
              positionAfterIfBlockD41 = positionAfterelseLen;
            }
            else
            {
              uint16_t r = Load16Le(Input + (uint32_t)positionAfterthenLen);
              uint16_t elseLen = (uint16_t)(uint32_t)r;
              /* Validating field then_body */
              BOOLEAN
              hasEnoughBytes0 = (uint64_t)(uint32_t)thenLen <= (InputLength - positionAfterelseLen);
              uint64_t positionAfterIfBlockD44;
              if (!hasEnoughBytes0)
              {
                positionAfterIfBlockD44 =
                  EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                    positionAfterelseLen);
              }
              else
              {
                uint8_t *truncatedInput = Input;
                uint64_t truncatedInputLength = positionAfterelseLen + (uint64_t)(uint32_t)thenLen;
                uint64_t result = positionAfterelseLen;
                while (TRUE)
                {
                  uint64_t position = result;
                  BOOLEAN ite;
                  if (!(1ULL <= (truncatedInputLength - position)))
                  {
                    ite = TRUE;
                  }
                  else
                  {
                    uint64_t
                    positionAfterIfBlockD45 =
                      ValidateStmtD5(Ctxt,
                        ErrorHandlerFn,
                        truncatedInput,
                        truncatedInputLength,
                        position);
                    uint64_t result1;
                    if (EverParseIsSuccess(positionAfterIfBlockD45))
                    {
                      result1 = positionAfterIfBlockD45;
                    }
                    else
                    {
                      ErrorHandlerFn("_IfBlockD4",
                        "then_body.element",
                        EverParseErrorReasonOfResult(positionAfterIfBlockD45),
                        EverParseGetValidatorErrorKind(positionAfterIfBlockD45),
                        Ctxt,
                        truncatedInput,
                        position);
                      result1 = positionAfterIfBlockD45;
                    }
                    result = result1;
                    ite = EverParseIsError(result1);
                  }
                  if (ite)
                  {
                    break;
                  }
                }
                uint64_t res0 = result;
                positionAfterIfBlockD44 = res0;
              }
              uint64_t positionAfterthenBody;
              if (EverParseIsSuccess(positionAfterIfBlockD44))
              {
                positionAfterthenBody = positionAfterIfBlockD44;
              }
              else
              {
                ErrorHandlerFn("_IfBlockD4",
                  "then_body",
                  EverParseErrorReasonOfResult(positionAfterIfBlockD44),
                  EverParseGetValidatorErrorKind(positionAfterIfBlockD44),
                  Ctxt,
                  Input,
                  positionAfterelseLen);
                positionAfterthenBody = positionAfterIfBlockD44;
              }
              if (EverParseIsError(positionAfterthenBody))
              {
                positionAfterIfBlockD41 = positionAfterthenBody;
              }
              else
              {
                /* Validating field else_body */
                BOOLEAN
                hasEnoughBytes =
                  (uint64_t)(uint32_t)elseLen <= (InputLength - positionAfterthenBody);
                uint64_t positionAfterIfBlockD45;
                if (!hasEnoughBytes)
                {
                  positionAfterIfBlockD45 =
                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                      positionAfterthenBody);
                }
                else
                {
                  uint8_t *truncatedInput = Input;
                  uint64_t
                  truncatedInputLength = positionAfterthenBody + (uint64_t)(uint32_t)elseLen;
                  uint64_t result = positionAfterthenBody;
                  while (TRUE)
                  {
                    uint64_t position = result;
                    BOOLEAN ite;
                    if (!(1ULL <= (truncatedInputLength - position)))
                    {
                      ite = TRUE;
                    }
                    else
                    {
                      uint64_t
                      positionAfterIfBlockD46 =
                        ValidateStmtD5(Ctxt,
                          ErrorHandlerFn,
                          truncatedInput,
                          truncatedInputLength,
                          position);
                      uint64_t result1;
                      if (EverParseIsSuccess(positionAfterIfBlockD46))
                      {
                        result1 = positionAfterIfBlockD46;
                      }
                      else
                      {
                        ErrorHandlerFn("_IfBlockD4",
                          "else_body.element",
                          EverParseErrorReasonOfResult(positionAfterIfBlockD46),
                          EverParseGetValidatorErrorKind(positionAfterIfBlockD46),
                          Ctxt,
                          truncatedInput,
                          position);
                        result1 = positionAfterIfBlockD46;
                      }
                      result = result1;
                      ite = EverParseIsError(result1);
                    }
                    if (ite)
                    {
                      break;
                    }
                  }
                  uint64_t res0 = result;
                  positionAfterIfBlockD45 = res0;
                }
                if (EverParseIsSuccess(positionAfterIfBlockD45))
                {
                  positionAfterIfBlockD41 = positionAfterIfBlockD45;
                }
                else
                {
                  ErrorHandlerFn("_IfBlockD4",
                    "else_body",
                    EverParseErrorReasonOfResult(positionAfterIfBlockD45),
                    EverParseGetValidatorErrorKind(positionAfterIfBlockD45),
                    Ctxt,
                    Input,
                    positionAfterthenBody);
                  positionAfterIfBlockD41 = positionAfterIfBlockD45;
                }
              }
            }
          }
        }
      }
      if (EverParseIsSuccess(positionAfterIfBlockD41))
      {
        positionAfterIfBlockD40 = positionAfterIfBlockD41;
      }
      else
      {
        ErrorHandlerFn("_IfBlockD4",
          "else_count",
          EverParseErrorReasonOfResult(positionAfterIfBlockD41),
          EverParseGetValidatorErrorKind(positionAfterIfBlockD41),
          Ctxt,
          Input,
          positionAfterthenCount1);
        positionAfterIfBlockD40 = positionAfterIfBlockD41;
      }
    }
  }
  if (EverParseIsSuccess(positionAfterIfBlockD40))
  {
    return positionAfterIfBlockD40;
  }
  ErrorHandlerFn("_IfBlockD4",
    "then_count",
    EverParseErrorReasonOfResult(positionAfterIfBlockD40),
    EverParseGetValidatorErrorKind(positionAfterIfBlockD40),
    Ctxt,
    Input,
    positionAftercond);
  return positionAfterIfBlockD40;
}

static inline uint64_t
ValidateForBlockD4(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field var_idx */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterForBlockD4;
  if (hasBytes0)
  {
    positionAfterForBlockD4 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterForBlockD4 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res0;
  if (EverParseIsSuccess(positionAfterForBlockD4))
  {
    res0 = positionAfterForBlockD4;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD4",
      "var_idx",
      EverParseErrorReasonOfResult(positionAfterForBlockD4),
      EverParseGetValidatorErrorKind(positionAfterForBlockD4),
      Ctxt,
      Input,
      StartPosition);
    res0 = positionAfterForBlockD4;
  }
  uint64_t positionAftervarIdx = res0;
  if (EverParseIsError(positionAftervarIdx))
  {
    return positionAftervarIdx;
  }
  /* Validating field from_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes1 = 2ULL <= (InputLength - positionAftervarIdx);
  uint64_t positionAfterForBlockD40;
  if (hasBytes1)
  {
    positionAfterForBlockD40 = positionAftervarIdx + 2ULL;
  }
  else
  {
    positionAfterForBlockD40 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftervarIdx);
  }
  uint64_t res1;
  if (EverParseIsSuccess(positionAfterForBlockD40))
  {
    res1 = positionAfterForBlockD40;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD4",
      "from_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD40),
      EverParseGetValidatorErrorKind(positionAfterForBlockD40),
      Ctxt,
      Input,
      positionAftervarIdx);
    res1 = positionAfterForBlockD40;
  }
  uint64_t positionAfterfromVal = res1;
  if (EverParseIsError(positionAfterfromVal))
  {
    return positionAfterfromVal;
  }
  /* Validating field to_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes2 = 2ULL <= (InputLength - positionAfterfromVal);
  uint64_t positionAfterForBlockD41;
  if (hasBytes2)
  {
    positionAfterForBlockD41 = positionAfterfromVal + 2ULL;
  }
  else
  {
    positionAfterForBlockD41 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterfromVal);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterForBlockD41))
  {
    res = positionAfterForBlockD41;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD4",
      "to_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD41),
      EverParseGetValidatorErrorKind(positionAfterForBlockD41),
      Ctxt,
      Input,
      positionAfterfromVal);
    res = positionAfterForBlockD41;
  }
  uint64_t positionAftertoVal = res;
  if (EverParseIsError(positionAftertoVal))
  {
    return positionAftertoVal;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes3 = 1ULL <= (InputLength - positionAftertoVal);
  uint64_t positionAfterbodyCount;
  if (hasBytes3)
  {
    positionAfterbodyCount = positionAftertoVal + 1ULL;
  }
  else
  {
    positionAfterbodyCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertoVal);
  }
  uint64_t positionAfterForBlockD42;
  if (EverParseIsError(positionAfterbodyCount))
  {
    positionAfterForBlockD42 = positionAfterbodyCount;
  }
  else
  {
    uint8_t bodyCount = Input[(uint32_t)positionAftertoVal];
    BOOLEAN bodyCountConstraintIsOk = bodyCount <= 64U;
    uint64_t
    positionAfterbodyCount1 =
      EverParseCheckConstraintOk(bodyCountConstraintIsOk,
        positionAfterbodyCount);
    if (EverParseIsError(positionAfterbodyCount1))
    {
      positionAfterForBlockD42 = positionAfterbodyCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
      BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterbodyCount1);
      uint64_t positionAfterForBlockD43;
      if (hasBytes)
      {
        positionAfterForBlockD43 = positionAfterbodyCount1 + 2ULL;
      }
      else
      {
        positionAfterForBlockD43 =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterbodyCount1);
      }
      uint64_t positionAfterbodyLen;
      if (EverParseIsSuccess(positionAfterForBlockD43))
      {
        positionAfterbodyLen = positionAfterForBlockD43;
      }
      else
      {
        ErrorHandlerFn("_ForBlockD4",
          "body_len",
          EverParseErrorReasonOfResult(positionAfterForBlockD43),
          EverParseGetValidatorErrorKind(positionAfterForBlockD43),
          Ctxt,
          Input,
          positionAfterbodyCount1);
        positionAfterbodyLen = positionAfterForBlockD43;
      }
      if (EverParseIsError(positionAfterbodyLen))
      {
        positionAfterForBlockD42 = positionAfterbodyLen;
      }
      else
      {
        uint16_t r = Load16Le(Input + (uint32_t)positionAfterbodyCount1);
        uint16_t bodyLen = (uint16_t)(uint32_t)r;
        /* Validating field body */
        BOOLEAN
        hasEnoughBytes = (uint64_t)(uint32_t)bodyLen <= (InputLength - positionAfterbodyLen);
        uint64_t positionAfterForBlockD44;
        if (!hasEnoughBytes)
        {
          positionAfterForBlockD44 =
            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
              positionAfterbodyLen);
        }
        else
        {
          uint8_t *truncatedInput = Input;
          uint64_t truncatedInputLength = positionAfterbodyLen + (uint64_t)(uint32_t)bodyLen;
          uint64_t result = positionAfterbodyLen;
          while (TRUE)
          {
            uint64_t position = result;
            BOOLEAN ite;
            if (!(1ULL <= (truncatedInputLength - position)))
            {
              ite = TRUE;
            }
            else
            {
              uint64_t
              positionAfterForBlockD45 =
                ValidateStmtD5(Ctxt,
                  ErrorHandlerFn,
                  truncatedInput,
                  truncatedInputLength,
                  position);
              uint64_t result1;
              if (EverParseIsSuccess(positionAfterForBlockD45))
              {
                result1 = positionAfterForBlockD45;
              }
              else
              {
                ErrorHandlerFn("_ForBlockD4",
                  "body.element",
                  EverParseErrorReasonOfResult(positionAfterForBlockD45),
                  EverParseGetValidatorErrorKind(positionAfterForBlockD45),
                  Ctxt,
                  truncatedInput,
                  position);
                result1 = positionAfterForBlockD45;
              }
              result = result1;
              ite = EverParseIsError(result1);
            }
            if (ite)
            {
              break;
            }
          }
          uint64_t res2 = result;
          positionAfterForBlockD44 = res2;
        }
        if (EverParseIsSuccess(positionAfterForBlockD44))
        {
          positionAfterForBlockD42 = positionAfterForBlockD44;
        }
        else
        {
          ErrorHandlerFn("_ForBlockD4",
            "body",
            EverParseErrorReasonOfResult(positionAfterForBlockD44),
            EverParseGetValidatorErrorKind(positionAfterForBlockD44),
            Ctxt,
            Input,
            positionAfterbodyLen);
          positionAfterForBlockD42 = positionAfterForBlockD44;
        }
      }
    }
  }
  if (EverParseIsSuccess(positionAfterForBlockD42))
  {
    return positionAfterForBlockD42;
  }
  ErrorHandlerFn("_ForBlockD4",
    "body_count",
    EverParseErrorReasonOfResult(positionAfterForBlockD42),
    EverParseGetValidatorErrorKind(positionAfterForBlockD42),
    Ctxt,
    Input,
    positionAftertoVal);
  return positionAfterForBlockD42;
}

static inline uint64_t
ValidateStmtD4payload(
  uint8_t Tag,
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLen,
  uint64_t StartPosition
)
{
  if (Tag == 1U)
  {
    /* Validating field load_true */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD4payload;
    if (hasBytes)
    {
      positionAfterStmtD4payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD4payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "load_true",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 2U)
  {
    /* Validating field load_false */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD4payload;
    if (hasBytes)
    {
      positionAfterStmtD4payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD4payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "load_false",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 3U)
  {
    /* Validating field load_int */
    uint64_t positionAfterStmtD4payload = ValidateLoadInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "load_int",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 16U)
  {
    /* Validating field bool_not */
    uint64_t positionAfterStmtD4payload = ValidateBoolNot(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "bool_not",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 17U)
  {
    /* Validating field bool_and */
    uint64_t positionAfterStmtD4payload = ValidateBoolAnd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "bool_and",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 18U)
  {
    /* Validating field bool_or */
    uint64_t positionAfterStmtD4payload = ValidateBoolOr(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "bool_or",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 19U)
  {
    /* Validating field bool_xor */
    uint64_t positionAfterStmtD4payload = ValidateBoolXor(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "bool_xor",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 20U)
  {
    /* Validating field copy_bool */
    uint64_t positionAfterStmtD4payload = ValidateCopyBool(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "copy_bool",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 32U)
  {
    /* Validating field int_add */
    uint64_t positionAfterStmtD4payload = ValidateIntAdd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "int_add",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 33U)
  {
    /* Validating field int_sub */
    uint64_t positionAfterStmtD4payload = ValidateIntSub(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "int_sub",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 34U)
  {
    /* Validating field int_mul */
    uint64_t positionAfterStmtD4payload = ValidateIntMul(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "int_mul",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 35U)
  {
    /* Validating field copy_int */
    uint64_t positionAfterStmtD4payload = ValidateCopyInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "copy_int",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 48U)
  {
    /* Validating field cmp_eq */
    uint64_t positionAfterStmtD4payload = ValidateCmpEq(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "cmp_eq",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 49U)
  {
    /* Validating field cmp_ne */
    uint64_t positionAfterStmtD4payload = ValidateCmpNe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "cmp_ne",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 50U)
  {
    /* Validating field cmp_lt */
    uint64_t positionAfterStmtD4payload = ValidateCmpLt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "cmp_lt",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 51U)
  {
    /* Validating field cmp_gt */
    uint64_t positionAfterStmtD4payload = ValidateCmpGt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "cmp_gt",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 52U)
  {
    /* Validating field cmp_le */
    uint64_t positionAfterStmtD4payload = ValidateCmpLe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "cmp_le",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 53U)
  {
    /* Validating field cmp_ge */
    uint64_t positionAfterStmtD4payload = ValidateCmpGe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "cmp_ge",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 64U)
  {
    /* Validating field if_block */
    uint64_t
    positionAfterStmtD4payload =
      ValidateIfBlockD4(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "if_block",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 65U)
  {
    /* Validating field for_block */
    uint64_t
    positionAfterStmtD4payload =
      ValidateForBlockD4(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "for_block",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 80U)
  {
    /* Validating field ton_call */
    uint64_t positionAfterStmtD4payload = ValidateTonCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "ton_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 81U)
  {
    /* Validating field tof_call */
    uint64_t positionAfterStmtD4payload = ValidateTofCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "tof_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 82U)
  {
    /* Validating field tp_call */
    uint64_t positionAfterStmtD4payload = ValidateTpCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "tp_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 96U)
  {
    /* Validating field ctu_call */
    uint64_t positionAfterStmtD4payload = ValidateCtuCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "ctu_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 97U)
  {
    /* Validating field ctd_call */
    uint64_t positionAfterStmtD4payload = ValidateCtdCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "ctd_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 98U)
  {
    /* Validating field ctud_call */
    uint64_t positionAfterStmtD4payload = ValidateCtudCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "ctud_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 112U)
  {
    /* Validating field sr_call */
    uint64_t positionAfterStmtD4payload = ValidateSrCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "sr_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 113U)
  {
    /* Validating field rs_call */
    uint64_t positionAfterStmtD4payload = ValidateRsCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "rs_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 114U)
  {
    /* Validating field rtrig_call */
    uint64_t positionAfterStmtD4payload = ValidateRtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "rtrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  if (Tag == 115U)
  {
    /* Validating field ftrig_call */
    uint64_t positionAfterStmtD4payload = ValidateFtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD4payload))
    {
      return positionAfterStmtD4payload;
    }
    ErrorHandlerFn("_StmtD4Payload",
      "ftrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD4payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD4payload;
  }
  uint64_t
  positionAfterStmtD4payload =
    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_IMPOSSIBLE,
      StartPosition);
  if (EverParseIsSuccess(positionAfterStmtD4payload))
  {
    return positionAfterStmtD4payload;
  }
  ErrorHandlerFn("_StmtD4Payload",
    "_x_60",
    EverParseErrorReasonOfResult(positionAfterStmtD4payload),
    EverParseGetValidatorErrorKind(positionAfterStmtD4payload),
    Ctxt,
    Input,
    StartPosition);
  return positionAfterStmtD4payload;
}

static inline uint64_t
ValidateStmtD4(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterStmtD4;
  if (hasBytes0)
  {
    positionAfterStmtD4 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterStmtD4 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t positionAftertag;
  if (EverParseIsSuccess(positionAfterStmtD4))
  {
    positionAftertag = positionAfterStmtD4;
  }
  else
  {
    ErrorHandlerFn("_StmtD4",
      "tag",
      EverParseErrorReasonOfResult(positionAfterStmtD4),
      EverParseGetValidatorErrorKind(positionAfterStmtD4),
      Ctxt,
      Input,
      StartPosition);
    positionAftertag = positionAfterStmtD4;
  }
  if (EverParseIsError(positionAftertag))
  {
    return positionAftertag;
  }
  uint8_t tag = Input[(uint32_t)StartPosition];
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes = 2ULL <= (InputLength - positionAftertag);
  uint64_t positionAfterStmtD40;
  if (hasBytes)
  {
    positionAfterStmtD40 = positionAftertag + 2ULL;
  }
  else
  {
    positionAfterStmtD40 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertag);
  }
  uint64_t positionAfterlen;
  if (EverParseIsSuccess(positionAfterStmtD40))
  {
    positionAfterlen = positionAfterStmtD40;
  }
  else
  {
    ErrorHandlerFn("_StmtD4",
      "len",
      EverParseErrorReasonOfResult(positionAfterStmtD40),
      EverParseGetValidatorErrorKind(positionAfterStmtD40),
      Ctxt,
      Input,
      positionAftertag);
    positionAfterlen = positionAfterStmtD40;
  }
  if (EverParseIsError(positionAfterlen))
  {
    return positionAfterlen;
  }
  uint16_t r = Load16Le(Input + (uint32_t)positionAftertag);
  uint16_t len = (uint16_t)(uint32_t)r;
  /* Validating field payload */
  BOOLEAN hasEnoughBytes = (uint64_t)(uint32_t)len <= (InputLength - positionAfterlen);
  uint64_t positionAfterStmtD41;
  if (!hasEnoughBytes)
  {
    positionAfterStmtD41 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterlen);
  }
  else
  {
    uint8_t *truncatedInput = Input;
    uint64_t truncatedInputLength = positionAfterlen + (uint64_t)(uint32_t)len;
    uint64_t result = positionAfterlen;
    while (TRUE)
    {
      uint64_t position = result;
      BOOLEAN ite;
      if (!(1ULL <= (truncatedInputLength - position)))
      {
        ite = TRUE;
      }
      else
      {
        uint64_t
        positionAfterStmtD42 =
          ValidateStmtD4payload(tag,
            Ctxt,
            ErrorHandlerFn,
            truncatedInput,
            truncatedInputLength,
            position);
        uint64_t result1;
        if (EverParseIsSuccess(positionAfterStmtD42))
        {
          result1 = positionAfterStmtD42;
        }
        else
        {
          ErrorHandlerFn("_StmtD4",
            "payload.element",
            EverParseErrorReasonOfResult(positionAfterStmtD42),
            EverParseGetValidatorErrorKind(positionAfterStmtD42),
            Ctxt,
            truncatedInput,
            position);
          result1 = positionAfterStmtD42;
        }
        result = result1;
        ite = EverParseIsError(result1);
      }
      if (ite)
      {
        break;
      }
    }
    uint64_t res = result;
    positionAfterStmtD41 = res;
  }
  if (EverParseIsSuccess(positionAfterStmtD41))
  {
    return positionAfterStmtD41;
  }
  ErrorHandlerFn("_StmtD4",
    "payload",
    EverParseErrorReasonOfResult(positionAfterStmtD41),
    EverParseGetValidatorErrorKind(positionAfterStmtD41),
    Ctxt,
    Input,
    positionAfterlen);
  return positionAfterStmtD41;
}

static inline uint64_t
ValidateIfBlockD3(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field cond */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterIfBlockD3;
  if (hasBytes0)
  {
    positionAfterIfBlockD3 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterIfBlockD3 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterIfBlockD3))
  {
    res = positionAfterIfBlockD3;
  }
  else
  {
    ErrorHandlerFn("_IfBlockD3",
      "cond",
      EverParseErrorReasonOfResult(positionAfterIfBlockD3),
      EverParseGetValidatorErrorKind(positionAfterIfBlockD3),
      Ctxt,
      Input,
      StartPosition);
    res = positionAfterIfBlockD3;
  }
  uint64_t positionAftercond = res;
  if (EverParseIsError(positionAftercond))
  {
    return positionAftercond;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes1 = 1ULL <= (InputLength - positionAftercond);
  uint64_t positionAfterthenCount;
  if (hasBytes1)
  {
    positionAfterthenCount = positionAftercond + 1ULL;
  }
  else
  {
    positionAfterthenCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftercond);
  }
  uint64_t positionAfterIfBlockD30;
  if (EverParseIsError(positionAfterthenCount))
  {
    positionAfterIfBlockD30 = positionAfterthenCount;
  }
  else
  {
    uint8_t thenCount = Input[(uint32_t)positionAftercond];
    BOOLEAN thenCountConstraintIsOk = thenCount <= 64U;
    uint64_t
    positionAfterthenCount1 =
      EverParseCheckConstraintOk(thenCountConstraintIsOk,
        positionAfterthenCount);
    if (EverParseIsError(positionAfterthenCount1))
    {
      positionAfterIfBlockD30 = positionAfterthenCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT8, i.e., 1 byte */
      BOOLEAN hasBytes2 = 1ULL <= (InputLength - positionAfterthenCount1);
      uint64_t positionAfterelseCount;
      if (hasBytes2)
      {
        positionAfterelseCount = positionAfterthenCount1 + 1ULL;
      }
      else
      {
        positionAfterelseCount =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterthenCount1);
      }
      uint64_t positionAfterIfBlockD31;
      if (EverParseIsError(positionAfterelseCount))
      {
        positionAfterIfBlockD31 = positionAfterelseCount;
      }
      else
      {
        uint8_t elseCount = Input[(uint32_t)positionAfterthenCount1];
        BOOLEAN elseCountConstraintIsOk = elseCount <= 64U;
        uint64_t
        positionAfterelseCount1 =
          EverParseCheckConstraintOk(elseCountConstraintIsOk,
            positionAfterelseCount);
        if (EverParseIsError(positionAfterelseCount1))
        {
          positionAfterIfBlockD31 = positionAfterelseCount1;
        }
        else
        {
          /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
          BOOLEAN hasBytes3 = 2ULL <= (InputLength - positionAfterelseCount1);
          uint64_t positionAfterIfBlockD32;
          if (hasBytes3)
          {
            positionAfterIfBlockD32 = positionAfterelseCount1 + 2ULL;
          }
          else
          {
            positionAfterIfBlockD32 =
              EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                positionAfterelseCount1);
          }
          uint64_t positionAfterthenLen;
          if (EverParseIsSuccess(positionAfterIfBlockD32))
          {
            positionAfterthenLen = positionAfterIfBlockD32;
          }
          else
          {
            ErrorHandlerFn("_IfBlockD3",
              "then_len",
              EverParseErrorReasonOfResult(positionAfterIfBlockD32),
              EverParseGetValidatorErrorKind(positionAfterIfBlockD32),
              Ctxt,
              Input,
              positionAfterelseCount1);
            positionAfterthenLen = positionAfterIfBlockD32;
          }
          if (EverParseIsError(positionAfterthenLen))
          {
            positionAfterIfBlockD31 = positionAfterthenLen;
          }
          else
          {
            uint16_t r0 = Load16Le(Input + (uint32_t)positionAfterelseCount1);
            uint16_t thenLen = (uint16_t)(uint32_t)r0;
            /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
            BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterthenLen);
            uint64_t positionAfterIfBlockD33;
            if (hasBytes)
            {
              positionAfterIfBlockD33 = positionAfterthenLen + 2ULL;
            }
            else
            {
              positionAfterIfBlockD33 =
                EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                  positionAfterthenLen);
            }
            uint64_t positionAfterelseLen;
            if (EverParseIsSuccess(positionAfterIfBlockD33))
            {
              positionAfterelseLen = positionAfterIfBlockD33;
            }
            else
            {
              ErrorHandlerFn("_IfBlockD3",
                "else_len",
                EverParseErrorReasonOfResult(positionAfterIfBlockD33),
                EverParseGetValidatorErrorKind(positionAfterIfBlockD33),
                Ctxt,
                Input,
                positionAfterthenLen);
              positionAfterelseLen = positionAfterIfBlockD33;
            }
            if (EverParseIsError(positionAfterelseLen))
            {
              positionAfterIfBlockD31 = positionAfterelseLen;
            }
            else
            {
              uint16_t r = Load16Le(Input + (uint32_t)positionAfterthenLen);
              uint16_t elseLen = (uint16_t)(uint32_t)r;
              /* Validating field then_body */
              BOOLEAN
              hasEnoughBytes0 = (uint64_t)(uint32_t)thenLen <= (InputLength - positionAfterelseLen);
              uint64_t positionAfterIfBlockD34;
              if (!hasEnoughBytes0)
              {
                positionAfterIfBlockD34 =
                  EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                    positionAfterelseLen);
              }
              else
              {
                uint8_t *truncatedInput = Input;
                uint64_t truncatedInputLength = positionAfterelseLen + (uint64_t)(uint32_t)thenLen;
                uint64_t result = positionAfterelseLen;
                while (TRUE)
                {
                  uint64_t position = result;
                  BOOLEAN ite;
                  if (!(1ULL <= (truncatedInputLength - position)))
                  {
                    ite = TRUE;
                  }
                  else
                  {
                    uint64_t
                    positionAfterIfBlockD35 =
                      ValidateStmtD4(Ctxt,
                        ErrorHandlerFn,
                        truncatedInput,
                        truncatedInputLength,
                        position);
                    uint64_t result1;
                    if (EverParseIsSuccess(positionAfterIfBlockD35))
                    {
                      result1 = positionAfterIfBlockD35;
                    }
                    else
                    {
                      ErrorHandlerFn("_IfBlockD3",
                        "then_body.element",
                        EverParseErrorReasonOfResult(positionAfterIfBlockD35),
                        EverParseGetValidatorErrorKind(positionAfterIfBlockD35),
                        Ctxt,
                        truncatedInput,
                        position);
                      result1 = positionAfterIfBlockD35;
                    }
                    result = result1;
                    ite = EverParseIsError(result1);
                  }
                  if (ite)
                  {
                    break;
                  }
                }
                uint64_t res0 = result;
                positionAfterIfBlockD34 = res0;
              }
              uint64_t positionAfterthenBody;
              if (EverParseIsSuccess(positionAfterIfBlockD34))
              {
                positionAfterthenBody = positionAfterIfBlockD34;
              }
              else
              {
                ErrorHandlerFn("_IfBlockD3",
                  "then_body",
                  EverParseErrorReasonOfResult(positionAfterIfBlockD34),
                  EverParseGetValidatorErrorKind(positionAfterIfBlockD34),
                  Ctxt,
                  Input,
                  positionAfterelseLen);
                positionAfterthenBody = positionAfterIfBlockD34;
              }
              if (EverParseIsError(positionAfterthenBody))
              {
                positionAfterIfBlockD31 = positionAfterthenBody;
              }
              else
              {
                /* Validating field else_body */
                BOOLEAN
                hasEnoughBytes =
                  (uint64_t)(uint32_t)elseLen <= (InputLength - positionAfterthenBody);
                uint64_t positionAfterIfBlockD35;
                if (!hasEnoughBytes)
                {
                  positionAfterIfBlockD35 =
                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                      positionAfterthenBody);
                }
                else
                {
                  uint8_t *truncatedInput = Input;
                  uint64_t
                  truncatedInputLength = positionAfterthenBody + (uint64_t)(uint32_t)elseLen;
                  uint64_t result = positionAfterthenBody;
                  while (TRUE)
                  {
                    uint64_t position = result;
                    BOOLEAN ite;
                    if (!(1ULL <= (truncatedInputLength - position)))
                    {
                      ite = TRUE;
                    }
                    else
                    {
                      uint64_t
                      positionAfterIfBlockD36 =
                        ValidateStmtD4(Ctxt,
                          ErrorHandlerFn,
                          truncatedInput,
                          truncatedInputLength,
                          position);
                      uint64_t result1;
                      if (EverParseIsSuccess(positionAfterIfBlockD36))
                      {
                        result1 = positionAfterIfBlockD36;
                      }
                      else
                      {
                        ErrorHandlerFn("_IfBlockD3",
                          "else_body.element",
                          EverParseErrorReasonOfResult(positionAfterIfBlockD36),
                          EverParseGetValidatorErrorKind(positionAfterIfBlockD36),
                          Ctxt,
                          truncatedInput,
                          position);
                        result1 = positionAfterIfBlockD36;
                      }
                      result = result1;
                      ite = EverParseIsError(result1);
                    }
                    if (ite)
                    {
                      break;
                    }
                  }
                  uint64_t res0 = result;
                  positionAfterIfBlockD35 = res0;
                }
                if (EverParseIsSuccess(positionAfterIfBlockD35))
                {
                  positionAfterIfBlockD31 = positionAfterIfBlockD35;
                }
                else
                {
                  ErrorHandlerFn("_IfBlockD3",
                    "else_body",
                    EverParseErrorReasonOfResult(positionAfterIfBlockD35),
                    EverParseGetValidatorErrorKind(positionAfterIfBlockD35),
                    Ctxt,
                    Input,
                    positionAfterthenBody);
                  positionAfterIfBlockD31 = positionAfterIfBlockD35;
                }
              }
            }
          }
        }
      }
      if (EverParseIsSuccess(positionAfterIfBlockD31))
      {
        positionAfterIfBlockD30 = positionAfterIfBlockD31;
      }
      else
      {
        ErrorHandlerFn("_IfBlockD3",
          "else_count",
          EverParseErrorReasonOfResult(positionAfterIfBlockD31),
          EverParseGetValidatorErrorKind(positionAfterIfBlockD31),
          Ctxt,
          Input,
          positionAfterthenCount1);
        positionAfterIfBlockD30 = positionAfterIfBlockD31;
      }
    }
  }
  if (EverParseIsSuccess(positionAfterIfBlockD30))
  {
    return positionAfterIfBlockD30;
  }
  ErrorHandlerFn("_IfBlockD3",
    "then_count",
    EverParseErrorReasonOfResult(positionAfterIfBlockD30),
    EverParseGetValidatorErrorKind(positionAfterIfBlockD30),
    Ctxt,
    Input,
    positionAftercond);
  return positionAfterIfBlockD30;
}

static inline uint64_t
ValidateForBlockD3(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field var_idx */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterForBlockD3;
  if (hasBytes0)
  {
    positionAfterForBlockD3 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterForBlockD3 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res0;
  if (EverParseIsSuccess(positionAfterForBlockD3))
  {
    res0 = positionAfterForBlockD3;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD3",
      "var_idx",
      EverParseErrorReasonOfResult(positionAfterForBlockD3),
      EverParseGetValidatorErrorKind(positionAfterForBlockD3),
      Ctxt,
      Input,
      StartPosition);
    res0 = positionAfterForBlockD3;
  }
  uint64_t positionAftervarIdx = res0;
  if (EverParseIsError(positionAftervarIdx))
  {
    return positionAftervarIdx;
  }
  /* Validating field from_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes1 = 2ULL <= (InputLength - positionAftervarIdx);
  uint64_t positionAfterForBlockD30;
  if (hasBytes1)
  {
    positionAfterForBlockD30 = positionAftervarIdx + 2ULL;
  }
  else
  {
    positionAfterForBlockD30 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftervarIdx);
  }
  uint64_t res1;
  if (EverParseIsSuccess(positionAfterForBlockD30))
  {
    res1 = positionAfterForBlockD30;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD3",
      "from_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD30),
      EverParseGetValidatorErrorKind(positionAfterForBlockD30),
      Ctxt,
      Input,
      positionAftervarIdx);
    res1 = positionAfterForBlockD30;
  }
  uint64_t positionAfterfromVal = res1;
  if (EverParseIsError(positionAfterfromVal))
  {
    return positionAfterfromVal;
  }
  /* Validating field to_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes2 = 2ULL <= (InputLength - positionAfterfromVal);
  uint64_t positionAfterForBlockD31;
  if (hasBytes2)
  {
    positionAfterForBlockD31 = positionAfterfromVal + 2ULL;
  }
  else
  {
    positionAfterForBlockD31 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterfromVal);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterForBlockD31))
  {
    res = positionAfterForBlockD31;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD3",
      "to_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD31),
      EverParseGetValidatorErrorKind(positionAfterForBlockD31),
      Ctxt,
      Input,
      positionAfterfromVal);
    res = positionAfterForBlockD31;
  }
  uint64_t positionAftertoVal = res;
  if (EverParseIsError(positionAftertoVal))
  {
    return positionAftertoVal;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes3 = 1ULL <= (InputLength - positionAftertoVal);
  uint64_t positionAfterbodyCount;
  if (hasBytes3)
  {
    positionAfterbodyCount = positionAftertoVal + 1ULL;
  }
  else
  {
    positionAfterbodyCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertoVal);
  }
  uint64_t positionAfterForBlockD32;
  if (EverParseIsError(positionAfterbodyCount))
  {
    positionAfterForBlockD32 = positionAfterbodyCount;
  }
  else
  {
    uint8_t bodyCount = Input[(uint32_t)positionAftertoVal];
    BOOLEAN bodyCountConstraintIsOk = bodyCount <= 64U;
    uint64_t
    positionAfterbodyCount1 =
      EverParseCheckConstraintOk(bodyCountConstraintIsOk,
        positionAfterbodyCount);
    if (EverParseIsError(positionAfterbodyCount1))
    {
      positionAfterForBlockD32 = positionAfterbodyCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
      BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterbodyCount1);
      uint64_t positionAfterForBlockD33;
      if (hasBytes)
      {
        positionAfterForBlockD33 = positionAfterbodyCount1 + 2ULL;
      }
      else
      {
        positionAfterForBlockD33 =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterbodyCount1);
      }
      uint64_t positionAfterbodyLen;
      if (EverParseIsSuccess(positionAfterForBlockD33))
      {
        positionAfterbodyLen = positionAfterForBlockD33;
      }
      else
      {
        ErrorHandlerFn("_ForBlockD3",
          "body_len",
          EverParseErrorReasonOfResult(positionAfterForBlockD33),
          EverParseGetValidatorErrorKind(positionAfterForBlockD33),
          Ctxt,
          Input,
          positionAfterbodyCount1);
        positionAfterbodyLen = positionAfterForBlockD33;
      }
      if (EverParseIsError(positionAfterbodyLen))
      {
        positionAfterForBlockD32 = positionAfterbodyLen;
      }
      else
      {
        uint16_t r = Load16Le(Input + (uint32_t)positionAfterbodyCount1);
        uint16_t bodyLen = (uint16_t)(uint32_t)r;
        /* Validating field body */
        BOOLEAN
        hasEnoughBytes = (uint64_t)(uint32_t)bodyLen <= (InputLength - positionAfterbodyLen);
        uint64_t positionAfterForBlockD34;
        if (!hasEnoughBytes)
        {
          positionAfterForBlockD34 =
            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
              positionAfterbodyLen);
        }
        else
        {
          uint8_t *truncatedInput = Input;
          uint64_t truncatedInputLength = positionAfterbodyLen + (uint64_t)(uint32_t)bodyLen;
          uint64_t result = positionAfterbodyLen;
          while (TRUE)
          {
            uint64_t position = result;
            BOOLEAN ite;
            if (!(1ULL <= (truncatedInputLength - position)))
            {
              ite = TRUE;
            }
            else
            {
              uint64_t
              positionAfterForBlockD35 =
                ValidateStmtD4(Ctxt,
                  ErrorHandlerFn,
                  truncatedInput,
                  truncatedInputLength,
                  position);
              uint64_t result1;
              if (EverParseIsSuccess(positionAfterForBlockD35))
              {
                result1 = positionAfterForBlockD35;
              }
              else
              {
                ErrorHandlerFn("_ForBlockD3",
                  "body.element",
                  EverParseErrorReasonOfResult(positionAfterForBlockD35),
                  EverParseGetValidatorErrorKind(positionAfterForBlockD35),
                  Ctxt,
                  truncatedInput,
                  position);
                result1 = positionAfterForBlockD35;
              }
              result = result1;
              ite = EverParseIsError(result1);
            }
            if (ite)
            {
              break;
            }
          }
          uint64_t res2 = result;
          positionAfterForBlockD34 = res2;
        }
        if (EverParseIsSuccess(positionAfterForBlockD34))
        {
          positionAfterForBlockD32 = positionAfterForBlockD34;
        }
        else
        {
          ErrorHandlerFn("_ForBlockD3",
            "body",
            EverParseErrorReasonOfResult(positionAfterForBlockD34),
            EverParseGetValidatorErrorKind(positionAfterForBlockD34),
            Ctxt,
            Input,
            positionAfterbodyLen);
          positionAfterForBlockD32 = positionAfterForBlockD34;
        }
      }
    }
  }
  if (EverParseIsSuccess(positionAfterForBlockD32))
  {
    return positionAfterForBlockD32;
  }
  ErrorHandlerFn("_ForBlockD3",
    "body_count",
    EverParseErrorReasonOfResult(positionAfterForBlockD32),
    EverParseGetValidatorErrorKind(positionAfterForBlockD32),
    Ctxt,
    Input,
    positionAftertoVal);
  return positionAfterForBlockD32;
}

static inline uint64_t
ValidateStmtD3payload(
  uint8_t Tag,
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLen,
  uint64_t StartPosition
)
{
  if (Tag == 1U)
  {
    /* Validating field load_true */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD3payload;
    if (hasBytes)
    {
      positionAfterStmtD3payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD3payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "load_true",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 2U)
  {
    /* Validating field load_false */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD3payload;
    if (hasBytes)
    {
      positionAfterStmtD3payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD3payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "load_false",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 3U)
  {
    /* Validating field load_int */
    uint64_t positionAfterStmtD3payload = ValidateLoadInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "load_int",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 16U)
  {
    /* Validating field bool_not */
    uint64_t positionAfterStmtD3payload = ValidateBoolNot(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "bool_not",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 17U)
  {
    /* Validating field bool_and */
    uint64_t positionAfterStmtD3payload = ValidateBoolAnd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "bool_and",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 18U)
  {
    /* Validating field bool_or */
    uint64_t positionAfterStmtD3payload = ValidateBoolOr(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "bool_or",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 19U)
  {
    /* Validating field bool_xor */
    uint64_t positionAfterStmtD3payload = ValidateBoolXor(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "bool_xor",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 20U)
  {
    /* Validating field copy_bool */
    uint64_t positionAfterStmtD3payload = ValidateCopyBool(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "copy_bool",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 32U)
  {
    /* Validating field int_add */
    uint64_t positionAfterStmtD3payload = ValidateIntAdd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "int_add",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 33U)
  {
    /* Validating field int_sub */
    uint64_t positionAfterStmtD3payload = ValidateIntSub(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "int_sub",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 34U)
  {
    /* Validating field int_mul */
    uint64_t positionAfterStmtD3payload = ValidateIntMul(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "int_mul",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 35U)
  {
    /* Validating field copy_int */
    uint64_t positionAfterStmtD3payload = ValidateCopyInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "copy_int",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 48U)
  {
    /* Validating field cmp_eq */
    uint64_t positionAfterStmtD3payload = ValidateCmpEq(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "cmp_eq",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 49U)
  {
    /* Validating field cmp_ne */
    uint64_t positionAfterStmtD3payload = ValidateCmpNe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "cmp_ne",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 50U)
  {
    /* Validating field cmp_lt */
    uint64_t positionAfterStmtD3payload = ValidateCmpLt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "cmp_lt",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 51U)
  {
    /* Validating field cmp_gt */
    uint64_t positionAfterStmtD3payload = ValidateCmpGt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "cmp_gt",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 52U)
  {
    /* Validating field cmp_le */
    uint64_t positionAfterStmtD3payload = ValidateCmpLe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "cmp_le",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 53U)
  {
    /* Validating field cmp_ge */
    uint64_t positionAfterStmtD3payload = ValidateCmpGe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "cmp_ge",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 64U)
  {
    /* Validating field if_block */
    uint64_t
    positionAfterStmtD3payload =
      ValidateIfBlockD3(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "if_block",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 65U)
  {
    /* Validating field for_block */
    uint64_t
    positionAfterStmtD3payload =
      ValidateForBlockD3(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "for_block",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 80U)
  {
    /* Validating field ton_call */
    uint64_t positionAfterStmtD3payload = ValidateTonCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "ton_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 81U)
  {
    /* Validating field tof_call */
    uint64_t positionAfterStmtD3payload = ValidateTofCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "tof_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 82U)
  {
    /* Validating field tp_call */
    uint64_t positionAfterStmtD3payload = ValidateTpCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "tp_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 96U)
  {
    /* Validating field ctu_call */
    uint64_t positionAfterStmtD3payload = ValidateCtuCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "ctu_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 97U)
  {
    /* Validating field ctd_call */
    uint64_t positionAfterStmtD3payload = ValidateCtdCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "ctd_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 98U)
  {
    /* Validating field ctud_call */
    uint64_t positionAfterStmtD3payload = ValidateCtudCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "ctud_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 112U)
  {
    /* Validating field sr_call */
    uint64_t positionAfterStmtD3payload = ValidateSrCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "sr_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 113U)
  {
    /* Validating field rs_call */
    uint64_t positionAfterStmtD3payload = ValidateRsCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "rs_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 114U)
  {
    /* Validating field rtrig_call */
    uint64_t positionAfterStmtD3payload = ValidateRtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "rtrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  if (Tag == 115U)
  {
    /* Validating field ftrig_call */
    uint64_t positionAfterStmtD3payload = ValidateFtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD3payload))
    {
      return positionAfterStmtD3payload;
    }
    ErrorHandlerFn("_StmtD3Payload",
      "ftrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD3payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD3payload;
  }
  uint64_t
  positionAfterStmtD3payload =
    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_IMPOSSIBLE,
      StartPosition);
  if (EverParseIsSuccess(positionAfterStmtD3payload))
  {
    return positionAfterStmtD3payload;
  }
  ErrorHandlerFn("_StmtD3Payload",
    "_x_91",
    EverParseErrorReasonOfResult(positionAfterStmtD3payload),
    EverParseGetValidatorErrorKind(positionAfterStmtD3payload),
    Ctxt,
    Input,
    StartPosition);
  return positionAfterStmtD3payload;
}

static inline uint64_t
ValidateStmtD3(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterStmtD3;
  if (hasBytes0)
  {
    positionAfterStmtD3 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterStmtD3 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t positionAftertag;
  if (EverParseIsSuccess(positionAfterStmtD3))
  {
    positionAftertag = positionAfterStmtD3;
  }
  else
  {
    ErrorHandlerFn("_StmtD3",
      "tag",
      EverParseErrorReasonOfResult(positionAfterStmtD3),
      EverParseGetValidatorErrorKind(positionAfterStmtD3),
      Ctxt,
      Input,
      StartPosition);
    positionAftertag = positionAfterStmtD3;
  }
  if (EverParseIsError(positionAftertag))
  {
    return positionAftertag;
  }
  uint8_t tag = Input[(uint32_t)StartPosition];
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes = 2ULL <= (InputLength - positionAftertag);
  uint64_t positionAfterStmtD30;
  if (hasBytes)
  {
    positionAfterStmtD30 = positionAftertag + 2ULL;
  }
  else
  {
    positionAfterStmtD30 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertag);
  }
  uint64_t positionAfterlen;
  if (EverParseIsSuccess(positionAfterStmtD30))
  {
    positionAfterlen = positionAfterStmtD30;
  }
  else
  {
    ErrorHandlerFn("_StmtD3",
      "len",
      EverParseErrorReasonOfResult(positionAfterStmtD30),
      EverParseGetValidatorErrorKind(positionAfterStmtD30),
      Ctxt,
      Input,
      positionAftertag);
    positionAfterlen = positionAfterStmtD30;
  }
  if (EverParseIsError(positionAfterlen))
  {
    return positionAfterlen;
  }
  uint16_t r = Load16Le(Input + (uint32_t)positionAftertag);
  uint16_t len = (uint16_t)(uint32_t)r;
  /* Validating field payload */
  BOOLEAN hasEnoughBytes = (uint64_t)(uint32_t)len <= (InputLength - positionAfterlen);
  uint64_t positionAfterStmtD31;
  if (!hasEnoughBytes)
  {
    positionAfterStmtD31 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterlen);
  }
  else
  {
    uint8_t *truncatedInput = Input;
    uint64_t truncatedInputLength = positionAfterlen + (uint64_t)(uint32_t)len;
    uint64_t result = positionAfterlen;
    while (TRUE)
    {
      uint64_t position = result;
      BOOLEAN ite;
      if (!(1ULL <= (truncatedInputLength - position)))
      {
        ite = TRUE;
      }
      else
      {
        uint64_t
        positionAfterStmtD32 =
          ValidateStmtD3payload(tag,
            Ctxt,
            ErrorHandlerFn,
            truncatedInput,
            truncatedInputLength,
            position);
        uint64_t result1;
        if (EverParseIsSuccess(positionAfterStmtD32))
        {
          result1 = positionAfterStmtD32;
        }
        else
        {
          ErrorHandlerFn("_StmtD3",
            "payload.element",
            EverParseErrorReasonOfResult(positionAfterStmtD32),
            EverParseGetValidatorErrorKind(positionAfterStmtD32),
            Ctxt,
            truncatedInput,
            position);
          result1 = positionAfterStmtD32;
        }
        result = result1;
        ite = EverParseIsError(result1);
      }
      if (ite)
      {
        break;
      }
    }
    uint64_t res = result;
    positionAfterStmtD31 = res;
  }
  if (EverParseIsSuccess(positionAfterStmtD31))
  {
    return positionAfterStmtD31;
  }
  ErrorHandlerFn("_StmtD3",
    "payload",
    EverParseErrorReasonOfResult(positionAfterStmtD31),
    EverParseGetValidatorErrorKind(positionAfterStmtD31),
    Ctxt,
    Input,
    positionAfterlen);
  return positionAfterStmtD31;
}

static inline uint64_t
ValidateIfBlockD2(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field cond */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterIfBlockD2;
  if (hasBytes0)
  {
    positionAfterIfBlockD2 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterIfBlockD2 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterIfBlockD2))
  {
    res = positionAfterIfBlockD2;
  }
  else
  {
    ErrorHandlerFn("_IfBlockD2",
      "cond",
      EverParseErrorReasonOfResult(positionAfterIfBlockD2),
      EverParseGetValidatorErrorKind(positionAfterIfBlockD2),
      Ctxt,
      Input,
      StartPosition);
    res = positionAfterIfBlockD2;
  }
  uint64_t positionAftercond = res;
  if (EverParseIsError(positionAftercond))
  {
    return positionAftercond;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes1 = 1ULL <= (InputLength - positionAftercond);
  uint64_t positionAfterthenCount;
  if (hasBytes1)
  {
    positionAfterthenCount = positionAftercond + 1ULL;
  }
  else
  {
    positionAfterthenCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftercond);
  }
  uint64_t positionAfterIfBlockD20;
  if (EverParseIsError(positionAfterthenCount))
  {
    positionAfterIfBlockD20 = positionAfterthenCount;
  }
  else
  {
    uint8_t thenCount = Input[(uint32_t)positionAftercond];
    BOOLEAN thenCountConstraintIsOk = thenCount <= 64U;
    uint64_t
    positionAfterthenCount1 =
      EverParseCheckConstraintOk(thenCountConstraintIsOk,
        positionAfterthenCount);
    if (EverParseIsError(positionAfterthenCount1))
    {
      positionAfterIfBlockD20 = positionAfterthenCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT8, i.e., 1 byte */
      BOOLEAN hasBytes2 = 1ULL <= (InputLength - positionAfterthenCount1);
      uint64_t positionAfterelseCount;
      if (hasBytes2)
      {
        positionAfterelseCount = positionAfterthenCount1 + 1ULL;
      }
      else
      {
        positionAfterelseCount =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterthenCount1);
      }
      uint64_t positionAfterIfBlockD21;
      if (EverParseIsError(positionAfterelseCount))
      {
        positionAfterIfBlockD21 = positionAfterelseCount;
      }
      else
      {
        uint8_t elseCount = Input[(uint32_t)positionAfterthenCount1];
        BOOLEAN elseCountConstraintIsOk = elseCount <= 64U;
        uint64_t
        positionAfterelseCount1 =
          EverParseCheckConstraintOk(elseCountConstraintIsOk,
            positionAfterelseCount);
        if (EverParseIsError(positionAfterelseCount1))
        {
          positionAfterIfBlockD21 = positionAfterelseCount1;
        }
        else
        {
          /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
          BOOLEAN hasBytes3 = 2ULL <= (InputLength - positionAfterelseCount1);
          uint64_t positionAfterIfBlockD22;
          if (hasBytes3)
          {
            positionAfterIfBlockD22 = positionAfterelseCount1 + 2ULL;
          }
          else
          {
            positionAfterIfBlockD22 =
              EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                positionAfterelseCount1);
          }
          uint64_t positionAfterthenLen;
          if (EverParseIsSuccess(positionAfterIfBlockD22))
          {
            positionAfterthenLen = positionAfterIfBlockD22;
          }
          else
          {
            ErrorHandlerFn("_IfBlockD2",
              "then_len",
              EverParseErrorReasonOfResult(positionAfterIfBlockD22),
              EverParseGetValidatorErrorKind(positionAfterIfBlockD22),
              Ctxt,
              Input,
              positionAfterelseCount1);
            positionAfterthenLen = positionAfterIfBlockD22;
          }
          if (EverParseIsError(positionAfterthenLen))
          {
            positionAfterIfBlockD21 = positionAfterthenLen;
          }
          else
          {
            uint16_t r0 = Load16Le(Input + (uint32_t)positionAfterelseCount1);
            uint16_t thenLen = (uint16_t)(uint32_t)r0;
            /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
            BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterthenLen);
            uint64_t positionAfterIfBlockD23;
            if (hasBytes)
            {
              positionAfterIfBlockD23 = positionAfterthenLen + 2ULL;
            }
            else
            {
              positionAfterIfBlockD23 =
                EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                  positionAfterthenLen);
            }
            uint64_t positionAfterelseLen;
            if (EverParseIsSuccess(positionAfterIfBlockD23))
            {
              positionAfterelseLen = positionAfterIfBlockD23;
            }
            else
            {
              ErrorHandlerFn("_IfBlockD2",
                "else_len",
                EverParseErrorReasonOfResult(positionAfterIfBlockD23),
                EverParseGetValidatorErrorKind(positionAfterIfBlockD23),
                Ctxt,
                Input,
                positionAfterthenLen);
              positionAfterelseLen = positionAfterIfBlockD23;
            }
            if (EverParseIsError(positionAfterelseLen))
            {
              positionAfterIfBlockD21 = positionAfterelseLen;
            }
            else
            {
              uint16_t r = Load16Le(Input + (uint32_t)positionAfterthenLen);
              uint16_t elseLen = (uint16_t)(uint32_t)r;
              /* Validating field then_body */
              BOOLEAN
              hasEnoughBytes0 = (uint64_t)(uint32_t)thenLen <= (InputLength - positionAfterelseLen);
              uint64_t positionAfterIfBlockD24;
              if (!hasEnoughBytes0)
              {
                positionAfterIfBlockD24 =
                  EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                    positionAfterelseLen);
              }
              else
              {
                uint8_t *truncatedInput = Input;
                uint64_t truncatedInputLength = positionAfterelseLen + (uint64_t)(uint32_t)thenLen;
                uint64_t result = positionAfterelseLen;
                while (TRUE)
                {
                  uint64_t position = result;
                  BOOLEAN ite;
                  if (!(1ULL <= (truncatedInputLength - position)))
                  {
                    ite = TRUE;
                  }
                  else
                  {
                    uint64_t
                    positionAfterIfBlockD25 =
                      ValidateStmtD3(Ctxt,
                        ErrorHandlerFn,
                        truncatedInput,
                        truncatedInputLength,
                        position);
                    uint64_t result1;
                    if (EverParseIsSuccess(positionAfterIfBlockD25))
                    {
                      result1 = positionAfterIfBlockD25;
                    }
                    else
                    {
                      ErrorHandlerFn("_IfBlockD2",
                        "then_body.element",
                        EverParseErrorReasonOfResult(positionAfterIfBlockD25),
                        EverParseGetValidatorErrorKind(positionAfterIfBlockD25),
                        Ctxt,
                        truncatedInput,
                        position);
                      result1 = positionAfterIfBlockD25;
                    }
                    result = result1;
                    ite = EverParseIsError(result1);
                  }
                  if (ite)
                  {
                    break;
                  }
                }
                uint64_t res0 = result;
                positionAfterIfBlockD24 = res0;
              }
              uint64_t positionAfterthenBody;
              if (EverParseIsSuccess(positionAfterIfBlockD24))
              {
                positionAfterthenBody = positionAfterIfBlockD24;
              }
              else
              {
                ErrorHandlerFn("_IfBlockD2",
                  "then_body",
                  EverParseErrorReasonOfResult(positionAfterIfBlockD24),
                  EverParseGetValidatorErrorKind(positionAfterIfBlockD24),
                  Ctxt,
                  Input,
                  positionAfterelseLen);
                positionAfterthenBody = positionAfterIfBlockD24;
              }
              if (EverParseIsError(positionAfterthenBody))
              {
                positionAfterIfBlockD21 = positionAfterthenBody;
              }
              else
              {
                /* Validating field else_body */
                BOOLEAN
                hasEnoughBytes =
                  (uint64_t)(uint32_t)elseLen <= (InputLength - positionAfterthenBody);
                uint64_t positionAfterIfBlockD25;
                if (!hasEnoughBytes)
                {
                  positionAfterIfBlockD25 =
                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                      positionAfterthenBody);
                }
                else
                {
                  uint8_t *truncatedInput = Input;
                  uint64_t
                  truncatedInputLength = positionAfterthenBody + (uint64_t)(uint32_t)elseLen;
                  uint64_t result = positionAfterthenBody;
                  while (TRUE)
                  {
                    uint64_t position = result;
                    BOOLEAN ite;
                    if (!(1ULL <= (truncatedInputLength - position)))
                    {
                      ite = TRUE;
                    }
                    else
                    {
                      uint64_t
                      positionAfterIfBlockD26 =
                        ValidateStmtD3(Ctxt,
                          ErrorHandlerFn,
                          truncatedInput,
                          truncatedInputLength,
                          position);
                      uint64_t result1;
                      if (EverParseIsSuccess(positionAfterIfBlockD26))
                      {
                        result1 = positionAfterIfBlockD26;
                      }
                      else
                      {
                        ErrorHandlerFn("_IfBlockD2",
                          "else_body.element",
                          EverParseErrorReasonOfResult(positionAfterIfBlockD26),
                          EverParseGetValidatorErrorKind(positionAfterIfBlockD26),
                          Ctxt,
                          truncatedInput,
                          position);
                        result1 = positionAfterIfBlockD26;
                      }
                      result = result1;
                      ite = EverParseIsError(result1);
                    }
                    if (ite)
                    {
                      break;
                    }
                  }
                  uint64_t res0 = result;
                  positionAfterIfBlockD25 = res0;
                }
                if (EverParseIsSuccess(positionAfterIfBlockD25))
                {
                  positionAfterIfBlockD21 = positionAfterIfBlockD25;
                }
                else
                {
                  ErrorHandlerFn("_IfBlockD2",
                    "else_body",
                    EverParseErrorReasonOfResult(positionAfterIfBlockD25),
                    EverParseGetValidatorErrorKind(positionAfterIfBlockD25),
                    Ctxt,
                    Input,
                    positionAfterthenBody);
                  positionAfterIfBlockD21 = positionAfterIfBlockD25;
                }
              }
            }
          }
        }
      }
      if (EverParseIsSuccess(positionAfterIfBlockD21))
      {
        positionAfterIfBlockD20 = positionAfterIfBlockD21;
      }
      else
      {
        ErrorHandlerFn("_IfBlockD2",
          "else_count",
          EverParseErrorReasonOfResult(positionAfterIfBlockD21),
          EverParseGetValidatorErrorKind(positionAfterIfBlockD21),
          Ctxt,
          Input,
          positionAfterthenCount1);
        positionAfterIfBlockD20 = positionAfterIfBlockD21;
      }
    }
  }
  if (EverParseIsSuccess(positionAfterIfBlockD20))
  {
    return positionAfterIfBlockD20;
  }
  ErrorHandlerFn("_IfBlockD2",
    "then_count",
    EverParseErrorReasonOfResult(positionAfterIfBlockD20),
    EverParseGetValidatorErrorKind(positionAfterIfBlockD20),
    Ctxt,
    Input,
    positionAftercond);
  return positionAfterIfBlockD20;
}

static inline uint64_t
ValidateForBlockD2(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field var_idx */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterForBlockD2;
  if (hasBytes0)
  {
    positionAfterForBlockD2 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterForBlockD2 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res0;
  if (EverParseIsSuccess(positionAfterForBlockD2))
  {
    res0 = positionAfterForBlockD2;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD2",
      "var_idx",
      EverParseErrorReasonOfResult(positionAfterForBlockD2),
      EverParseGetValidatorErrorKind(positionAfterForBlockD2),
      Ctxt,
      Input,
      StartPosition);
    res0 = positionAfterForBlockD2;
  }
  uint64_t positionAftervarIdx = res0;
  if (EverParseIsError(positionAftervarIdx))
  {
    return positionAftervarIdx;
  }
  /* Validating field from_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes1 = 2ULL <= (InputLength - positionAftervarIdx);
  uint64_t positionAfterForBlockD20;
  if (hasBytes1)
  {
    positionAfterForBlockD20 = positionAftervarIdx + 2ULL;
  }
  else
  {
    positionAfterForBlockD20 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftervarIdx);
  }
  uint64_t res1;
  if (EverParseIsSuccess(positionAfterForBlockD20))
  {
    res1 = positionAfterForBlockD20;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD2",
      "from_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD20),
      EverParseGetValidatorErrorKind(positionAfterForBlockD20),
      Ctxt,
      Input,
      positionAftervarIdx);
    res1 = positionAfterForBlockD20;
  }
  uint64_t positionAfterfromVal = res1;
  if (EverParseIsError(positionAfterfromVal))
  {
    return positionAfterfromVal;
  }
  /* Validating field to_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes2 = 2ULL <= (InputLength - positionAfterfromVal);
  uint64_t positionAfterForBlockD21;
  if (hasBytes2)
  {
    positionAfterForBlockD21 = positionAfterfromVal + 2ULL;
  }
  else
  {
    positionAfterForBlockD21 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterfromVal);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterForBlockD21))
  {
    res = positionAfterForBlockD21;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD2",
      "to_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD21),
      EverParseGetValidatorErrorKind(positionAfterForBlockD21),
      Ctxt,
      Input,
      positionAfterfromVal);
    res = positionAfterForBlockD21;
  }
  uint64_t positionAftertoVal = res;
  if (EverParseIsError(positionAftertoVal))
  {
    return positionAftertoVal;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes3 = 1ULL <= (InputLength - positionAftertoVal);
  uint64_t positionAfterbodyCount;
  if (hasBytes3)
  {
    positionAfterbodyCount = positionAftertoVal + 1ULL;
  }
  else
  {
    positionAfterbodyCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertoVal);
  }
  uint64_t positionAfterForBlockD22;
  if (EverParseIsError(positionAfterbodyCount))
  {
    positionAfterForBlockD22 = positionAfterbodyCount;
  }
  else
  {
    uint8_t bodyCount = Input[(uint32_t)positionAftertoVal];
    BOOLEAN bodyCountConstraintIsOk = bodyCount <= 64U;
    uint64_t
    positionAfterbodyCount1 =
      EverParseCheckConstraintOk(bodyCountConstraintIsOk,
        positionAfterbodyCount);
    if (EverParseIsError(positionAfterbodyCount1))
    {
      positionAfterForBlockD22 = positionAfterbodyCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
      BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterbodyCount1);
      uint64_t positionAfterForBlockD23;
      if (hasBytes)
      {
        positionAfterForBlockD23 = positionAfterbodyCount1 + 2ULL;
      }
      else
      {
        positionAfterForBlockD23 =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterbodyCount1);
      }
      uint64_t positionAfterbodyLen;
      if (EverParseIsSuccess(positionAfterForBlockD23))
      {
        positionAfterbodyLen = positionAfterForBlockD23;
      }
      else
      {
        ErrorHandlerFn("_ForBlockD2",
          "body_len",
          EverParseErrorReasonOfResult(positionAfterForBlockD23),
          EverParseGetValidatorErrorKind(positionAfterForBlockD23),
          Ctxt,
          Input,
          positionAfterbodyCount1);
        positionAfterbodyLen = positionAfterForBlockD23;
      }
      if (EverParseIsError(positionAfterbodyLen))
      {
        positionAfterForBlockD22 = positionAfterbodyLen;
      }
      else
      {
        uint16_t r = Load16Le(Input + (uint32_t)positionAfterbodyCount1);
        uint16_t bodyLen = (uint16_t)(uint32_t)r;
        /* Validating field body */
        BOOLEAN
        hasEnoughBytes = (uint64_t)(uint32_t)bodyLen <= (InputLength - positionAfterbodyLen);
        uint64_t positionAfterForBlockD24;
        if (!hasEnoughBytes)
        {
          positionAfterForBlockD24 =
            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
              positionAfterbodyLen);
        }
        else
        {
          uint8_t *truncatedInput = Input;
          uint64_t truncatedInputLength = positionAfterbodyLen + (uint64_t)(uint32_t)bodyLen;
          uint64_t result = positionAfterbodyLen;
          while (TRUE)
          {
            uint64_t position = result;
            BOOLEAN ite;
            if (!(1ULL <= (truncatedInputLength - position)))
            {
              ite = TRUE;
            }
            else
            {
              uint64_t
              positionAfterForBlockD25 =
                ValidateStmtD3(Ctxt,
                  ErrorHandlerFn,
                  truncatedInput,
                  truncatedInputLength,
                  position);
              uint64_t result1;
              if (EverParseIsSuccess(positionAfterForBlockD25))
              {
                result1 = positionAfterForBlockD25;
              }
              else
              {
                ErrorHandlerFn("_ForBlockD2",
                  "body.element",
                  EverParseErrorReasonOfResult(positionAfterForBlockD25),
                  EverParseGetValidatorErrorKind(positionAfterForBlockD25),
                  Ctxt,
                  truncatedInput,
                  position);
                result1 = positionAfterForBlockD25;
              }
              result = result1;
              ite = EverParseIsError(result1);
            }
            if (ite)
            {
              break;
            }
          }
          uint64_t res2 = result;
          positionAfterForBlockD24 = res2;
        }
        if (EverParseIsSuccess(positionAfterForBlockD24))
        {
          positionAfterForBlockD22 = positionAfterForBlockD24;
        }
        else
        {
          ErrorHandlerFn("_ForBlockD2",
            "body",
            EverParseErrorReasonOfResult(positionAfterForBlockD24),
            EverParseGetValidatorErrorKind(positionAfterForBlockD24),
            Ctxt,
            Input,
            positionAfterbodyLen);
          positionAfterForBlockD22 = positionAfterForBlockD24;
        }
      }
    }
  }
  if (EverParseIsSuccess(positionAfterForBlockD22))
  {
    return positionAfterForBlockD22;
  }
  ErrorHandlerFn("_ForBlockD2",
    "body_count",
    EverParseErrorReasonOfResult(positionAfterForBlockD22),
    EverParseGetValidatorErrorKind(positionAfterForBlockD22),
    Ctxt,
    Input,
    positionAftertoVal);
  return positionAfterForBlockD22;
}

static inline uint64_t
ValidateStmtD2payload(
  uint8_t Tag,
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLen,
  uint64_t StartPosition
)
{
  if (Tag == 1U)
  {
    /* Validating field load_true */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD2payload;
    if (hasBytes)
    {
      positionAfterStmtD2payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD2payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "load_true",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 2U)
  {
    /* Validating field load_false */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD2payload;
    if (hasBytes)
    {
      positionAfterStmtD2payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD2payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "load_false",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 3U)
  {
    /* Validating field load_int */
    uint64_t positionAfterStmtD2payload = ValidateLoadInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "load_int",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 16U)
  {
    /* Validating field bool_not */
    uint64_t positionAfterStmtD2payload = ValidateBoolNot(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "bool_not",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 17U)
  {
    /* Validating field bool_and */
    uint64_t positionAfterStmtD2payload = ValidateBoolAnd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "bool_and",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 18U)
  {
    /* Validating field bool_or */
    uint64_t positionAfterStmtD2payload = ValidateBoolOr(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "bool_or",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 19U)
  {
    /* Validating field bool_xor */
    uint64_t positionAfterStmtD2payload = ValidateBoolXor(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "bool_xor",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 20U)
  {
    /* Validating field copy_bool */
    uint64_t positionAfterStmtD2payload = ValidateCopyBool(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "copy_bool",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 32U)
  {
    /* Validating field int_add */
    uint64_t positionAfterStmtD2payload = ValidateIntAdd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "int_add",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 33U)
  {
    /* Validating field int_sub */
    uint64_t positionAfterStmtD2payload = ValidateIntSub(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "int_sub",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 34U)
  {
    /* Validating field int_mul */
    uint64_t positionAfterStmtD2payload = ValidateIntMul(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "int_mul",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 35U)
  {
    /* Validating field copy_int */
    uint64_t positionAfterStmtD2payload = ValidateCopyInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "copy_int",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 48U)
  {
    /* Validating field cmp_eq */
    uint64_t positionAfterStmtD2payload = ValidateCmpEq(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "cmp_eq",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 49U)
  {
    /* Validating field cmp_ne */
    uint64_t positionAfterStmtD2payload = ValidateCmpNe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "cmp_ne",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 50U)
  {
    /* Validating field cmp_lt */
    uint64_t positionAfterStmtD2payload = ValidateCmpLt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "cmp_lt",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 51U)
  {
    /* Validating field cmp_gt */
    uint64_t positionAfterStmtD2payload = ValidateCmpGt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "cmp_gt",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 52U)
  {
    /* Validating field cmp_le */
    uint64_t positionAfterStmtD2payload = ValidateCmpLe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "cmp_le",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 53U)
  {
    /* Validating field cmp_ge */
    uint64_t positionAfterStmtD2payload = ValidateCmpGe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "cmp_ge",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 64U)
  {
    /* Validating field if_block */
    uint64_t
    positionAfterStmtD2payload =
      ValidateIfBlockD2(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "if_block",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 65U)
  {
    /* Validating field for_block */
    uint64_t
    positionAfterStmtD2payload =
      ValidateForBlockD2(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "for_block",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 80U)
  {
    /* Validating field ton_call */
    uint64_t positionAfterStmtD2payload = ValidateTonCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "ton_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 81U)
  {
    /* Validating field tof_call */
    uint64_t positionAfterStmtD2payload = ValidateTofCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "tof_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 82U)
  {
    /* Validating field tp_call */
    uint64_t positionAfterStmtD2payload = ValidateTpCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "tp_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 96U)
  {
    /* Validating field ctu_call */
    uint64_t positionAfterStmtD2payload = ValidateCtuCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "ctu_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 97U)
  {
    /* Validating field ctd_call */
    uint64_t positionAfterStmtD2payload = ValidateCtdCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "ctd_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 98U)
  {
    /* Validating field ctud_call */
    uint64_t positionAfterStmtD2payload = ValidateCtudCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "ctud_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 112U)
  {
    /* Validating field sr_call */
    uint64_t positionAfterStmtD2payload = ValidateSrCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "sr_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 113U)
  {
    /* Validating field rs_call */
    uint64_t positionAfterStmtD2payload = ValidateRsCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "rs_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 114U)
  {
    /* Validating field rtrig_call */
    uint64_t positionAfterStmtD2payload = ValidateRtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "rtrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  if (Tag == 115U)
  {
    /* Validating field ftrig_call */
    uint64_t positionAfterStmtD2payload = ValidateFtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD2payload))
    {
      return positionAfterStmtD2payload;
    }
    ErrorHandlerFn("_StmtD2Payload",
      "ftrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD2payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD2payload;
  }
  uint64_t
  positionAfterStmtD2payload =
    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_IMPOSSIBLE,
      StartPosition);
  if (EverParseIsSuccess(positionAfterStmtD2payload))
  {
    return positionAfterStmtD2payload;
  }
  ErrorHandlerFn("_StmtD2Payload",
    "_x_122",
    EverParseErrorReasonOfResult(positionAfterStmtD2payload),
    EverParseGetValidatorErrorKind(positionAfterStmtD2payload),
    Ctxt,
    Input,
    StartPosition);
  return positionAfterStmtD2payload;
}

static inline uint64_t
ValidateStmtD2(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterStmtD2;
  if (hasBytes0)
  {
    positionAfterStmtD2 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterStmtD2 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t positionAftertag;
  if (EverParseIsSuccess(positionAfterStmtD2))
  {
    positionAftertag = positionAfterStmtD2;
  }
  else
  {
    ErrorHandlerFn("_StmtD2",
      "tag",
      EverParseErrorReasonOfResult(positionAfterStmtD2),
      EverParseGetValidatorErrorKind(positionAfterStmtD2),
      Ctxt,
      Input,
      StartPosition);
    positionAftertag = positionAfterStmtD2;
  }
  if (EverParseIsError(positionAftertag))
  {
    return positionAftertag;
  }
  uint8_t tag = Input[(uint32_t)StartPosition];
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes = 2ULL <= (InputLength - positionAftertag);
  uint64_t positionAfterStmtD20;
  if (hasBytes)
  {
    positionAfterStmtD20 = positionAftertag + 2ULL;
  }
  else
  {
    positionAfterStmtD20 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertag);
  }
  uint64_t positionAfterlen;
  if (EverParseIsSuccess(positionAfterStmtD20))
  {
    positionAfterlen = positionAfterStmtD20;
  }
  else
  {
    ErrorHandlerFn("_StmtD2",
      "len",
      EverParseErrorReasonOfResult(positionAfterStmtD20),
      EverParseGetValidatorErrorKind(positionAfterStmtD20),
      Ctxt,
      Input,
      positionAftertag);
    positionAfterlen = positionAfterStmtD20;
  }
  if (EverParseIsError(positionAfterlen))
  {
    return positionAfterlen;
  }
  uint16_t r = Load16Le(Input + (uint32_t)positionAftertag);
  uint16_t len = (uint16_t)(uint32_t)r;
  /* Validating field payload */
  BOOLEAN hasEnoughBytes = (uint64_t)(uint32_t)len <= (InputLength - positionAfterlen);
  uint64_t positionAfterStmtD21;
  if (!hasEnoughBytes)
  {
    positionAfterStmtD21 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterlen);
  }
  else
  {
    uint8_t *truncatedInput = Input;
    uint64_t truncatedInputLength = positionAfterlen + (uint64_t)(uint32_t)len;
    uint64_t result = positionAfterlen;
    while (TRUE)
    {
      uint64_t position = result;
      BOOLEAN ite;
      if (!(1ULL <= (truncatedInputLength - position)))
      {
        ite = TRUE;
      }
      else
      {
        uint64_t
        positionAfterStmtD22 =
          ValidateStmtD2payload(tag,
            Ctxt,
            ErrorHandlerFn,
            truncatedInput,
            truncatedInputLength,
            position);
        uint64_t result1;
        if (EverParseIsSuccess(positionAfterStmtD22))
        {
          result1 = positionAfterStmtD22;
        }
        else
        {
          ErrorHandlerFn("_StmtD2",
            "payload.element",
            EverParseErrorReasonOfResult(positionAfterStmtD22),
            EverParseGetValidatorErrorKind(positionAfterStmtD22),
            Ctxt,
            truncatedInput,
            position);
          result1 = positionAfterStmtD22;
        }
        result = result1;
        ite = EverParseIsError(result1);
      }
      if (ite)
      {
        break;
      }
    }
    uint64_t res = result;
    positionAfterStmtD21 = res;
  }
  if (EverParseIsSuccess(positionAfterStmtD21))
  {
    return positionAfterStmtD21;
  }
  ErrorHandlerFn("_StmtD2",
    "payload",
    EverParseErrorReasonOfResult(positionAfterStmtD21),
    EverParseGetValidatorErrorKind(positionAfterStmtD21),
    Ctxt,
    Input,
    positionAfterlen);
  return positionAfterStmtD21;
}

static inline uint64_t
ValidateIfBlockD1(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field cond */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterIfBlockD1;
  if (hasBytes0)
  {
    positionAfterIfBlockD1 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterIfBlockD1 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterIfBlockD1))
  {
    res = positionAfterIfBlockD1;
  }
  else
  {
    ErrorHandlerFn("_IfBlockD1",
      "cond",
      EverParseErrorReasonOfResult(positionAfterIfBlockD1),
      EverParseGetValidatorErrorKind(positionAfterIfBlockD1),
      Ctxt,
      Input,
      StartPosition);
    res = positionAfterIfBlockD1;
  }
  uint64_t positionAftercond = res;
  if (EverParseIsError(positionAftercond))
  {
    return positionAftercond;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes1 = 1ULL <= (InputLength - positionAftercond);
  uint64_t positionAfterthenCount;
  if (hasBytes1)
  {
    positionAfterthenCount = positionAftercond + 1ULL;
  }
  else
  {
    positionAfterthenCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftercond);
  }
  uint64_t positionAfterIfBlockD10;
  if (EverParseIsError(positionAfterthenCount))
  {
    positionAfterIfBlockD10 = positionAfterthenCount;
  }
  else
  {
    uint8_t thenCount = Input[(uint32_t)positionAftercond];
    BOOLEAN thenCountConstraintIsOk = thenCount <= 64U;
    uint64_t
    positionAfterthenCount1 =
      EverParseCheckConstraintOk(thenCountConstraintIsOk,
        positionAfterthenCount);
    if (EverParseIsError(positionAfterthenCount1))
    {
      positionAfterIfBlockD10 = positionAfterthenCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT8, i.e., 1 byte */
      BOOLEAN hasBytes2 = 1ULL <= (InputLength - positionAfterthenCount1);
      uint64_t positionAfterelseCount;
      if (hasBytes2)
      {
        positionAfterelseCount = positionAfterthenCount1 + 1ULL;
      }
      else
      {
        positionAfterelseCount =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterthenCount1);
      }
      uint64_t positionAfterIfBlockD11;
      if (EverParseIsError(positionAfterelseCount))
      {
        positionAfterIfBlockD11 = positionAfterelseCount;
      }
      else
      {
        uint8_t elseCount = Input[(uint32_t)positionAfterthenCount1];
        BOOLEAN elseCountConstraintIsOk = elseCount <= 64U;
        uint64_t
        positionAfterelseCount1 =
          EverParseCheckConstraintOk(elseCountConstraintIsOk,
            positionAfterelseCount);
        if (EverParseIsError(positionAfterelseCount1))
        {
          positionAfterIfBlockD11 = positionAfterelseCount1;
        }
        else
        {
          /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
          BOOLEAN hasBytes3 = 2ULL <= (InputLength - positionAfterelseCount1);
          uint64_t positionAfterIfBlockD12;
          if (hasBytes3)
          {
            positionAfterIfBlockD12 = positionAfterelseCount1 + 2ULL;
          }
          else
          {
            positionAfterIfBlockD12 =
              EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                positionAfterelseCount1);
          }
          uint64_t positionAfterthenLen;
          if (EverParseIsSuccess(positionAfterIfBlockD12))
          {
            positionAfterthenLen = positionAfterIfBlockD12;
          }
          else
          {
            ErrorHandlerFn("_IfBlockD1",
              "then_len",
              EverParseErrorReasonOfResult(positionAfterIfBlockD12),
              EverParseGetValidatorErrorKind(positionAfterIfBlockD12),
              Ctxt,
              Input,
              positionAfterelseCount1);
            positionAfterthenLen = positionAfterIfBlockD12;
          }
          if (EverParseIsError(positionAfterthenLen))
          {
            positionAfterIfBlockD11 = positionAfterthenLen;
          }
          else
          {
            uint16_t r0 = Load16Le(Input + (uint32_t)positionAfterelseCount1);
            uint16_t thenLen = (uint16_t)(uint32_t)r0;
            /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
            BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterthenLen);
            uint64_t positionAfterIfBlockD13;
            if (hasBytes)
            {
              positionAfterIfBlockD13 = positionAfterthenLen + 2ULL;
            }
            else
            {
              positionAfterIfBlockD13 =
                EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                  positionAfterthenLen);
            }
            uint64_t positionAfterelseLen;
            if (EverParseIsSuccess(positionAfterIfBlockD13))
            {
              positionAfterelseLen = positionAfterIfBlockD13;
            }
            else
            {
              ErrorHandlerFn("_IfBlockD1",
                "else_len",
                EverParseErrorReasonOfResult(positionAfterIfBlockD13),
                EverParseGetValidatorErrorKind(positionAfterIfBlockD13),
                Ctxt,
                Input,
                positionAfterthenLen);
              positionAfterelseLen = positionAfterIfBlockD13;
            }
            if (EverParseIsError(positionAfterelseLen))
            {
              positionAfterIfBlockD11 = positionAfterelseLen;
            }
            else
            {
              uint16_t r = Load16Le(Input + (uint32_t)positionAfterthenLen);
              uint16_t elseLen = (uint16_t)(uint32_t)r;
              /* Validating field then_body */
              BOOLEAN
              hasEnoughBytes0 = (uint64_t)(uint32_t)thenLen <= (InputLength - positionAfterelseLen);
              uint64_t positionAfterIfBlockD14;
              if (!hasEnoughBytes0)
              {
                positionAfterIfBlockD14 =
                  EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                    positionAfterelseLen);
              }
              else
              {
                uint8_t *truncatedInput = Input;
                uint64_t truncatedInputLength = positionAfterelseLen + (uint64_t)(uint32_t)thenLen;
                uint64_t result = positionAfterelseLen;
                while (TRUE)
                {
                  uint64_t position = result;
                  BOOLEAN ite;
                  if (!(1ULL <= (truncatedInputLength - position)))
                  {
                    ite = TRUE;
                  }
                  else
                  {
                    uint64_t
                    positionAfterIfBlockD15 =
                      ValidateStmtD2(Ctxt,
                        ErrorHandlerFn,
                        truncatedInput,
                        truncatedInputLength,
                        position);
                    uint64_t result1;
                    if (EverParseIsSuccess(positionAfterIfBlockD15))
                    {
                      result1 = positionAfterIfBlockD15;
                    }
                    else
                    {
                      ErrorHandlerFn("_IfBlockD1",
                        "then_body.element",
                        EverParseErrorReasonOfResult(positionAfterIfBlockD15),
                        EverParseGetValidatorErrorKind(positionAfterIfBlockD15),
                        Ctxt,
                        truncatedInput,
                        position);
                      result1 = positionAfterIfBlockD15;
                    }
                    result = result1;
                    ite = EverParseIsError(result1);
                  }
                  if (ite)
                  {
                    break;
                  }
                }
                uint64_t res0 = result;
                positionAfterIfBlockD14 = res0;
              }
              uint64_t positionAfterthenBody;
              if (EverParseIsSuccess(positionAfterIfBlockD14))
              {
                positionAfterthenBody = positionAfterIfBlockD14;
              }
              else
              {
                ErrorHandlerFn("_IfBlockD1",
                  "then_body",
                  EverParseErrorReasonOfResult(positionAfterIfBlockD14),
                  EverParseGetValidatorErrorKind(positionAfterIfBlockD14),
                  Ctxt,
                  Input,
                  positionAfterelseLen);
                positionAfterthenBody = positionAfterIfBlockD14;
              }
              if (EverParseIsError(positionAfterthenBody))
              {
                positionAfterIfBlockD11 = positionAfterthenBody;
              }
              else
              {
                /* Validating field else_body */
                BOOLEAN
                hasEnoughBytes =
                  (uint64_t)(uint32_t)elseLen <= (InputLength - positionAfterthenBody);
                uint64_t positionAfterIfBlockD15;
                if (!hasEnoughBytes)
                {
                  positionAfterIfBlockD15 =
                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                      positionAfterthenBody);
                }
                else
                {
                  uint8_t *truncatedInput = Input;
                  uint64_t
                  truncatedInputLength = positionAfterthenBody + (uint64_t)(uint32_t)elseLen;
                  uint64_t result = positionAfterthenBody;
                  while (TRUE)
                  {
                    uint64_t position = result;
                    BOOLEAN ite;
                    if (!(1ULL <= (truncatedInputLength - position)))
                    {
                      ite = TRUE;
                    }
                    else
                    {
                      uint64_t
                      positionAfterIfBlockD16 =
                        ValidateStmtD2(Ctxt,
                          ErrorHandlerFn,
                          truncatedInput,
                          truncatedInputLength,
                          position);
                      uint64_t result1;
                      if (EverParseIsSuccess(positionAfterIfBlockD16))
                      {
                        result1 = positionAfterIfBlockD16;
                      }
                      else
                      {
                        ErrorHandlerFn("_IfBlockD1",
                          "else_body.element",
                          EverParseErrorReasonOfResult(positionAfterIfBlockD16),
                          EverParseGetValidatorErrorKind(positionAfterIfBlockD16),
                          Ctxt,
                          truncatedInput,
                          position);
                        result1 = positionAfterIfBlockD16;
                      }
                      result = result1;
                      ite = EverParseIsError(result1);
                    }
                    if (ite)
                    {
                      break;
                    }
                  }
                  uint64_t res0 = result;
                  positionAfterIfBlockD15 = res0;
                }
                if (EverParseIsSuccess(positionAfterIfBlockD15))
                {
                  positionAfterIfBlockD11 = positionAfterIfBlockD15;
                }
                else
                {
                  ErrorHandlerFn("_IfBlockD1",
                    "else_body",
                    EverParseErrorReasonOfResult(positionAfterIfBlockD15),
                    EverParseGetValidatorErrorKind(positionAfterIfBlockD15),
                    Ctxt,
                    Input,
                    positionAfterthenBody);
                  positionAfterIfBlockD11 = positionAfterIfBlockD15;
                }
              }
            }
          }
        }
      }
      if (EverParseIsSuccess(positionAfterIfBlockD11))
      {
        positionAfterIfBlockD10 = positionAfterIfBlockD11;
      }
      else
      {
        ErrorHandlerFn("_IfBlockD1",
          "else_count",
          EverParseErrorReasonOfResult(positionAfterIfBlockD11),
          EverParseGetValidatorErrorKind(positionAfterIfBlockD11),
          Ctxt,
          Input,
          positionAfterthenCount1);
        positionAfterIfBlockD10 = positionAfterIfBlockD11;
      }
    }
  }
  if (EverParseIsSuccess(positionAfterIfBlockD10))
  {
    return positionAfterIfBlockD10;
  }
  ErrorHandlerFn("_IfBlockD1",
    "then_count",
    EverParseErrorReasonOfResult(positionAfterIfBlockD10),
    EverParseGetValidatorErrorKind(positionAfterIfBlockD10),
    Ctxt,
    Input,
    positionAftercond);
  return positionAfterIfBlockD10;
}

static inline uint64_t
ValidateForBlockD1(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Validating field var_idx */
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterForBlockD1;
  if (hasBytes0)
  {
    positionAfterForBlockD1 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterForBlockD1 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t res0;
  if (EverParseIsSuccess(positionAfterForBlockD1))
  {
    res0 = positionAfterForBlockD1;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD1",
      "var_idx",
      EverParseErrorReasonOfResult(positionAfterForBlockD1),
      EverParseGetValidatorErrorKind(positionAfterForBlockD1),
      Ctxt,
      Input,
      StartPosition);
    res0 = positionAfterForBlockD1;
  }
  uint64_t positionAftervarIdx = res0;
  if (EverParseIsError(positionAftervarIdx))
  {
    return positionAftervarIdx;
  }
  /* Validating field from_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes1 = 2ULL <= (InputLength - positionAftervarIdx);
  uint64_t positionAfterForBlockD10;
  if (hasBytes1)
  {
    positionAfterForBlockD10 = positionAftervarIdx + 2ULL;
  }
  else
  {
    positionAfterForBlockD10 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftervarIdx);
  }
  uint64_t res1;
  if (EverParseIsSuccess(positionAfterForBlockD10))
  {
    res1 = positionAfterForBlockD10;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD1",
      "from_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD10),
      EverParseGetValidatorErrorKind(positionAfterForBlockD10),
      Ctxt,
      Input,
      positionAftervarIdx);
    res1 = positionAfterForBlockD10;
  }
  uint64_t positionAfterfromVal = res1;
  if (EverParseIsError(positionAfterfromVal))
  {
    return positionAfterfromVal;
  }
  /* Validating field to_val */
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes2 = 2ULL <= (InputLength - positionAfterfromVal);
  uint64_t positionAfterForBlockD11;
  if (hasBytes2)
  {
    positionAfterForBlockD11 = positionAfterfromVal + 2ULL;
  }
  else
  {
    positionAfterForBlockD11 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterfromVal);
  }
  uint64_t res;
  if (EverParseIsSuccess(positionAfterForBlockD11))
  {
    res = positionAfterForBlockD11;
  }
  else
  {
    ErrorHandlerFn("_ForBlockD1",
      "to_val",
      EverParseErrorReasonOfResult(positionAfterForBlockD11),
      EverParseGetValidatorErrorKind(positionAfterForBlockD11),
      Ctxt,
      Input,
      positionAfterfromVal);
    res = positionAfterForBlockD11;
  }
  uint64_t positionAftertoVal = res;
  if (EverParseIsError(positionAftertoVal))
  {
    return positionAftertoVal;
  }
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes3 = 1ULL <= (InputLength - positionAftertoVal);
  uint64_t positionAfterbodyCount;
  if (hasBytes3)
  {
    positionAfterbodyCount = positionAftertoVal + 1ULL;
  }
  else
  {
    positionAfterbodyCount =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertoVal);
  }
  uint64_t positionAfterForBlockD12;
  if (EverParseIsError(positionAfterbodyCount))
  {
    positionAfterForBlockD12 = positionAfterbodyCount;
  }
  else
  {
    uint8_t bodyCount = Input[(uint32_t)positionAftertoVal];
    BOOLEAN bodyCountConstraintIsOk = bodyCount <= 64U;
    uint64_t
    positionAfterbodyCount1 =
      EverParseCheckConstraintOk(bodyCountConstraintIsOk,
        positionAfterbodyCount);
    if (EverParseIsError(positionAfterbodyCount1))
    {
      positionAfterForBlockD12 = positionAfterbodyCount1;
    }
    else
    {
      /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
      BOOLEAN hasBytes = 2ULL <= (InputLength - positionAfterbodyCount1);
      uint64_t positionAfterForBlockD13;
      if (hasBytes)
      {
        positionAfterForBlockD13 = positionAfterbodyCount1 + 2ULL;
      }
      else
      {
        positionAfterForBlockD13 =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAfterbodyCount1);
      }
      uint64_t positionAfterbodyLen;
      if (EverParseIsSuccess(positionAfterForBlockD13))
      {
        positionAfterbodyLen = positionAfterForBlockD13;
      }
      else
      {
        ErrorHandlerFn("_ForBlockD1",
          "body_len",
          EverParseErrorReasonOfResult(positionAfterForBlockD13),
          EverParseGetValidatorErrorKind(positionAfterForBlockD13),
          Ctxt,
          Input,
          positionAfterbodyCount1);
        positionAfterbodyLen = positionAfterForBlockD13;
      }
      if (EverParseIsError(positionAfterbodyLen))
      {
        positionAfterForBlockD12 = positionAfterbodyLen;
      }
      else
      {
        uint16_t r = Load16Le(Input + (uint32_t)positionAfterbodyCount1);
        uint16_t bodyLen = (uint16_t)(uint32_t)r;
        /* Validating field body */
        BOOLEAN
        hasEnoughBytes = (uint64_t)(uint32_t)bodyLen <= (InputLength - positionAfterbodyLen);
        uint64_t positionAfterForBlockD14;
        if (!hasEnoughBytes)
        {
          positionAfterForBlockD14 =
            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
              positionAfterbodyLen);
        }
        else
        {
          uint8_t *truncatedInput = Input;
          uint64_t truncatedInputLength = positionAfterbodyLen + (uint64_t)(uint32_t)bodyLen;
          uint64_t result = positionAfterbodyLen;
          while (TRUE)
          {
            uint64_t position = result;
            BOOLEAN ite;
            if (!(1ULL <= (truncatedInputLength - position)))
            {
              ite = TRUE;
            }
            else
            {
              uint64_t
              positionAfterForBlockD15 =
                ValidateStmtD2(Ctxt,
                  ErrorHandlerFn,
                  truncatedInput,
                  truncatedInputLength,
                  position);
              uint64_t result1;
              if (EverParseIsSuccess(positionAfterForBlockD15))
              {
                result1 = positionAfterForBlockD15;
              }
              else
              {
                ErrorHandlerFn("_ForBlockD1",
                  "body.element",
                  EverParseErrorReasonOfResult(positionAfterForBlockD15),
                  EverParseGetValidatorErrorKind(positionAfterForBlockD15),
                  Ctxt,
                  truncatedInput,
                  position);
                result1 = positionAfterForBlockD15;
              }
              result = result1;
              ite = EverParseIsError(result1);
            }
            if (ite)
            {
              break;
            }
          }
          uint64_t res2 = result;
          positionAfterForBlockD14 = res2;
        }
        if (EverParseIsSuccess(positionAfterForBlockD14))
        {
          positionAfterForBlockD12 = positionAfterForBlockD14;
        }
        else
        {
          ErrorHandlerFn("_ForBlockD1",
            "body",
            EverParseErrorReasonOfResult(positionAfterForBlockD14),
            EverParseGetValidatorErrorKind(positionAfterForBlockD14),
            Ctxt,
            Input,
            positionAfterbodyLen);
          positionAfterForBlockD12 = positionAfterForBlockD14;
        }
      }
    }
  }
  if (EverParseIsSuccess(positionAfterForBlockD12))
  {
    return positionAfterForBlockD12;
  }
  ErrorHandlerFn("_ForBlockD1",
    "body_count",
    EverParseErrorReasonOfResult(positionAfterForBlockD12),
    EverParseGetValidatorErrorKind(positionAfterForBlockD12),
    Ctxt,
    Input,
    positionAftertoVal);
  return positionAfterForBlockD12;
}

static inline uint64_t
ValidateStmtD1payload(
  uint8_t Tag,
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLen,
  uint64_t StartPosition
)
{
  if (Tag == 1U)
  {
    /* Validating field load_true */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD1payload;
    if (hasBytes)
    {
      positionAfterStmtD1payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD1payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "load_true",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 2U)
  {
    /* Validating field load_false */
    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
    BOOLEAN hasBytes = 1ULL <= (InputLen - StartPosition);
    uint64_t positionAfterStmtD1payload;
    if (hasBytes)
    {
      positionAfterStmtD1payload = StartPosition + 1ULL;
    }
    else
    {
      positionAfterStmtD1payload =
        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
          StartPosition);
    }
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "load_false",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 3U)
  {
    /* Validating field load_int */
    uint64_t positionAfterStmtD1payload = ValidateLoadInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "load_int",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 16U)
  {
    /* Validating field bool_not */
    uint64_t positionAfterStmtD1payload = ValidateBoolNot(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "bool_not",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 17U)
  {
    /* Validating field bool_and */
    uint64_t positionAfterStmtD1payload = ValidateBoolAnd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "bool_and",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 18U)
  {
    /* Validating field bool_or */
    uint64_t positionAfterStmtD1payload = ValidateBoolOr(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "bool_or",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 19U)
  {
    /* Validating field bool_xor */
    uint64_t positionAfterStmtD1payload = ValidateBoolXor(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "bool_xor",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 20U)
  {
    /* Validating field copy_bool */
    uint64_t positionAfterStmtD1payload = ValidateCopyBool(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "copy_bool",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 32U)
  {
    /* Validating field int_add */
    uint64_t positionAfterStmtD1payload = ValidateIntAdd(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "int_add",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 33U)
  {
    /* Validating field int_sub */
    uint64_t positionAfterStmtD1payload = ValidateIntSub(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "int_sub",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 34U)
  {
    /* Validating field int_mul */
    uint64_t positionAfterStmtD1payload = ValidateIntMul(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "int_mul",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 35U)
  {
    /* Validating field copy_int */
    uint64_t positionAfterStmtD1payload = ValidateCopyInt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "copy_int",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 48U)
  {
    /* Validating field cmp_eq */
    uint64_t positionAfterStmtD1payload = ValidateCmpEq(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "cmp_eq",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 49U)
  {
    /* Validating field cmp_ne */
    uint64_t positionAfterStmtD1payload = ValidateCmpNe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "cmp_ne",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 50U)
  {
    /* Validating field cmp_lt */
    uint64_t positionAfterStmtD1payload = ValidateCmpLt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "cmp_lt",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 51U)
  {
    /* Validating field cmp_gt */
    uint64_t positionAfterStmtD1payload = ValidateCmpGt(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "cmp_gt",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 52U)
  {
    /* Validating field cmp_le */
    uint64_t positionAfterStmtD1payload = ValidateCmpLe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "cmp_le",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 53U)
  {
    /* Validating field cmp_ge */
    uint64_t positionAfterStmtD1payload = ValidateCmpGe(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "cmp_ge",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 64U)
  {
    /* Validating field if_block */
    uint64_t
    positionAfterStmtD1payload =
      ValidateIfBlockD1(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "if_block",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 65U)
  {
    /* Validating field for_block */
    uint64_t
    positionAfterStmtD1payload =
      ValidateForBlockD1(Ctxt,
        ErrorHandlerFn,
        Input,
        InputLen,
        StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "for_block",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 80U)
  {
    /* Validating field ton_call */
    uint64_t positionAfterStmtD1payload = ValidateTonCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "ton_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 81U)
  {
    /* Validating field tof_call */
    uint64_t positionAfterStmtD1payload = ValidateTofCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "tof_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 82U)
  {
    /* Validating field tp_call */
    uint64_t positionAfterStmtD1payload = ValidateTpCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "tp_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 96U)
  {
    /* Validating field ctu_call */
    uint64_t positionAfterStmtD1payload = ValidateCtuCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "ctu_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 97U)
  {
    /* Validating field ctd_call */
    uint64_t positionAfterStmtD1payload = ValidateCtdCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "ctd_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 98U)
  {
    /* Validating field ctud_call */
    uint64_t positionAfterStmtD1payload = ValidateCtudCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "ctud_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 112U)
  {
    /* Validating field sr_call */
    uint64_t positionAfterStmtD1payload = ValidateSrCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "sr_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 113U)
  {
    /* Validating field rs_call */
    uint64_t positionAfterStmtD1payload = ValidateRsCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "rs_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 114U)
  {
    /* Validating field rtrig_call */
    uint64_t positionAfterStmtD1payload = ValidateRtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "rtrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  if (Tag == 115U)
  {
    /* Validating field ftrig_call */
    uint64_t positionAfterStmtD1payload = ValidateFtrigCall(InputLen, StartPosition);
    if (EverParseIsSuccess(positionAfterStmtD1payload))
    {
      return positionAfterStmtD1payload;
    }
    ErrorHandlerFn("_StmtD1Payload",
      "ftrig_call",
      EverParseErrorReasonOfResult(positionAfterStmtD1payload),
      EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
      Ctxt,
      Input,
      StartPosition);
    return positionAfterStmtD1payload;
  }
  uint64_t
  positionAfterStmtD1payload =
    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_IMPOSSIBLE,
      StartPosition);
  if (EverParseIsSuccess(positionAfterStmtD1payload))
  {
    return positionAfterStmtD1payload;
  }
  ErrorHandlerFn("_StmtD1Payload",
    "_x_153",
    EverParseErrorReasonOfResult(positionAfterStmtD1payload),
    EverParseGetValidatorErrorKind(positionAfterStmtD1payload),
    Ctxt,
    Input,
    StartPosition);
  return positionAfterStmtD1payload;
}

static inline uint64_t
ValidateStmtD1(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAfterStmtD1;
  if (hasBytes0)
  {
    positionAfterStmtD1 = StartPosition + 1ULL;
  }
  else
  {
    positionAfterStmtD1 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t positionAftertag;
  if (EverParseIsSuccess(positionAfterStmtD1))
  {
    positionAftertag = positionAfterStmtD1;
  }
  else
  {
    ErrorHandlerFn("_StmtD1",
      "tag",
      EverParseErrorReasonOfResult(positionAfterStmtD1),
      EverParseGetValidatorErrorKind(positionAfterStmtD1),
      Ctxt,
      Input,
      StartPosition);
    positionAftertag = positionAfterStmtD1;
  }
  if (EverParseIsError(positionAftertag))
  {
    return positionAftertag;
  }
  uint8_t tag = Input[(uint32_t)StartPosition];
  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
  BOOLEAN hasBytes = 2ULL <= (InputLength - positionAftertag);
  uint64_t positionAfterStmtD10;
  if (hasBytes)
  {
    positionAfterStmtD10 = positionAftertag + 2ULL;
  }
  else
  {
    positionAfterStmtD10 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAftertag);
  }
  uint64_t positionAfterlen;
  if (EverParseIsSuccess(positionAfterStmtD10))
  {
    positionAfterlen = positionAfterStmtD10;
  }
  else
  {
    ErrorHandlerFn("_StmtD1",
      "len",
      EverParseErrorReasonOfResult(positionAfterStmtD10),
      EverParseGetValidatorErrorKind(positionAfterStmtD10),
      Ctxt,
      Input,
      positionAftertag);
    positionAfterlen = positionAfterStmtD10;
  }
  if (EverParseIsError(positionAfterlen))
  {
    return positionAfterlen;
  }
  uint16_t r = Load16Le(Input + (uint32_t)positionAftertag);
  uint16_t len = (uint16_t)(uint32_t)r;
  /* Validating field payload */
  BOOLEAN hasEnoughBytes = (uint64_t)(uint32_t)len <= (InputLength - positionAfterlen);
  uint64_t positionAfterStmtD11;
  if (!hasEnoughBytes)
  {
    positionAfterStmtD11 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        positionAfterlen);
  }
  else
  {
    uint8_t *truncatedInput = Input;
    uint64_t truncatedInputLength = positionAfterlen + (uint64_t)(uint32_t)len;
    uint64_t result = positionAfterlen;
    while (TRUE)
    {
      uint64_t position = result;
      BOOLEAN ite;
      if (!(1ULL <= (truncatedInputLength - position)))
      {
        ite = TRUE;
      }
      else
      {
        uint64_t
        positionAfterStmtD12 =
          ValidateStmtD1payload(tag,
            Ctxt,
            ErrorHandlerFn,
            truncatedInput,
            truncatedInputLength,
            position);
        uint64_t result1;
        if (EverParseIsSuccess(positionAfterStmtD12))
        {
          result1 = positionAfterStmtD12;
        }
        else
        {
          ErrorHandlerFn("_StmtD1",
            "payload.element",
            EverParseErrorReasonOfResult(positionAfterStmtD12),
            EverParseGetValidatorErrorKind(positionAfterStmtD12),
            Ctxt,
            truncatedInput,
            position);
          result1 = positionAfterStmtD12;
        }
        result = result1;
        ite = EverParseIsError(result1);
      }
      if (ite)
      {
        break;
      }
    }
    uint64_t res = result;
    positionAfterStmtD11 = res;
  }
  if (EverParseIsSuccess(positionAfterStmtD11))
  {
    return positionAfterStmtD11;
  }
  ErrorHandlerFn("_StmtD1",
    "payload",
    EverParseErrorReasonOfResult(positionAfterStmtD11),
    EverParseGetValidatorErrorKind(positionAfterStmtD11),
    Ctxt,
    Input,
    positionAfterlen);
  return positionAfterStmtD11;
}

uint64_t
LowPlcTlvValidateLowPlcprogram(
  uint8_t *Ctxt,
  void
  (*ErrorHandlerFn)(
    EVERPARSE_STRING x0,
    EVERPARSE_STRING x1,
    EVERPARSE_STRING x2,
    uint64_t x3,
    uint8_t *x4,
    uint8_t *x5,
    uint64_t x6
  ),
  uint8_t *Input,
  uint64_t InputLength,
  uint64_t StartPosition
)
{
  /* Checking that we have enough space for a UINT8, i.e., 1 byte */
  BOOLEAN hasBytes0 = 1ULL <= (InputLength - StartPosition);
  uint64_t positionAftermagic0;
  if (hasBytes0)
  {
    positionAftermagic0 = StartPosition + 1ULL;
  }
  else
  {
    positionAftermagic0 =
      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
        StartPosition);
  }
  uint64_t positionAfterLowPlcprogram;
  if (EverParseIsError(positionAftermagic0))
  {
    positionAfterLowPlcprogram = positionAftermagic0;
  }
  else
  {
    uint8_t magic0 = Input[(uint32_t)StartPosition];
    BOOLEAN magic0ConstraintIsOk = magic0 == 76U;
    uint64_t
    positionAftermagic01 = EverParseCheckConstraintOk(magic0ConstraintIsOk, positionAftermagic0);
    if (EverParseIsError(positionAftermagic01))
    {
      positionAfterLowPlcprogram = positionAftermagic01;
    }
    else
    {
      /* Checking that we have enough space for a UINT8, i.e., 1 byte */
      BOOLEAN hasBytes1 = 1ULL <= (InputLength - positionAftermagic01);
      uint64_t positionAftermagic1;
      if (hasBytes1)
      {
        positionAftermagic1 = positionAftermagic01 + 1ULL;
      }
      else
      {
        positionAftermagic1 =
          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
            positionAftermagic01);
      }
      uint64_t positionAfterLowPlcprogram0;
      if (EverParseIsError(positionAftermagic1))
      {
        positionAfterLowPlcprogram0 = positionAftermagic1;
      }
      else
      {
        uint8_t magic1 = Input[(uint32_t)positionAftermagic01];
        BOOLEAN magic1ConstraintIsOk = magic1 == 80U;
        uint64_t
        positionAftermagic11 = EverParseCheckConstraintOk(magic1ConstraintIsOk, positionAftermagic1);
        if (EverParseIsError(positionAftermagic11))
        {
          positionAfterLowPlcprogram0 = positionAftermagic11;
        }
        else
        {
          /* Checking that we have enough space for a UINT8, i.e., 1 byte */
          BOOLEAN hasBytes2 = 1ULL <= (InputLength - positionAftermagic11);
          uint64_t positionAfterversion;
          if (hasBytes2)
          {
            positionAfterversion = positionAftermagic11 + 1ULL;
          }
          else
          {
            positionAfterversion =
              EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                positionAftermagic11);
          }
          uint64_t positionAfterLowPlcprogram1;
          if (EverParseIsError(positionAfterversion))
          {
            positionAfterLowPlcprogram1 = positionAfterversion;
          }
          else
          {
            uint8_t version = Input[(uint32_t)positionAftermagic11];
            BOOLEAN versionConstraintIsOk = version == 1U;
            uint64_t
            positionAfterversion1 =
              EverParseCheckConstraintOk(versionConstraintIsOk,
                positionAfterversion);
            if (EverParseIsError(positionAfterversion1))
            {
              positionAfterLowPlcprogram1 = positionAfterversion1;
            }
            else
            {
              /* Validating field flags */
              /* Checking that we have enough space for a UINT8, i.e., 1 byte */
              BOOLEAN hasBytes3 = 1ULL <= (InputLength - positionAfterversion1);
              uint64_t positionAfterLowPlcprogram2;
              if (hasBytes3)
              {
                positionAfterLowPlcprogram2 = positionAfterversion1 + 1ULL;
              }
              else
              {
                positionAfterLowPlcprogram2 =
                  EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                    positionAfterversion1);
              }
              uint64_t res;
              if (EverParseIsSuccess(positionAfterLowPlcprogram2))
              {
                res = positionAfterLowPlcprogram2;
              }
              else
              {
                ErrorHandlerFn("_LowPLCProgram",
                  "flags",
                  EverParseErrorReasonOfResult(positionAfterLowPlcprogram2),
                  EverParseGetValidatorErrorKind(positionAfterLowPlcprogram2),
                  Ctxt,
                  Input,
                  positionAfterversion1);
                res = positionAfterLowPlcprogram2;
              }
              uint64_t positionAfterflags = res;
              if (EverParseIsError(positionAfterflags))
              {
                positionAfterLowPlcprogram1 = positionAfterflags;
              }
              else
              {
                /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                BOOLEAN hasBytes4 = 1ULL <= (InputLength - positionAfterflags);
                uint64_t positionAfternBools;
                if (hasBytes4)
                {
                  positionAfternBools = positionAfterflags + 1ULL;
                }
                else
                {
                  positionAfternBools =
                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                      positionAfterflags);
                }
                uint64_t positionAfterLowPlcprogram3;
                if (EverParseIsError(positionAfternBools))
                {
                  positionAfterLowPlcprogram3 = positionAfternBools;
                }
                else
                {
                  uint8_t nBools = Input[(uint32_t)positionAfterflags];
                  BOOLEAN nBoolsConstraintIsOk = nBools <= 64U;
                  uint64_t
                  positionAfternBools1 =
                    EverParseCheckConstraintOk(nBoolsConstraintIsOk,
                      positionAfternBools);
                  if (EverParseIsError(positionAfternBools1))
                  {
                    positionAfterLowPlcprogram3 = positionAfternBools1;
                  }
                  else
                  {
                    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                    BOOLEAN hasBytes5 = 1ULL <= (InputLength - positionAfternBools1);
                    uint64_t positionAfternInts;
                    if (hasBytes5)
                    {
                      positionAfternInts = positionAfternBools1 + 1ULL;
                    }
                    else
                    {
                      positionAfternInts =
                        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                          positionAfternBools1);
                    }
                    uint64_t positionAfterLowPlcprogram4;
                    if (EverParseIsError(positionAfternInts))
                    {
                      positionAfterLowPlcprogram4 = positionAfternInts;
                    }
                    else
                    {
                      uint8_t nInts = Input[(uint32_t)positionAfternBools1];
                      BOOLEAN nIntsConstraintIsOk = nInts <= 32U;
                      uint64_t
                      positionAfternInts1 =
                        EverParseCheckConstraintOk(nIntsConstraintIsOk,
                          positionAfternInts);
                      if (EverParseIsError(positionAfternInts1))
                      {
                        positionAfterLowPlcprogram4 = positionAfternInts1;
                      }
                      else
                      {
                        /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                        BOOLEAN hasBytes6 = 1ULL <= (InputLength - positionAfternInts1);
                        uint64_t positionAfternTon;
                        if (hasBytes6)
                        {
                          positionAfternTon = positionAfternInts1 + 1ULL;
                        }
                        else
                        {
                          positionAfternTon =
                            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                              positionAfternInts1);
                        }
                        uint64_t positionAfterLowPlcprogram5;
                        if (EverParseIsError(positionAfternTon))
                        {
                          positionAfterLowPlcprogram5 = positionAfternTon;
                        }
                        else
                        {
                          uint8_t nTon = Input[(uint32_t)positionAfternInts1];
                          BOOLEAN nTonConstraintIsOk = nTon <= 8U;
                          uint64_t
                          positionAfternTon1 =
                            EverParseCheckConstraintOk(nTonConstraintIsOk,
                              positionAfternTon);
                          if (EverParseIsError(positionAfternTon1))
                          {
                            positionAfterLowPlcprogram5 = positionAfternTon1;
                          }
                          else
                          {
                            /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                            BOOLEAN hasBytes7 = 1ULL <= (InputLength - positionAfternTon1);
                            uint64_t positionAfternTof;
                            if (hasBytes7)
                            {
                              positionAfternTof = positionAfternTon1 + 1ULL;
                            }
                            else
                            {
                              positionAfternTof =
                                EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                  positionAfternTon1);
                            }
                            uint64_t positionAfterLowPlcprogram6;
                            if (EverParseIsError(positionAfternTof))
                            {
                              positionAfterLowPlcprogram6 = positionAfternTof;
                            }
                            else
                            {
                              uint8_t nTof = Input[(uint32_t)positionAfternTon1];
                              BOOLEAN nTofConstraintIsOk = nTof <= 8U;
                              uint64_t
                              positionAfternTof1 =
                                EverParseCheckConstraintOk(nTofConstraintIsOk,
                                  positionAfternTof);
                              if (EverParseIsError(positionAfternTof1))
                              {
                                positionAfterLowPlcprogram6 = positionAfternTof1;
                              }
                              else
                              {
                                /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                                BOOLEAN hasBytes8 = 1ULL <= (InputLength - positionAfternTof1);
                                uint64_t positionAfternTp;
                                if (hasBytes8)
                                {
                                  positionAfternTp = positionAfternTof1 + 1ULL;
                                }
                                else
                                {
                                  positionAfternTp =
                                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                      positionAfternTof1);
                                }
                                uint64_t positionAfterLowPlcprogram7;
                                if (EverParseIsError(positionAfternTp))
                                {
                                  positionAfterLowPlcprogram7 = positionAfternTp;
                                }
                                else
                                {
                                  uint8_t nTp = Input[(uint32_t)positionAfternTof1];
                                  BOOLEAN nTpConstraintIsOk = nTp <= 8U;
                                  uint64_t
                                  positionAfternTp1 =
                                    EverParseCheckConstraintOk(nTpConstraintIsOk,
                                      positionAfternTp);
                                  if (EverParseIsError(positionAfternTp1))
                                  {
                                    positionAfterLowPlcprogram7 = positionAfternTp1;
                                  }
                                  else
                                  {
                                    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                                    BOOLEAN hasBytes9 = 1ULL <= (InputLength - positionAfternTp1);
                                    uint64_t positionAfternCtu;
                                    if (hasBytes9)
                                    {
                                      positionAfternCtu = positionAfternTp1 + 1ULL;
                                    }
                                    else
                                    {
                                      positionAfternCtu =
                                        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                          positionAfternTp1);
                                    }
                                    uint64_t positionAfterLowPlcprogram8;
                                    if (EverParseIsError(positionAfternCtu))
                                    {
                                      positionAfterLowPlcprogram8 = positionAfternCtu;
                                    }
                                    else
                                    {
                                      uint8_t nCtu = Input[(uint32_t)positionAfternTp1];
                                      BOOLEAN nCtuConstraintIsOk = nCtu <= 8U;
                                      uint64_t
                                      positionAfternCtu1 =
                                        EverParseCheckConstraintOk(nCtuConstraintIsOk,
                                          positionAfternCtu);
                                      if (EverParseIsError(positionAfternCtu1))
                                      {
                                        positionAfterLowPlcprogram8 = positionAfternCtu1;
                                      }
                                      else
                                      {
                                        /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                                        BOOLEAN
                                        hasBytes10 = 1ULL <= (InputLength - positionAfternCtu1);
                                        uint64_t positionAfternCtd;
                                        if (hasBytes10)
                                        {
                                          positionAfternCtd = positionAfternCtu1 + 1ULL;
                                        }
                                        else
                                        {
                                          positionAfternCtd =
                                            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                              positionAfternCtu1);
                                        }
                                        uint64_t positionAfterLowPlcprogram9;
                                        if (EverParseIsError(positionAfternCtd))
                                        {
                                          positionAfterLowPlcprogram9 = positionAfternCtd;
                                        }
                                        else
                                        {
                                          uint8_t nCtd = Input[(uint32_t)positionAfternCtu1];
                                          BOOLEAN nCtdConstraintIsOk = nCtd <= 8U;
                                          uint64_t
                                          positionAfternCtd1 =
                                            EverParseCheckConstraintOk(nCtdConstraintIsOk,
                                              positionAfternCtd);
                                          if (EverParseIsError(positionAfternCtd1))
                                          {
                                            positionAfterLowPlcprogram9 = positionAfternCtd1;
                                          }
                                          else
                                          {
                                            /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                                            BOOLEAN
                                            hasBytes11 = 1ULL <= (InputLength - positionAfternCtd1);
                                            uint64_t positionAfternCtud;
                                            if (hasBytes11)
                                            {
                                              positionAfternCtud = positionAfternCtd1 + 1ULL;
                                            }
                                            else
                                            {
                                              positionAfternCtud =
                                                EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                  positionAfternCtd1);
                                            }
                                            uint64_t positionAfterLowPlcprogram10;
                                            if (EverParseIsError(positionAfternCtud))
                                            {
                                              positionAfterLowPlcprogram10 = positionAfternCtud;
                                            }
                                            else
                                            {
                                              uint8_t nCtud = Input[(uint32_t)positionAfternCtd1];
                                              BOOLEAN nCtudConstraintIsOk = nCtud <= 4U;
                                              uint64_t
                                              positionAfternCtud1 =
                                                EverParseCheckConstraintOk(nCtudConstraintIsOk,
                                                  positionAfternCtud);
                                              if (EverParseIsError(positionAfternCtud1))
                                              {
                                                positionAfterLowPlcprogram10 = positionAfternCtud1;
                                              }
                                              else
                                              {
                                                /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                                                BOOLEAN
                                                hasBytes12 =
                                                  1ULL <= (InputLength - positionAfternCtud1);
                                                uint64_t positionAfternSr;
                                                if (hasBytes12)
                                                {
                                                  positionAfternSr = positionAfternCtud1 + 1ULL;
                                                }
                                                else
                                                {
                                                  positionAfternSr =
                                                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                      positionAfternCtud1);
                                                }
                                                uint64_t positionAfterLowPlcprogram11;
                                                if (EverParseIsError(positionAfternSr))
                                                {
                                                  positionAfterLowPlcprogram11 = positionAfternSr;
                                                }
                                                else
                                                {
                                                  uint8_t
                                                  nSr = Input[(uint32_t)positionAfternCtud1];
                                                  BOOLEAN nSrConstraintIsOk = nSr <= 8U;
                                                  uint64_t
                                                  positionAfternSr1 =
                                                    EverParseCheckConstraintOk(nSrConstraintIsOk,
                                                      positionAfternSr);
                                                  if (EverParseIsError(positionAfternSr1))
                                                  {
                                                    positionAfterLowPlcprogram11 = positionAfternSr1;
                                                  }
                                                  else
                                                  {
                                                    /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                                                    BOOLEAN
                                                    hasBytes13 =
                                                      1ULL <= (InputLength - positionAfternSr1);
                                                    uint64_t positionAfternRs;
                                                    if (hasBytes13)
                                                    {
                                                      positionAfternRs = positionAfternSr1 + 1ULL;
                                                    }
                                                    else
                                                    {
                                                      positionAfternRs =
                                                        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                          positionAfternSr1);
                                                    }
                                                    uint64_t positionAfterLowPlcprogram12;
                                                    if (EverParseIsError(positionAfternRs))
                                                    {
                                                      positionAfterLowPlcprogram12 =
                                                        positionAfternRs;
                                                    }
                                                    else
                                                    {
                                                      uint8_t
                                                      nRs = Input[(uint32_t)positionAfternSr1];
                                                      BOOLEAN nRsConstraintIsOk = nRs <= 8U;
                                                      uint64_t
                                                      positionAfternRs1 =
                                                        EverParseCheckConstraintOk(nRsConstraintIsOk,
                                                          positionAfternRs);
                                                      if (EverParseIsError(positionAfternRs1))
                                                      {
                                                        positionAfterLowPlcprogram12 =
                                                          positionAfternRs1;
                                                      }
                                                      else
                                                      {
                                                        /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                                                        BOOLEAN
                                                        hasBytes14 =
                                                          1ULL <= (InputLength - positionAfternRs1);
                                                        uint64_t positionAfternRtrig;
                                                        if (hasBytes14)
                                                        {
                                                          positionAfternRtrig =
                                                            positionAfternRs1 + 1ULL;
                                                        }
                                                        else
                                                        {
                                                          positionAfternRtrig =
                                                            EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                              positionAfternRs1);
                                                        }
                                                        uint64_t positionAfterLowPlcprogram13;
                                                        if (EverParseIsError(positionAfternRtrig))
                                                        {
                                                          positionAfterLowPlcprogram13 =
                                                            positionAfternRtrig;
                                                        }
                                                        else
                                                        {
                                                          uint8_t
                                                          nRtrig =
                                                            Input[(uint32_t)positionAfternRs1];
                                                          BOOLEAN
                                                          nRtrigConstraintIsOk = nRtrig <= 8U;
                                                          uint64_t
                                                          positionAfternRtrig1 =
                                                            EverParseCheckConstraintOk(nRtrigConstraintIsOk,
                                                              positionAfternRtrig);
                                                          if
                                                          (EverParseIsError(positionAfternRtrig1))
                                                          {
                                                            positionAfterLowPlcprogram13 =
                                                              positionAfternRtrig1;
                                                          }
                                                          else
                                                          {
                                                            /* Checking that we have enough space for a UINT8, i.e., 1 byte */
                                                            BOOLEAN
                                                            hasBytes15 =
                                                              1ULL <=
                                                                (InputLength - positionAfternRtrig1);
                                                            uint64_t positionAfternFtrig;
                                                            if (hasBytes15)
                                                            {
                                                              positionAfternFtrig =
                                                                positionAfternRtrig1 + 1ULL;
                                                            }
                                                            else
                                                            {
                                                              positionAfternFtrig =
                                                                EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                                  positionAfternRtrig1);
                                                            }
                                                            uint64_t positionAfterLowPlcprogram14;
                                                            if
                                                            (EverParseIsError(positionAfternFtrig))
                                                            {
                                                              positionAfterLowPlcprogram14 =
                                                                positionAfternFtrig;
                                                            }
                                                            else
                                                            {
                                                              uint8_t
                                                              nFtrig =
                                                                Input[(uint32_t)positionAfternRtrig1];
                                                              BOOLEAN
                                                              nFtrigConstraintIsOk = nFtrig <= 8U;
                                                              uint64_t
                                                              positionAfternFtrig1 =
                                                                EverParseCheckConstraintOk(nFtrigConstraintIsOk,
                                                                  positionAfternFtrig);
                                                              if
                                                              (
                                                                EverParseIsError(positionAfternFtrig1)
                                                              )
                                                              {
                                                                positionAfterLowPlcprogram14 =
                                                                  positionAfternFtrig1;
                                                              }
                                                              else
                                                              {
                                                                /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
                                                                BOOLEAN
                                                                hasBytes16 =
                                                                  2ULL <=
                                                                    (InputLength -
                                                                      positionAfternFtrig1);
                                                                uint64_t
                                                                positionAfterLowPlcprogram15;
                                                                if (hasBytes16)
                                                                {
                                                                  positionAfterLowPlcprogram15 =
                                                                    positionAfternFtrig1 + 2ULL;
                                                                }
                                                                else
                                                                {
                                                                  positionAfterLowPlcprogram15 =
                                                                    EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                                      positionAfternFtrig1);
                                                                }
                                                                uint64_t positionAfterinitLen;
                                                                if
                                                                (
                                                                  EverParseIsSuccess(positionAfterLowPlcprogram15)
                                                                )
                                                                {
                                                                  positionAfterinitLen =
                                                                    positionAfterLowPlcprogram15;
                                                                }
                                                                else
                                                                {
                                                                  ErrorHandlerFn("_LowPLCProgram",
                                                                    "init_len",
                                                                    EverParseErrorReasonOfResult(positionAfterLowPlcprogram15),
                                                                    EverParseGetValidatorErrorKind(positionAfterLowPlcprogram15),
                                                                    Ctxt,
                                                                    Input,
                                                                    positionAfternFtrig1);
                                                                  positionAfterinitLen =
                                                                    positionAfterLowPlcprogram15;
                                                                }
                                                                if
                                                                (
                                                                  EverParseIsError(positionAfterinitLen)
                                                                )
                                                                {
                                                                  positionAfterLowPlcprogram14 =
                                                                    positionAfterinitLen;
                                                                }
                                                                else
                                                                {
                                                                  uint16_t
                                                                  r0 =
                                                                    Load16Le(Input +
                                                                        (uint32_t)positionAfternFtrig1);
                                                                  uint16_t
                                                                  initLen = (uint16_t)(uint32_t)r0;
                                                                  /* Checking that we have enough space for a UINT16, i.e., 2 bytes */
                                                                  BOOLEAN
                                                                  hasBytes =
                                                                    2ULL <=
                                                                      (InputLength -
                                                                        positionAfterinitLen);
                                                                  uint64_t
                                                                  positionAfterLowPlcprogram16;
                                                                  if (hasBytes)
                                                                  {
                                                                    positionAfterLowPlcprogram16 =
                                                                      positionAfterinitLen + 2ULL;
                                                                  }
                                                                  else
                                                                  {
                                                                    positionAfterLowPlcprogram16 =
                                                                      EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                                        positionAfterinitLen);
                                                                  }
                                                                  uint64_t positionAfterbodyLen;
                                                                  if
                                                                  (
                                                                    EverParseIsSuccess(positionAfterLowPlcprogram16)
                                                                  )
                                                                  {
                                                                    positionAfterbodyLen =
                                                                      positionAfterLowPlcprogram16;
                                                                  }
                                                                  else
                                                                  {
                                                                    ErrorHandlerFn("_LowPLCProgram",
                                                                      "body_len",
                                                                      EverParseErrorReasonOfResult(positionAfterLowPlcprogram16),
                                                                      EverParseGetValidatorErrorKind(positionAfterLowPlcprogram16),
                                                                      Ctxt,
                                                                      Input,
                                                                      positionAfterinitLen);
                                                                    positionAfterbodyLen =
                                                                      positionAfterLowPlcprogram16;
                                                                  }
                                                                  if
                                                                  (
                                                                    EverParseIsError(positionAfterbodyLen)
                                                                  )
                                                                  {
                                                                    positionAfterLowPlcprogram14 =
                                                                      positionAfterbodyLen;
                                                                  }
                                                                  else
                                                                  {
                                                                    uint16_t
                                                                    r =
                                                                      Load16Le(Input +
                                                                          (uint32_t)positionAfterinitLen);
                                                                    uint16_t
                                                                    bodyLen = (uint16_t)(uint32_t)r;
                                                                    /* Validating field init_stmts */
                                                                    BOOLEAN
                                                                    hasEnoughBytes0 =
                                                                      (uint64_t)(uint32_t)initLen <=
                                                                        (InputLength -
                                                                          positionAfterbodyLen);
                                                                    uint64_t
                                                                    positionAfterLowPlcprogram17;
                                                                    if (!hasEnoughBytes0)
                                                                    {
                                                                      positionAfterLowPlcprogram17 =
                                                                        EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                                          positionAfterbodyLen);
                                                                    }
                                                                    else
                                                                    {
                                                                      uint8_t
                                                                      *truncatedInput = Input;
                                                                      uint64_t
                                                                      truncatedInputLength =
                                                                        positionAfterbodyLen +
                                                                          (uint64_t)(uint32_t)initLen;
                                                                      uint64_t
                                                                      result = positionAfterbodyLen;
                                                                      while (TRUE)
                                                                      {
                                                                        uint64_t position = result;
                                                                        BOOLEAN ite;
                                                                        if
                                                                        (
                                                                          !(1ULL <=
                                                                            (truncatedInputLength -
                                                                              position))
                                                                        )
                                                                        {
                                                                          ite = TRUE;
                                                                        }
                                                                        else
                                                                        {
                                                                          uint64_t
                                                                          positionAfterLowPlcprogram18 =
                                                                            ValidateStmtD1(Ctxt,
                                                                              ErrorHandlerFn,
                                                                              truncatedInput,
                                                                              truncatedInputLength,
                                                                              position);
                                                                          uint64_t result1;
                                                                          if
                                                                          (
                                                                            EverParseIsSuccess(positionAfterLowPlcprogram18)
                                                                          )
                                                                          {
                                                                            result1 =
                                                                              positionAfterLowPlcprogram18;
                                                                          }
                                                                          else
                                                                          {
                                                                            ErrorHandlerFn("_LowPLCProgram",
                                                                              "init_stmts.element",
                                                                              EverParseErrorReasonOfResult(positionAfterLowPlcprogram18),
                                                                              EverParseGetValidatorErrorKind(positionAfterLowPlcprogram18),
                                                                              Ctxt,
                                                                              truncatedInput,
                                                                              position);
                                                                            result1 =
                                                                              positionAfterLowPlcprogram18;
                                                                          }
                                                                          result = result1;
                                                                          ite =
                                                                            EverParseIsError(result1);
                                                                        }
                                                                        if (ite)
                                                                        {
                                                                          break;
                                                                        }
                                                                      }
                                                                      uint64_t res0 = result;
                                                                      positionAfterLowPlcprogram17 =
                                                                        res0;
                                                                    }
                                                                    uint64_t positionAfterinitStmts;
                                                                    if
                                                                    (
                                                                      EverParseIsSuccess(positionAfterLowPlcprogram17)
                                                                    )
                                                                    {
                                                                      positionAfterinitStmts =
                                                                        positionAfterLowPlcprogram17;
                                                                    }
                                                                    else
                                                                    {
                                                                      ErrorHandlerFn("_LowPLCProgram",
                                                                        "init_stmts",
                                                                        EverParseErrorReasonOfResult(positionAfterLowPlcprogram17),
                                                                        EverParseGetValidatorErrorKind(positionAfterLowPlcprogram17),
                                                                        Ctxt,
                                                                        Input,
                                                                        positionAfterbodyLen);
                                                                      positionAfterinitStmts =
                                                                        positionAfterLowPlcprogram17;
                                                                    }
                                                                    if
                                                                    (
                                                                      EverParseIsError(positionAfterinitStmts)
                                                                    )
                                                                    {
                                                                      positionAfterLowPlcprogram14 =
                                                                        positionAfterinitStmts;
                                                                    }
                                                                    else
                                                                    {
                                                                      /* Validating field body_stmts */
                                                                      BOOLEAN
                                                                      hasEnoughBytes =
                                                                        (uint64_t)(uint32_t)bodyLen
                                                                        <=
                                                                          (InputLength -
                                                                            positionAfterinitStmts);
                                                                      uint64_t
                                                                      positionAfterLowPlcprogram18;
                                                                      if (!hasEnoughBytes)
                                                                      {
                                                                        positionAfterLowPlcprogram18
                                                                        =
                                                                          EverParseSetValidatorErrorPos(EVERPARSE_VALIDATOR_ERROR_NOT_ENOUGH_DATA,
                                                                            positionAfterinitStmts);
                                                                      }
                                                                      else
                                                                      {
                                                                        uint8_t
                                                                        *truncatedInput = Input;
                                                                        uint64_t
                                                                        truncatedInputLength =
                                                                          positionAfterinitStmts +
                                                                            (uint64_t)(uint32_t)bodyLen;
                                                                        uint64_t
                                                                        result =
                                                                          positionAfterinitStmts;
                                                                        while (TRUE)
                                                                        {
                                                                          uint64_t
                                                                          position = result;
                                                                          BOOLEAN ite;
                                                                          if
                                                                          (
                                                                            !(1ULL <=
                                                                              (truncatedInputLength
                                                                              - position))
                                                                          )
                                                                          {
                                                                            ite = TRUE;
                                                                          }
                                                                          else
                                                                          {
                                                                            uint64_t
                                                                            positionAfterLowPlcprogram19 =
                                                                              ValidateStmtD1(Ctxt,
                                                                                ErrorHandlerFn,
                                                                                truncatedInput,
                                                                                truncatedInputLength,
                                                                                position);
                                                                            uint64_t result1;
                                                                            if
                                                                            (
                                                                              EverParseIsSuccess(positionAfterLowPlcprogram19)
                                                                            )
                                                                            {
                                                                              result1 =
                                                                                positionAfterLowPlcprogram19;
                                                                            }
                                                                            else
                                                                            {
                                                                              ErrorHandlerFn("_LowPLCProgram",
                                                                                "body_stmts.element",
                                                                                EverParseErrorReasonOfResult(positionAfterLowPlcprogram19),
                                                                                EverParseGetValidatorErrorKind(positionAfterLowPlcprogram19),
                                                                                Ctxt,
                                                                                truncatedInput,
                                                                                position);
                                                                              result1 =
                                                                                positionAfterLowPlcprogram19;
                                                                            }
                                                                            result = result1;
                                                                            ite =
                                                                              EverParseIsError(result1);
                                                                          }
                                                                          if (ite)
                                                                          {
                                                                            break;
                                                                          }
                                                                        }
                                                                        uint64_t res0 = result;
                                                                        positionAfterLowPlcprogram18
                                                                        = res0;
                                                                      }
                                                                      if
                                                                      (
                                                                        EverParseIsSuccess(positionAfterLowPlcprogram18)
                                                                      )
                                                                      {
                                                                        positionAfterLowPlcprogram14
                                                                        =
                                                                          positionAfterLowPlcprogram18;
                                                                      }
                                                                      else
                                                                      {
                                                                        ErrorHandlerFn("_LowPLCProgram",
                                                                          "body_stmts",
                                                                          EverParseErrorReasonOfResult(positionAfterLowPlcprogram18),
                                                                          EverParseGetValidatorErrorKind(positionAfterLowPlcprogram18),
                                                                          Ctxt,
                                                                          Input,
                                                                          positionAfterinitStmts);
                                                                        positionAfterLowPlcprogram14
                                                                        =
                                                                          positionAfterLowPlcprogram18;
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              }
                                                            }
                                                            if
                                                            (
                                                              EverParseIsSuccess(positionAfterLowPlcprogram14)
                                                            )
                                                            {
                                                              positionAfterLowPlcprogram13 =
                                                                positionAfterLowPlcprogram14;
                                                            }
                                                            else
                                                            {
                                                              ErrorHandlerFn("_LowPLCProgram",
                                                                "n_ftrig",
                                                                EverParseErrorReasonOfResult(positionAfterLowPlcprogram14),
                                                                EverParseGetValidatorErrorKind(positionAfterLowPlcprogram14),
                                                                Ctxt,
                                                                Input,
                                                                positionAfternRtrig1);
                                                              positionAfterLowPlcprogram13 =
                                                                positionAfterLowPlcprogram14;
                                                            }
                                                          }
                                                        }
                                                        if
                                                        (
                                                          EverParseIsSuccess(positionAfterLowPlcprogram13)
                                                        )
                                                        {
                                                          positionAfterLowPlcprogram12 =
                                                            positionAfterLowPlcprogram13;
                                                        }
                                                        else
                                                        {
                                                          ErrorHandlerFn("_LowPLCProgram",
                                                            "n_rtrig",
                                                            EverParseErrorReasonOfResult(positionAfterLowPlcprogram13),
                                                            EverParseGetValidatorErrorKind(positionAfterLowPlcprogram13),
                                                            Ctxt,
                                                            Input,
                                                            positionAfternRs1);
                                                          positionAfterLowPlcprogram12 =
                                                            positionAfterLowPlcprogram13;
                                                        }
                                                      }
                                                    }
                                                    if
                                                    (
                                                      EverParseIsSuccess(positionAfterLowPlcprogram12)
                                                    )
                                                    {
                                                      positionAfterLowPlcprogram11 =
                                                        positionAfterLowPlcprogram12;
                                                    }
                                                    else
                                                    {
                                                      ErrorHandlerFn("_LowPLCProgram",
                                                        "n_rs",
                                                        EverParseErrorReasonOfResult(positionAfterLowPlcprogram12),
                                                        EverParseGetValidatorErrorKind(positionAfterLowPlcprogram12),
                                                        Ctxt,
                                                        Input,
                                                        positionAfternSr1);
                                                      positionAfterLowPlcprogram11 =
                                                        positionAfterLowPlcprogram12;
                                                    }
                                                  }
                                                }
                                                if
                                                (EverParseIsSuccess(positionAfterLowPlcprogram11))
                                                {
                                                  positionAfterLowPlcprogram10 =
                                                    positionAfterLowPlcprogram11;
                                                }
                                                else
                                                {
                                                  ErrorHandlerFn("_LowPLCProgram",
                                                    "n_sr",
                                                    EverParseErrorReasonOfResult(positionAfterLowPlcprogram11),
                                                    EverParseGetValidatorErrorKind(positionAfterLowPlcprogram11),
                                                    Ctxt,
                                                    Input,
                                                    positionAfternCtud1);
                                                  positionAfterLowPlcprogram10 =
                                                    positionAfterLowPlcprogram11;
                                                }
                                              }
                                            }
                                            if (EverParseIsSuccess(positionAfterLowPlcprogram10))
                                            {
                                              positionAfterLowPlcprogram9 =
                                                positionAfterLowPlcprogram10;
                                            }
                                            else
                                            {
                                              ErrorHandlerFn("_LowPLCProgram",
                                                "n_ctud",
                                                EverParseErrorReasonOfResult(positionAfterLowPlcprogram10),
                                                EverParseGetValidatorErrorKind(positionAfterLowPlcprogram10),
                                                Ctxt,
                                                Input,
                                                positionAfternCtd1);
                                              positionAfterLowPlcprogram9 =
                                                positionAfterLowPlcprogram10;
                                            }
                                          }
                                        }
                                        if (EverParseIsSuccess(positionAfterLowPlcprogram9))
                                        {
                                          positionAfterLowPlcprogram8 = positionAfterLowPlcprogram9;
                                        }
                                        else
                                        {
                                          ErrorHandlerFn("_LowPLCProgram",
                                            "n_ctd",
                                            EverParseErrorReasonOfResult(positionAfterLowPlcprogram9),
                                            EverParseGetValidatorErrorKind(positionAfterLowPlcprogram9),
                                            Ctxt,
                                            Input,
                                            positionAfternCtu1);
                                          positionAfterLowPlcprogram8 = positionAfterLowPlcprogram9;
                                        }
                                      }
                                    }
                                    if (EverParseIsSuccess(positionAfterLowPlcprogram8))
                                    {
                                      positionAfterLowPlcprogram7 = positionAfterLowPlcprogram8;
                                    }
                                    else
                                    {
                                      ErrorHandlerFn("_LowPLCProgram",
                                        "n_ctu",
                                        EverParseErrorReasonOfResult(positionAfterLowPlcprogram8),
                                        EverParseGetValidatorErrorKind(positionAfterLowPlcprogram8),
                                        Ctxt,
                                        Input,
                                        positionAfternTp1);
                                      positionAfterLowPlcprogram7 = positionAfterLowPlcprogram8;
                                    }
                                  }
                                }
                                if (EverParseIsSuccess(positionAfterLowPlcprogram7))
                                {
                                  positionAfterLowPlcprogram6 = positionAfterLowPlcprogram7;
                                }
                                else
                                {
                                  ErrorHandlerFn("_LowPLCProgram",
                                    "n_tp",
                                    EverParseErrorReasonOfResult(positionAfterLowPlcprogram7),
                                    EverParseGetValidatorErrorKind(positionAfterLowPlcprogram7),
                                    Ctxt,
                                    Input,
                                    positionAfternTof1);
                                  positionAfterLowPlcprogram6 = positionAfterLowPlcprogram7;
                                }
                              }
                            }
                            if (EverParseIsSuccess(positionAfterLowPlcprogram6))
                            {
                              positionAfterLowPlcprogram5 = positionAfterLowPlcprogram6;
                            }
                            else
                            {
                              ErrorHandlerFn("_LowPLCProgram",
                                "n_tof",
                                EverParseErrorReasonOfResult(positionAfterLowPlcprogram6),
                                EverParseGetValidatorErrorKind(positionAfterLowPlcprogram6),
                                Ctxt,
                                Input,
                                positionAfternTon1);
                              positionAfterLowPlcprogram5 = positionAfterLowPlcprogram6;
                            }
                          }
                        }
                        if (EverParseIsSuccess(positionAfterLowPlcprogram5))
                        {
                          positionAfterLowPlcprogram4 = positionAfterLowPlcprogram5;
                        }
                        else
                        {
                          ErrorHandlerFn("_LowPLCProgram",
                            "n_ton",
                            EverParseErrorReasonOfResult(positionAfterLowPlcprogram5),
                            EverParseGetValidatorErrorKind(positionAfterLowPlcprogram5),
                            Ctxt,
                            Input,
                            positionAfternInts1);
                          positionAfterLowPlcprogram4 = positionAfterLowPlcprogram5;
                        }
                      }
                    }
                    if (EverParseIsSuccess(positionAfterLowPlcprogram4))
                    {
                      positionAfterLowPlcprogram3 = positionAfterLowPlcprogram4;
                    }
                    else
                    {
                      ErrorHandlerFn("_LowPLCProgram",
                        "n_ints",
                        EverParseErrorReasonOfResult(positionAfterLowPlcprogram4),
                        EverParseGetValidatorErrorKind(positionAfterLowPlcprogram4),
                        Ctxt,
                        Input,
                        positionAfternBools1);
                      positionAfterLowPlcprogram3 = positionAfterLowPlcprogram4;
                    }
                  }
                }
                if (EverParseIsSuccess(positionAfterLowPlcprogram3))
                {
                  positionAfterLowPlcprogram1 = positionAfterLowPlcprogram3;
                }
                else
                {
                  ErrorHandlerFn("_LowPLCProgram",
                    "n_bools",
                    EverParseErrorReasonOfResult(positionAfterLowPlcprogram3),
                    EverParseGetValidatorErrorKind(positionAfterLowPlcprogram3),
                    Ctxt,
                    Input,
                    positionAfterflags);
                  positionAfterLowPlcprogram1 = positionAfterLowPlcprogram3;
                }
              }
            }
          }
          if (EverParseIsSuccess(positionAfterLowPlcprogram1))
          {
            positionAfterLowPlcprogram0 = positionAfterLowPlcprogram1;
          }
          else
          {
            ErrorHandlerFn("_LowPLCProgram",
              "version",
              EverParseErrorReasonOfResult(positionAfterLowPlcprogram1),
              EverParseGetValidatorErrorKind(positionAfterLowPlcprogram1),
              Ctxt,
              Input,
              positionAftermagic11);
            positionAfterLowPlcprogram0 = positionAfterLowPlcprogram1;
          }
        }
      }
      if (EverParseIsSuccess(positionAfterLowPlcprogram0))
      {
        positionAfterLowPlcprogram = positionAfterLowPlcprogram0;
      }
      else
      {
        ErrorHandlerFn("_LowPLCProgram",
          "magic1",
          EverParseErrorReasonOfResult(positionAfterLowPlcprogram0),
          EverParseGetValidatorErrorKind(positionAfterLowPlcprogram0),
          Ctxt,
          Input,
          positionAftermagic01);
        positionAfterLowPlcprogram = positionAfterLowPlcprogram0;
      }
    }
  }
  if (EverParseIsSuccess(positionAfterLowPlcprogram))
  {
    return positionAfterLowPlcprogram;
  }
  ErrorHandlerFn("_LowPLCProgram",
    "magic0",
    EverParseErrorReasonOfResult(positionAfterLowPlcprogram),
    EverParseGetValidatorErrorKind(positionAfterLowPlcprogram),
    Ctxt,
    Input,
    StartPosition);
  return positionAfterLowPlcprogram;
}

